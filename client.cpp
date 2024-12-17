#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int InitializeWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
    return 0;
}

SOCKET CreateSocket() {
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        perror("socket");
        WSACleanup();
        return INVALID_SOCKET;
    }
    return sockfd;
}

int ConnectToServer(SOCKET sockfd, struct sockaddr_in* server_addr) {
    if (connect(sockfd, (struct sockaddr*)server_addr, sizeof(*server_addr)) == SOCKET_ERROR) {
        perror("connect");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    return 0;
}

void CommunicateWithServer(SOCKET sockfd) {
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        send(sockfd, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE);

        int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        printf("Server response: %s\n", buffer);
    }
}

int main() {
    if (InitializeWinsock() != 0) {
        return 1;
    }

    SOCKET sockfd = CreateSocket();
    if (sockfd == INVALID_SOCKET) {
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (ConnectToServer(sockfd, &server_addr) != 0) {
        return 1;
    }

    printf("Connected to server.\n");

    CommunicateWithServer(sockfd);

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
