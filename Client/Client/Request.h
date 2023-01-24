#pragma once
#include <cstdint>
#include <atomic>
#include "ClientSocket.h"

#define CLIENT_ID_LEN 16
typedef struct req {
	uint8_t clientID[CLIENT_ID_LEN];
	uint64_t version : 8;
	uint64_t code : 16;
	uint64_t payloadSize :32;
	uint64_t rsrvd : 8;
} request;
#define USER_NAME_LEN 255
#define FILE_NAME_LEN 255

#define REQUEST_REGISTERATION	1100
#define REQUEST_SEND_PUB_KEY	1101
#define REQUEST_RECONNECT		1102
#define REQUEST_SEND_FILE		1103
#define REQUEST_CORRECT_CRC		1104
#define REQUEST_WRONG_CRC		1105
#define REQUEST_FATAL_WRONG_CRC	1106
class Request
{
private:
	ClientSocket *socket;
protected:
	request req;
	uint8_t* payload;
public:
	Request(request req, ClientSocket* socket);
	virtual int sendRequest();
};

