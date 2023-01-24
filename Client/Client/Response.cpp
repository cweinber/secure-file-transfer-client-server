#include "Response.h"

Response::Response(ClientSocket* socket, response *res) {
	memset(&this->res, 0, sizeof(this->res));
	this->socket = socket;
	this->res = res;
}

int Response::recResponse() {
	int err = 0;
	if (this->res->payloadSize) {
		this->payload = (uint8_t*)malloc(this->res->payloadSize);
		err = this->socket->RecData(this->res->payloadSize, this->payload);
	}
	return err;
}

//common function
string Response::convert_to_hex_string(uint8_t* array, uint32_t len) {
	std::stringstream ss;
	ss << std::setfill('0') << std::hex << std::setw(2);
	for (int i = 0; i < len; i++) {
		ss << (int)array[i];
	}
	return ss.str();
}