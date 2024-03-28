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
#include <sys/stat.h>

using namespace std;

struct ClientState {
    int lastInOrderSeqNum = -1;
};

string getCurrentTimestamp() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);

    stringstream ss;
    ss << put_time(gmtime(&in_time_t), "%FT%TZ");
    return ss.str();
}

void logEvent(const string& clientIP, int clientPort, const string& eventType, int packetSN, int serverPort) {
    string timestamp = getCurrentTimestamp();
    cout << timestamp << ", " << serverPort << ", " << clientIP << ", " << clientPort << ", " << eventType << ", " << packetSN << endl;
}

// Helper function to create directory recursively
void createDirectory(const string& path) {
    size_t pos = 0;
    string dirPath;
    while ((pos = path.find('/', pos)) != string::npos) {
        dirPath = path.substr(0, pos++);
        if (!dirPath.empty()) {
            mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
    }
}

// Generate a unique identifier for the client
string generateClientKey(const sockaddr_in& clientAddr) {
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddr.sin_port);
    return string(clientIP) + ":" + to_string(clientPort);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <port> <droppc> <root_folder_path>" << endl;
        return -1;
    }

    int port = atoi(argv[1]);
    int droppc = atoi(argv[2]);
    string rootFolder = argv[3];
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[65535]; // 与客户端BUFFER_SIZE相同
    socklen_t clientLen = sizeof(clientAddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        cerr << "Error creating socket" << endl;
        return -1;
    }

    string clientIP = inet_ntoa(clientAddr.sin_addr);
    int clientPort = ntohs(clientAddr.sin_port);

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Bind failed" << endl;
        return -1;
    }

    // cout << "Server is running on port " << port << "..." << endl;

    srand(time(NULL));

    map<string, ClientState> clients;

    while (true) {
        ssize_t bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &clientLen);
        if (bytes_received < 0) {
            cerr << "Error receiving message" << endl;
            continue;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);

        string clientKey = generateClientKey(clientAddr);
        if (clients.find(clientKey) == clients.end()) {
            // If it is a new client, initialize its state
            clients[clientKey] = ClientState();
        }

        ClientState& state = clients[clientKey];

        stringstream ss(string(buffer, bytes_received));
        string seqNumStr, payloadSizeStr, outFilePath;
        getline(ss, seqNumStr, '\r');
        ss.ignore(1); // Skip '\n'
        getline(ss, payloadSizeStr, '\r');
        ss.ignore(1); // Skip '\n'
        getline(ss, outFilePath, '\r');
        ss.ignore(1); // Skip '\n'

        // print the whole message
        // cout << "Received: " << buffer << endl;

        int seqNum = stoi(seqNumStr);
        int payloadSize = stoi(payloadSizeStr);

        int dropChance = rand() % 100;
        if (dropChance < droppc) {
            // cout << "Packet dropped intentionally" << endl;
            logEvent(clientIP, clientPort, "DROP", seqNum, port);
            continue;
        }

        // cout << "Received: SeqNum=" << seqNum << ", PayloadSize=" << payloadSize << ", Path=" << outFilePath << endl;

        // Ensure the output directory exists
        string fullPath = rootFolder + "/" + outFilePath;
        createDirectory(fullPath.substr(0, fullPath.find_last_of('/')));

        // Check if seqNum is equal to lastInOrderSeqNum + 1
        // cout << "Received: SeqNum=" << seqNum << "state.lastInOrderSeqNum=" << state.lastInOrderSeqNum << endl;
        if (seqNum == state.lastInOrderSeqNum + 1) {
            // Select the open mode based on whether it is the first write
            ios_base::openmode fileMode = ios::binary;
            if (state.lastInOrderSeqNum != -1) {
                // If this is not the first write, use append mode
                fileMode |= ios::app;
            }
            // cout << "Writing to file: " << fullPath << endl;
            ofstream outputFile(fullPath, fileMode);
            if (outputFile.is_open()) {
                outputFile.write(buffer + ss.tellg(), payloadSize);
                state.lastInOrderSeqNum = seqNum; // 更新序列号
            }
            outputFile.close();
        }

        // Send ACK
        string ackMsg = "ACK " + to_string(state.lastInOrderSeqNum);
        logEvent(clientIP, clientPort, "ACK", state.lastInOrderSeqNum, port);
        // cout << "Sending: " << ackMsg << endl;
        sendto(sockfd, ackMsg.c_str(), ackMsg.length(), 0, (struct sockaddr *)&clientAddr, clientLen);
    }

    close(sockfd);
    return 0;
}
