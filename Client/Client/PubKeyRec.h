#pragma once
#include "Response.h"

class PubKeyRec :
	public Response
{
private:
	uint8_t clientId[CLIENT_ID];
	uint8_t* aesKey;
	uint32_t aesKeyLen;
public:
	PubKeyRec(ClientSocket* socket, response* res);
	int recResponse() override;
	uint8_t* getAESKey();
	uint32_t getAESKeyLen();
	uint8_t* getClientId();
	string getHexClientId();
};
