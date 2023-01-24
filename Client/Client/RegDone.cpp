#include "RegDone.h"
RegDone::RegDone( ClientSocket* socket, response* res) : Response(socket,res) {
	memset(this->clientId, 0, CLIENT_ID);

}
int RegDone::recResponse() {
	int err = Response::recResponse();
	if (err || res->payloadSize < CLIENT_ID) {
		return ERROR_BAD_COMPRESSION_BUFFER;
	}
	memcpy_s(this->clientId, CLIENT_ID, payload, CLIENT_ID);
	return err;
}
uint8_t* RegDone::getClientId() {
	return clientId;
}

string RegDone::getHexClientId() {
	return convert_to_hex_string(this->clientId, CLIENT_ID);
}