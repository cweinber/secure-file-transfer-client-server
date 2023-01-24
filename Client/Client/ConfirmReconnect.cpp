#include "ConfirmReconnect.h"
ConfirmReconnect::ConfirmReconnect(ClientSocket* socket, response* res) : Response( socket,res) {
	memset(clientId, 0, CLIENT_ID);
}
int ConfirmReconnect::recResponse() {
	int err = Response::recResponse();
	if (err) {
		return ERROR_BAD_COMPRESSION_BUFFER;
	}
	if (res->payloadSize < CLIENT_ID) {
		return ERROR_BAD_COMPRESSION_BUFFER;
	}
	memcpy_s(clientId, CLIENT_ID, payload, CLIENT_ID);
	this->key = (uint8_t*)malloc((res->payloadSize - CLIENT_ID) * sizeof(uint8_t));

	if (this->key == NULL) {
		return ERROR_BAD_COMPRESSION_BUFFER;
	}
	memcpy_s(this->key, res->payloadSize - CLIENT_ID, payload + CLIENT_ID, (res->payloadSize - CLIENT_ID));
	aesKeyLen = res->payloadSize - CLIENT_ID;
	return err;
}

uint8_t* ConfirmReconnect::getAESKey() {
	return this->key;
}

uint32_t ConfirmReconnect::getAESKeyLen() {
	return this->aesKeyLen;
}

uint8_t* ConfirmReconnect::getClientId() {
	return clientId;
}

string ConfirmReconnect::getHexClientId() {
	return convert_to_hex_string(this->clientId, CLIENT_ID);
}