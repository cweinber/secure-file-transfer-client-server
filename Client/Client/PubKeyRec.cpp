#include "PubKeyRec.h"

PubKeyRec::PubKeyRec(ClientSocket* socket, response* res) : Response(socket,res) {
	memset(clientId, 0, CLIENT_ID);
}
int PubKeyRec::recResponse() {
	int err = Response::recResponse();
	if (err || res->payloadSize < CLIENT_ID) {
		return ERROR_BAD_COMPRESSION_BUFFER;
	}
	memcpy_s(clientId, CLIENT_ID, payload, CLIENT_ID);
	this->aesKey = (uint8_t*)malloc((res->payloadSize - CLIENT_ID) * sizeof(uint8_t));

	if (this->aesKey == NULL) {
		return ERROR_BAD_COMPRESSION_BUFFER;
	}
	memcpy_s(this->aesKey, res->payloadSize - CLIENT_ID, payload + CLIENT_ID, (res->payloadSize - CLIENT_ID));
	aesKeyLen = res->payloadSize - CLIENT_ID;
	return err;
}

uint8_t* PubKeyRec::getAESKey() {
	return this->aesKey;
}

uint32_t PubKeyRec::getAESKeyLen() {
	return this->aesKeyLen;
}

uint8_t* PubKeyRec::getClientId() {
	return clientId;
}

string PubKeyRec::getHexClientId() {
	return convert_to_hex_string(this->clientId, CLIENT_ID);
}