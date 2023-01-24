#pragma once

#include <cstdint>
#include <atomic>
#include "ClientSocket.h"

typedef struct res {
	uint64_t version :16;
	uint64_t code:16;
	uint64_t payloadSize:32;
} response;
#define CLIENT_ID 16
#define FILE_NAME_LEN 255

#define RESPONSE_REG_SUCCEEDED		2100
#define RESPONSE_REG_FAILED			2101
#define RESPONSE_PUB_KEY_REC		2102
#define RESPONSE_FILE_REC			2103
#define RESPONSE_CONFIRM_MESSAGE	2104
#define RESPONSE_CONFIRM_RECONNECT	2105
#define RESPONSE_FAIL_RECONNECT		2106
#define RESPONSE_FATAL_ERROR		2107
class Response
{


protected:
	response* res;
	ClientSocket *socket;
	uint8_t* payload;

public:
	Response(ClientSocket* socket, response* res);
	virtual int recResponse();
	string convert_to_hex_string(uint8_t* array, uint32_t len);
};
