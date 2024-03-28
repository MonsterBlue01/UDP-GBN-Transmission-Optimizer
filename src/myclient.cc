#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <vector>
#include <thread>
#include <map>
#include <fstream>
#include <fcntl.h>
#include <mutex>

using namespace std;
using ServerConfig = vector<pair<string, int>>;

mutex logMutex;
mutex threadStatusMutex;

bool threadErrorOccurred = false;

string getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);

    stringstream ss;
    ss << put_time(gmtime(&in_time_t), "%FT%TZ");
    return ss.str();
}

void logEvent(int localPort, const string& serverIP, int serverPort, const string& eventType, int packetSN) {
    lock_guard<mutex> guard(logMutex);
    
    string timestamp = getCurrentTimestamp();
    cout << timestamp << ", " << localPort << ", " << serverIP << ", " << serverPort << ", " << eventType << ", " << packetSN << endl;
}

ServerConfig parseServerConfig(const string& filePath) {
    ServerConfig serverConfig;
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Failed to open server config file" << endl;
        return serverConfig;
    }

    string line;
    while (getline(file, line)) {
        // Skip comment lines and blank lines
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        string ip;
        int port;
        if (ss >> ip >> port) {
            // cout << "Parsed Server: " << ip << ":" << port << endl;
            serverConfig.push_back({ip, port});
        } else {
            cerr << "Invalid line in config file: " << line << endl;
        }
    }
    file.close();
    return serverConfig;
}

// Modify the preparePacket function signature, including the mtu parameter
vector<char> preparePacket(int sequenceNumber, const string& out_file_path, const vector<char>& payload, int mtu) {
    vector<char> packet;
    string header = to_string(sequenceNumber) + "\r\n" + to_string(payload.size()) + "\r\n" + out_file_path + "\r\n";
    if (header.size() + payload.size() + 4 > mtu) {
        cerr << "Packet size exceeds MTU" << endl;
        return packet;
    }
    packet.insert(packet.end(), header.begin(), header.end());
    packet.insert(packet.end(), payload.begin(), payload.end());
    string ending = "\r\n\r\n";
    packet.insert(packet.end(), ending.begin(), ending.end());
    return packet;
}

bool checkTimeouts(int& base, int nextSeqNum, vector<vector<char>>& window, const string& outFilePath, int sockfd, const sockaddr_in& serverAddr, map<int, chrono::steady_clock::time_point>& sendTimes, map<int, int>& retries, int mtu, int N, int localPort, const string& serverIP, int serverPort) {
    auto now = chrono::steady_clock::now();
    for (int i = base; i < nextSeqNum; ++i) {
        if (sendTimes.find(i) != sendTimes.end() && now - sendTimes[i] > chrono::seconds(1)) {
            vector<char>& buffer = window[i % N];
            vector<char> packet = preparePacket(i, outFilePath, buffer, mtu);
            if (sendto(sockfd, packet.data(), packet.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                logEvent(localPort, serverIP, serverPort, "DATA", i);
                cerr << "Error re-sending packet " << i << endl;
            } else {
                sendTimes[i] = chrono::steady_clock::now();
                retries[i]++;
                // cout << "Retransmitting packet " << i << " due to timeout, retry #" << retries[i] << endl;
                if (retries[i] > 4) {
                    cerr << "Maximum retry attempts reached for packet " << i << ". Terminating." << endl;
                    return false; // Indicate to stop the thread
                }
            }
        }
    }
    return true; // Indicate to continue running the thread
}

void sendFile(const string& fileName, const string& serverIP, int serverPort, const string& outFilePath, int mtu, int winsz) {
    int sockfd;
    struct sockaddr_in serverAddr;
    int base = 0;
    int nextSeqNum = 0;
    map<int, chrono::steady_clock::time_point> sendTimes;
    map<int, int> retries;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        cerr << "Error creating socket" << endl;
        threadStatusMutex.lock();
        threadErrorOccurred = true;
        threadStatusMutex.unlock();
        return;
    }

    // Get the local port number
    struct sockaddr_in localAddr;
    socklen_t addrLen = sizeof(localAddr);
    memset(&localAddr, 0, sizeof(localAddr));
    if (getsockname(sockfd, (struct sockaddr *)&localAddr, &addrLen) == -1) {
        cerr << "Error getting socket name" << endl;
        threadStatusMutex.lock();
        threadErrorOccurred = true;
        threadStatusMutex.unlock();
        return;
    }
    int localPort = ntohs(localAddr.sin_port);

    // Set the socket to non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags < 0) {
        cerr << "Error getting socket flags" << endl;
        threadStatusMutex.lock();
        threadErrorOccurred = true;
        threadStatusMutex.unlock();
        return;
    }
    flags = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    if (flags < 0) {
        cerr << "Error setting socket to non-blocking" << endl;
        threadStatusMutex.lock();
        threadErrorOccurred = true;
        threadStatusMutex.unlock();
        return;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

    ifstream file(fileName, ios::binary);
    if (!file.is_open()) {
        cerr << "Failed to open file" << endl;
        threadStatusMutex.lock();
        threadErrorOccurred = true;
        threadStatusMutex.unlock();
        return;
    }

    int N = winsz / mtu;
    // Cache data packets within the window
    vector<vector<char>> window(N);
    auto startTime = chrono::steady_clock::now();

    while (!file.eof()) {
        while (nextSeqNum < base + N && !file.eof()) {
            vector<char>& buffer = window[nextSeqNum % N];
            // Use mtu to determine payload size, ensuring that the total size including header information does not exceed mtu
            int headerAndFooterSize = 50; // Estimate header and trailer sizes, may need to be adjusted
            int maxPayloadSize = mtu - headerAndFooterSize;
            buffer.resize(maxPayloadSize);
            file.read(buffer.data(), maxPayloadSize);
            int bytesRead = file.gcount();
            buffer.resize(bytesRead);
            
            vector<char> packet = preparePacket(nextSeqNum, outFilePath, buffer, mtu);
            // cout << "Sending packet " << nextSeqNum << " with " << bytesRead << " bytes" << endl;
            if (sendto(sockfd, packet.data(), packet.size(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
                cerr << "Error sending packet " << nextSeqNum << endl;
                threadStatusMutex.lock();
                threadErrorOccurred = true;
                threadStatusMutex.unlock();
                return;
            } else {
                logEvent(localPort, serverIP, serverPort, "DATA", nextSeqNum);
                sendTimes[nextSeqNum] = chrono::steady_clock::now();
                retries[nextSeqNum] = 0;
                nextSeqNum++;
            }
        }

        // Use a loop to wait for ACK non-blockingly and check for timeout
        bool keepChecking = true;
        while (keepChecking) {
            auto now = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(now - startTime).count();
            checkTimeouts(base, nextSeqNum, window, outFilePath, sockfd, serverAddr, sendTimes, retries, mtu, N, localPort, serverIP, serverPort);

            bool shouldContinue = checkTimeouts(base, nextSeqNum, window, outFilePath, sockfd, serverAddr, sendTimes, retries, mtu, N, localPort, serverIP, serverPort);
            if (!shouldContinue) {
                close(sockfd);
                threadStatusMutex.lock();
                threadErrorOccurred = true;
                threadStatusMutex.unlock();
                return; // Cleanly exit the thread
            }

            if (elapsed > 30) {
                cerr << "Timeout, no ACK received for 30 seconds. Terminating thread for " << serverIP << ":" << serverPort << endl;
                close(sockfd);
                threadStatusMutex.lock();
                threadErrorOccurred = true;
                threadStatusMutex.unlock();
                return;
            }

            char ack[1024];
            socklen_t len = sizeof(serverAddr);
            int receivedBytes = recvfrom(sockfd, ack, sizeof(ack), 0, (struct sockaddr *)&serverAddr, &len);
            if (receivedBytes >= 0) {
                startTime = chrono::steady_clock::now();
                int ackNum = atoi(ack + 4); // Assuming ACK format is "ACK [number]"
                logEvent(localPort, serverIP, serverPort, "ACK", ackNum);
                if (ackNum >= base) { // Move window if ACK is for the base packet or beyond
                    base = ackNum + 1;
                    // cout << "Window moved to: " << base << endl;
                }
                retries.erase(ackNum);
            } else if (errno == EWOULDBLOCK || errno == EAGAIN) {
                this_thread::sleep_for(chrono::milliseconds(100));
            } else {
                cerr << "Error receiving ACK" << endl;
                break;
            }
            
            keepChecking = base < nextSeqNum; // If the base sequence number has reached the next sequence number, stop checking
        }
    }

    file.close();
    close(sockfd);
}

int main(int argc, char* argv[]) {
    // Modify the main function to receive the MTU parameter and pass it to the sendFile function
    if (argc != 7) {
        cerr << "Usage: " << argv[0] << "<servn> <servaddr.conf> <mtu> <winsz> <in_file_path> <out_file_path>" << endl;
        return -1;
    }

    int servn = stoi(argv[1]);
    string servAddrConf = argv[2];
    int mtu = stoi(argv[3]);
    int winsz = stoi(argv[4]);
    string fileName = argv[5];

    // cout << servAddrConf << endl;

    vector<thread> threads;

    if (mtu < 50 || winsz < mtu) {
        cerr << "Invalid MTU or window size" << endl;
        return -1;
    }

    auto serverConfigs = parseServerConfig(servAddrConf);
    for (const auto& [ip, port] : serverConfigs) {
        // cout << "Sending file to " << ip << ":" << port << "..." << endl;
        threads.push_back(thread(sendFile, fileName, ip, port, argv[6], mtu, winsz));
    }

    for (auto& t : threads) {
        t.join();
    }   

    threadStatusMutex.lock();
    bool errorOccurred = threadErrorOccurred;
    threadStatusMutex.unlock();

    return errorOccurred ? 1 : 0;
}