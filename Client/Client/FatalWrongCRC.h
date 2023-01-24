#pragma once
#include "Request.h"

class FatalWrongCRC :
	public Request
{
private:
	char name[FILE_NAME_LEN];

public:
	FatalWrongCRC(request r, ClientSocket* socket, char* name);
	int sendRequest() override;
};

