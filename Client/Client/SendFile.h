#pragma once
#include "Request.h"
#define FILE_PAYLOAD_DATA_LEN FILE_NAME_LEN + 4 + 1

typedef struct payload
{
	uint32_t contentSize;
	uint8_t fileName[FILE_NAME_LEN];
	uint8_t reserved;
} payload;
typedef union sendFilePayload {
	payload sendFilePayload;
	uint8_t data[FILE_PAYLOAD_DATA_LEN];
} sendFilePayload;

class SendFile :
	public Request
{
private:
	sendFilePayload sendPayload;
	uint8_t* messageContent;
	uint8_t* fullContent;
public:
	SendFile(request r, ClientSocket* socket, uint32_t contentSize, char* name,
		uint8_t* messageContent);
	int sendRequest() override;
};

