#pragma once
#include "Request.h"
class Reconnect :
	public Request
{
private:
	uint8_t name[USER_NAME_LEN];
	uint32_t nameLen;
public:
	Reconnect(request r, ClientSocket* socket, uint8_t name[USER_NAME_LEN], uint32_t nameLen);
	int sendRequest() override;
};

