#pragma once

#include <iostream>
#include <string>
#include <list>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <sstream>
#include <iomanip>
#include <bitset>
using namespace std;
class ClientSocket {
private:
    string socket_ip;
    int socket_port;
    SOCKET sock;
public:
    ClientSocket(string ip, int port);
    int CreateSession();
    void CloseSession();
    void SendData(string data);
    int SendData(uint8_t *data, uint32_t dataLen);
    int RecData(uint32_t max_data_len, uint8_t* data);
};