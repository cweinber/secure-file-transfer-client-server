#include "ConfirmMessage.h"
ConfirmMessage::ConfirmMessage(ClientSocket* socket, response* res) : Response(socket,res) {
	memset(this->clientId, 0, CLIENT_ID);
}
int ConfirmMessage::recResponse() {

	int err = Response::recResponse();
	if (!err) {
		if (this->res->payloadSize < CLIENT_ID) {
			return ERROR_BAD_COMPRESSION_BUFFER;
		}
		memcpy_s(this->clientId, CLIENT_ID, this->payload, res->payloadSize);
	}
	return err;
}

uint8_t* ConfirmMessage::getClientId() {
	return clientId;
}

string ConfirmMessage::getHexClientId() {
	return convert_to_hex_string(this->clientId, CLIENT_ID);
}