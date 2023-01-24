#include "FatalWrongCRC.h"

FatalWrongCRC::FatalWrongCRC(request r, ClientSocket* socket, char* name) : Request(r, socket) {
	memcpy_s(this->name, FILE_NAME_LEN, name, FILE_NAME_LEN);
	this->payload = (uint8_t*)this->name;
	this->req.payloadSize = FILE_NAME_LEN;
}
int FatalWrongCRC::sendRequest() {
	int err = Request::sendRequest();
	return err;
}