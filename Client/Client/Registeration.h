#pragma once
#include "Request.h"

class Registeration :
    public Request
{
private:
    uint8_t name[USER_NAME_LEN];
    uint32_t nameLen;
public:
    Registeration(request r, ClientSocket* socket, uint8_t name[USER_NAME_LEN], uint32_t nameLen);
    int sendRequest() override;
};

