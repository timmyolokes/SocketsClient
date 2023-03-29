# UDP Image Sender

This is a simple C program that sends an image file over a UDP connection using Winsock2. It demonstrates the basic usage of Winsock library to create a UDP client socket, read an image file from the local filesystem, and send the image data to a server through the socket.

## Dependencies

This program requires the following dependencies:

- [Winsock2](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2)
- Windows operating system (tested on Windows 10)

## Usage

1. Make sure you have a compatible C compiler installed on your system (e.g., [Visual Studio](https://visualstudio.microsoft.com/downloads/)).
2. Clone this repository to your local machine.
3. Update the file path in the source code to point to the image file you want to send:
4. Compile the program with your C compiler.
5. Run the compiled executable.

## Important Notes

- This program sends the image data to the server at IP address `127.0.0.1` (localhost) and port `80`. Make sure you have a UDP server running at the same IP address and port to receive the image data.
- This program does not handle any error correction or data integrity checks. It is intended as a simple demonstration of sending raw data over UDP using Winsock2.
- The program uses non-blocking I/O on the socket. This means that the `sendto` function may return immediately if the socket buffer is full, and the program does not wait for the data to be sent before exiting. This may result in partial or lost data transmission.
- This program assumes that the image file is small enough to fit into memory. For large files, it may be necessary to read and send the file data in smaller chunks to avoid memory issues.
