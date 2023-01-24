#pragma once
#include "Request.h"

class WrongCRC :
	public Request
{
private:
	char name[FILE_NAME_LEN];

public:
	WrongCRC(request r, ClientSocket* socket, char* name);
	int sendRequest() override;
};

