#pragma once
#include "Response.h"

class RegFailed :
	public Response
{
private:

public:
	RegFailed(ClientSocket* socket, response* res);
	int recResponse() override;
};
