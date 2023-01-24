#include "Reconnect.h"

Reconnect::Reconnect(request r, ClientSocket* socket, uint8_t name[USER_NAME_LEN], uint32_t nameLen) : Request(r, socket) {
	memset(this->name, '\0', USER_NAME_LEN);
	memcpy_s(this->name, nameLen, name, nameLen);
	this->nameLen = nameLen;
	this->payload = this->name;
	this->req.payloadSize = USER_NAME_LEN;
}
int Reconnect::sendRequest() {
	int err = Request::sendRequest();
	return err;
}