#pragma once
#include "Response.h"
class RegDone :
    public Response
{
private:
	uint8_t clientId[CLIENT_ID];

public:
	RegDone(ClientSocket* socket, response* res);
	int recResponse() override;
	uint8_t* getClientId();
	string getHexClientId();
};



