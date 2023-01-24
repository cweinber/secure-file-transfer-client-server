#pragma once
#include "Response.h"

class ReconnectFail :
	public Response
{
private:
	uint8_t clientId[CLIENT_ID];

public:
	ReconnectFail(ClientSocket* socket, response* res);
	int recResponse() override;
};



