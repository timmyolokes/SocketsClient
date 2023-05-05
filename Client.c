#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

#define PACKET_SIZE 1024

typedef struct {
    int seqNum;
    char data[PACKET_SIZE];
} Packet;

int main() {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in si_other;
    int slen = sizeof(si_other);
    unsigned long noBlock;

    printf("\n****** INITIALIZING WINSOCK ***********");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
    else printf("\nWINSOCK INITIALIZED");

    /**file variable **/
    unsigned long fileLen; // length of image file
    FILE* fp; // file pointer
    char* buffer; // pointer to character array
    /*** OPEN IMAGE FILE AND COPY TO DATA STRUCTURE ***/
    fp = fopen("C:\\Users\\timil\\Downloads\\test.jpg", "rb");
    if (fp == NULL) {
        printf("\n Error Opening Image - read");
        fclose(fp);
        exit(0);
    }

    /*** ALLOCATE MEMORY (BUFFER) TO HOLD IMAGE *****/
    fseek(fp, 0, SEEK_END); //go to EOF
    fileLen = ftell(fp); // determine length
    fseek(fp, 0, SEEK_SET); //reset fp
    buffer = (char*)malloc(fileLen + 1); //allocated memory

    if (!buffer) {
        printf("\n memory error allocating buffer");
        fclose(fp);
        return 1;
    }

    /********* READ FILE DATA INTO BUFFER AND CLOSE FILE *************/
    fread(buffer, 1, fileLen, fp);
    fclose(fp);

    /***** CREATE CLIENT SOCKET ****/
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
    }
    printf("\nUDP CLIENT SOCKET CREATED.");

    /***** INITIALIZE SOCKET STRUCT - Non Blocking Client ****/
    noBlock = 1;
    ioctlsocket(s, FIONBIO, &noBlock);
    si_other.sin_addr.s_addr = inet_addr("127.0.0.1");
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(80);

    /***** SEND PACKETS ****/
    int NumPackets = (fileLen + PACKET_SIZE - 1) / PACKET_SIZE;
    int seqNum = 0;
    for (int i = 0; i < NumPackets; i++) {
        Packet packet;
        packet.seqNum = seqNum;
        int packetSize = PACKET_SIZE;
        if (i == NumPackets - 1) { //last packet may not be full packet size
            packetSize = fileLen % PACKET_SIZE;
        }
        memcpy(packet.data, &buffer[i * PACKET_SIZE], packetSize); //copy packet data from buffer
        if (sendto(s, (char*)&packet, packetSize + sizeof(int), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR) {
            printf("sendto() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        printf("Packet %d of %d sent\n", seqNum, NumPackets);

        int ACK = 0;
        while (ACK == 0) {
            Packet ackPacket;
            int ackSize = recvfrom(s, (char*)&ackPacket, sizeof(ackPacket), 0, (struct sockaddr*)&si_other, &slen);
            if (ackSize == SOCKET_ERROR) {
                int errorCode = WSAGetLastError();
                if (errorCode == WSAEWOULDBLOCK || errorCode == WSAECONNRESET) {
                    // No data available to be received yet, continue loop
                    continue;
                }
                else {
                    printf("recvfrom() failed with error code : %d", errorCode);
                    exit(EXIT_FAILURE);
                }
            }
            else if (ackSize == sizeof(int) && ackPacket.seqNum == seqNum) {
                printf("ACK %d received\n", seqNum);
                seqNum++;
                ACK = 1;
            }
        }
        ACK = 0; // reset ACK back to false

    }
    printf("\nAll packets sent.");

    // Cleanup
    closesocket(s);
    WSACleanup();

    return 0;
}
