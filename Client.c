#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")  // link with Winsock library


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
    FILE *fp; // file pointer
    char *buffer; // pointer to character array
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
    fread(buffer, fileLen, 1, fp);
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

    if (sendto(s, buffer, sizeof(buffer), 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR) {
        printf("sendto() failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    else printf("\nsent buffer");

}