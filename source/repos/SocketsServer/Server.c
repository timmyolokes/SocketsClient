#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#pragma comment(lib, "ws2_32.lib")

#define PACKET_SIZE 1024

typedef struct {
    int seqNum;
    char data[PACKET_SIZE];
} Packet;

int main() {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server, si_other;
    int slen = sizeof(si_other);

    printf("\n****** INITIALIZING WINSOCK ***********");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
    else printf("\nWINSOCK INITIALIZED");

    /***** CREATE SERVER SOCKET ****/
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
    }
    else printf("\nUDP SERVER SOCKET CREATED.");

    /***** INITIALIZE SOCKET STRUCT - Server ****/
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(80);

    /***** BIND SOCKET ****/
    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("\nSERVER SOCKET BIND SUCCESS");


    // Open file to write the received image
    FILE* writefp = fopen("test2.jpg", "wb");
    if (writefp == NULL) {
        printf("Error Opening Image-write");
        exit(EXIT_FAILURE);
    }

    int expected_sequence_number = 0;
    int transmission_ended = 0;

    // Receive data packets
    printf("\nWaiting for data...");

    while (!transmission_ended) {
        Packet recvPacket;
        int recv_len;

        // Receive packet
        if ((recv_len = recvfrom(s, (char*)&recvPacket, sizeof(recvPacket), 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR) {
            printf("recvfrom() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        else {
            // Check for the end of transmission signal
            if (recvPacket.seqNum < 0) {
                transmission_ended = 1;
                continue;
            }

            printf("\nReceived packet from %s, Packet sequence number: %d", inet_ntoa(si_other.sin_addr), recvPacket.seqNum);

            // Check sequence number and copy to data structure if it's OK
            if (recvPacket.seqNum == expected_sequence_number) {
                fwrite(recvPacket.data, 1, recv_len - sizeof(int), writefp);
                expected_sequence_number++;
            }
            else {
                printf("Unexpected packet sequence number, expecting %d\n", expected_sequence_number);
            }

            // ACK sequence number
            Packet ackPacket;
            ackPacket.seqNum = recvPacket.seqNum;
            if (sendto(s, (char*)&ackPacket, sizeof(int), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR) {
                printf("sendto() failed with error code : %d", WSAGetLastError());
                exit(EXIT_FAILURE);
            }
            printf("\nACK %d sent", recvPacket.seqNum);

        }
    }

    // Cleanup
    fclose(writefp);
    closesocket(s);
    WSACleanup();

    return 0;
}