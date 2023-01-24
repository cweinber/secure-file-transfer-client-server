#include "ClientSocket.h"

#pragma warning(disable:4996)

void ClientSocket::SendData(string data) {
    // Send an initial buffer
    int iResult = send(sock, data.c_str(), (int)strlen(data.c_str()), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
    }
}
int ClientSocket::SendData(uint8_t* data, uint32_t dataLen) {
    // Send an initial buffer
    int iResult = send(sock, (const char *)data, dataLen, 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return iResult;
    }
    return 0;
}

int ClientSocket::RecData(uint32_t max_data_len, uint8_t* data) {
    int iResult = recv(sock, (char*)data, max_data_len, 0);
    if (iResult <= 0) {
        return ERROR_BAD_COMPRESSION_BUFFER;
    }
    return 0;
}

ClientSocket::ClientSocket(string ip, int port) {
    socket_ip = ip;
    socket_port = port;
}
int ClientSocket::CreateSession() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 0), &wsaData);
    this->sock = socket(AF_INET, SOCK_STREAM, 0);

    if (this->sock < 0) {
        cout << "[!] Unable to create socket" << endl;
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr((ClientSocket::socket_ip).c_str());
    serv_addr.sin_port = htons(ClientSocket::socket_port);

    if (connect(this->sock, (SOCKADDR*)&serv_addr, sizeof(sockaddr)) == SOCKET_ERROR) {
        return -1;
    }
    return 0;
}
void ClientSocket::CloseSession() {
    closesocket(this->sock);
}