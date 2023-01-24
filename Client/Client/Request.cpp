#include "Request.h"

Request::Request(request req, ClientSocket* socket) {
	this->req = req;
	this->socket = socket;
	this->payload = NULL;
}

int Request::sendRequest() {
	uint32_t dataLen = (req.payloadSize * sizeof(uint8_t)) + sizeof(request);
	uint8_t* data = (uint8_t*)malloc(dataLen);
	if (!data) {
		return ERROR_BAD_LENGTH;
	}
	memcpy_s(data, sizeof(request), &req, sizeof(request));
	memcpy_s(data + sizeof(request), req.payloadSize, payload, req.payloadSize);
	return socket->SendData(data, dataLen);
}