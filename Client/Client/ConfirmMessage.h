#pragma once
#include "Response.h"
class ConfirmMessage :
	public Response
{
private:
	uint8_t clientId[CLIENT_ID];
	uint8_t* pubKey;
public:
	ConfirmMessage(ClientSocket* socket, response* res);
	int recResponse() override;
	uint8_t* getClientId();
	string getHexClientId();
};

