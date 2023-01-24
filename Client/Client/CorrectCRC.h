#pragma once
#include "Request.h"

class CorrectCRC :
	public Request
{
private:
	char name[FILE_NAME_LEN];

public:
	CorrectCRC(request r, ClientSocket* socket, char* name);
	int sendRequest() override;
};

