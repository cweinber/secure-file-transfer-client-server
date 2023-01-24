#pragma once
#include "Response.h"

typedef struct fileData
{
	uint8_t clientId[CLIENT_ID];
	uint32_t contentSize;
	uint8_t fileName[FILE_NAME_LEN];
	uint8_t reserved;
	uint32_t checksum;
} fileData;
typedef union recFilePayload {
	fileData recFilePayload;
	uint8_t data[sizeof(fileData)];
} recFilePayload;

class FileRec :
    public Response
{
private:
	recFilePayload filePayload;
public:
	FileRec(ClientSocket* socket, response* res);
	int recResponse() override;
	uint32_t getChecksum();
	uint8_t* getFileName();
	uint32_t getContentSize();
	uint8_t* getClientId();
	string getHexClientId();
};

