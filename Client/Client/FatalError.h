#pragma once
#include "Response.h"
class FatalError :
	public Response
{
private:

public:
	FatalError(ClientSocket* socket, response* res);
	int recResponse() override;
};
