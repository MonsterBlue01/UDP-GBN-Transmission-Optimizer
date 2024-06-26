# The first test (Simple test)
cd ..
make
cd bin
./myserver 8080 5 ./ > /dev/null 2>&1 &
SERVER_PID=$!

TEST1_RESULT=0
TEST2_RESULT=0

# Carry out the first part of the test
if [ -f "received_file" ]; then
    rm received_file
fi

./myclient 1 ../src/servaddr.conf 512 1025 ../download.png received_file > /dev/null 2>&1
diff received_file ../download.png > /dev/null
if [ $? -eq 0 ]; then
    echo "Test 1 Part 1: Passed"
else
    echo "Test 1 Part 1: Failed"
    TEST1_RESULT=1
fi

# Carry out the second part of the test
if [ -f "received_file2" ]; then
    rm received_file2
fi

./myclient 1 ../src/servaddr.conf 512 1025 ../test.txt received_file2 > /dev/null 2>&1
diff received_file2 ../test.txt > /dev/null
if [ $? -eq 0 ]; then
    echo "Test 1 Part 2: Passed"
else
    echo "Test 1 Part 2: Failed"
    TEST2_RESULT=1
fi

# End server process
kill $SERVER_PID

# Clean the files generated by the test
rm -f received_file received_file2

# Give final feedback based on test results
if [ $TEST1_RESULT -eq 0 ] && [ $TEST2_RESULT -eq 0 ]; then
    echo "Test 1: All parts passed."
else
    echo "Test 1: Some parts failed."
fi

# Second test
cd .. && cd src

cp servaddr.conf servaddr.conf.bak
# Modify the contents of the servaddr.conf file
echo "# IP address port" > ../src/servaddr.conf
echo "127.0.0.1 8080" >> ../src/servaddr.conf
echo "127.0.0.1 8081" >> ../src/servaddr.conf
echo "127.0.0.1 8082" >> ../src/servaddr.conf
echo "127.0.0.1 8083" >> ../src/servaddr.conf
echo "127.0.0.1 8084" >> ../src/servaddr.conf

cd .. && cd bin
./myserver 8080 5 ./tmp/ > /dev/null 2>&1 &
SERVER_PID=$!
./myserver 8081 5 ./ > /dev/null 2>&1 &
SERVER_PID2=$!
./myserver 8082 5 ./tmp1/ > /dev/null 2>&1 &
SERVER_PID3=$!
./myserver 8083 5 ./tmp2/ > /dev/null 2>&1 &
SERVER_PID4=$!
./myserver 8084 5 ./tmp3/ > /dev/null 2>&1 &
SERVER_PID5=$!

./myclient 5 ../src/servaddr.conf 512 1025 ../download.png received_file > /dev/null 2>&1

diff received_file ../download.png
if [ $? -eq 0 ]; then
    echo "Test 2 Part 1: Passed"
else
    echo "Test 2 Part 1: Failed"
fi

diff ./tmp/received_file ../download.png
if [ $? -eq 0 ]; then
    echo "Test 2 Part 2: Passed"
else
    echo "Test 2 Part 2: Failed"
fi

diff ./tmp1/received_file ../download.png
if [ $? -eq 0 ]; then
    echo "Test 2 Part 3: Passed"
else
    echo "Test 2 Part 3: Failed"
fi

diff ./tmp2/received_file ../download.png
if [ $? -eq 0 ]; then
    echo "Test 2 Part 4: Passed"
else
    echo "Test 2 Part 4: Failed"
fi

diff ./tmp3/received_file ../download.png
if [ $? -eq 0 ]; then
    echo "Test 2 Part 5: Passed"
else
    echo "Test 2 Part 5: Failed"
fi

kill $SERVER_PID $SERVER_PID2 $SERVER_PID3 $SERVER_PID4 $SERVER_PID5
rm -f received_file
rm -rf ./tmp ./tmp1 ./tmp2 ./tmp3

cd .. && cd src
mv servaddr.conf.bak servaddr.conf

cd .. && cd bin
# The third test (test the situation when the server cannot be connected)
./myclient 1 ../src/servaddr.conf 512 1025 ../download.png received_file > /dev/null 2>&1
CLIENT_EXIT_STATUS=$?

sleep 5

if [ $CLIENT_EXIT_STATUS -eq 1 ]; then
    echo "Test 3: Passed - Client reported an error as expected."
else
    echo "Test 3: Failed - Client did not report an error as expected."
fi

# The fourth test (one server corresponds to multiple clients)
./myserver 8080 5 ./ > /dev/null 2>&1 &
SERVER_PID=$!

./myclient 1 ../src/servaddr.conf 512 1025 ../download.png received_file > /dev/null 2>&1 &
./myclient 1 ../src/servaddr.conf 512 1025 ../test.txt received_file2 > /dev/null 2>&1 &

sleep 7

diff received_file ../download.png > diff.txt
if [ $? -eq 0 ]; then
    echo "Test 4 Part 1: Passed"
else
    echo "Test 4 Part 1: Failed"
fi

diff received_file2 ../test.txt
if [ $? -eq 0 ]; then
    echo "Test 4 Part 2: Passed"
else
    echo "Test 4 Part 2: Failed"
fi

rm -f received_file received_file2

kill $SERVER_PID

# Fifth test (large binaries)
# ./myserver 8080 5 ./ > /dev/null 2>&1 &
# SERVER_PID=$!

# ./myclient 1 ../src/servaddr.conf 50000 60000 ../100MB.bin received_file > /dev/null 2>&1

# diff received_file ../100MB.bin

# if [ $? -eq 0 ]; then
#     echo "Test 5: Passed"
# else
#     echo "Test 5: Failed"
# fi

# rm -f received_file

cd ..
make clean
