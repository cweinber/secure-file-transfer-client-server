#include "WrongCRC.h"

WrongCRC::WrongCRC(request r, ClientSocket* socket, char* name) : Request(r, socket) {
	memcpy_s(this->name, FILE_NAME_LEN, name, FILE_NAME_LEN);
	payload = (uint8_t*)this->name;
	req.payloadSize = FILE_NAME_LEN;
}
int WrongCRC::sendRequest() {
	int err = Request::sendRequest();
	return err;
}