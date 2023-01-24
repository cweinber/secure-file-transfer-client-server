#pragma once
#include "Request.h"
#define PUB_KEY_VAL_LEN 160

typedef struct pubKeyPayload {
    uint8_t name[USER_NAME_LEN];
    uint8_t *value;
} pubKeyPayload;

class SendPublicKey :
    public Request
{
private:
    pubKeyPayload pubKey;
    uint32_t nameLen;
public:
    SendPublicKey(request r, ClientSocket* socket, uint8_t name[USER_NAME_LEN], string value, uint32_t pubKeyLen, uint32_t nameLen);
    int sendRequest() override;
};


