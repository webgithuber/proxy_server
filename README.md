# proxy_server

This project demonstrates how to compile and run a program consisting of `request.cpp`, `proxy_parse.c`, and `LRU.cpp`.

## Requirements

- **g++**: Ensure that you have `g++` installed on your system.
- **Windows**: The `-lwsock32` flag is used to link the WinSock library, which is specific to Windows networking.

## How to Compile and Run

1. Open your terminal and navigate to the directory where your project files are located.
   
2. Run the following command to compile the code:
   ```bash
   g++ request.cpp proxy_parse.c LRU.cpp -o request.exe -lwsock32
3. Run the following command to run the code:
   ```bash
   ./request.exe
