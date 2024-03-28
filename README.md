# README for Lab 4

## Project Overview
This project aims to enhance file transfer reliability and redundancy using UDP. It involves replicating a local file across multiple servers, each of which will reassemble and store the file from the client's packets. Despite potential packet loss or reordering, the system ensures the accurate and complete transmission of files. The client segments the file into packets for transmission, while servers save these packets locally. The project tests the system's resilience against packet loss, requiring the client to manage packet retransmission and ensure successful file delivery across all servers.

## Notice:
I've included the Shell Script source code for testing in my src directory, along with several test files such as `download.png` and `test.txt`. Additionally, I sourced a file named `100MB.bin` for testing purposes from [https://ash-speed.hetzner.com/](https://ash-speed.hetzner.com/). Since the file is too large, I won't put it in the compressed package. To conduct tests, please place these three test files in the same directory as the Makefile. You can then run my `.sh` script located in the src folder for testing. `client_log.txt` is used to get the graph mentioned in `writeup.pdf`. I use shell command to output the result from `stdout` to this file.
