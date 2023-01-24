#include "SendPublicKey.h"

SendPublicKey::SendPublicKey(request r, ClientSocket* socket, uint8_t name[USER_NAME_LEN], string value,  uint32_t pubKeyLen, uint32_t nameLen) : Request(r, socket) {
	memset(pubKey.name, '\0', USER_NAME_LEN);
	memcpy_s(pubKey.name, nameLen, name, nameLen);
	//pubKey.value= value;
	uint8_t* ptr = (uint8_t*)malloc((USER_NAME_LEN + pubKeyLen) * sizeof(uint8_t));
	memcpy_s(ptr, USER_NAME_LEN, pubKey.name, USER_NAME_LEN);
	memcpy_s(ptr + USER_NAME_LEN, pubKeyLen, value.c_str(), pubKeyLen);
	payload = (uint8_t*)ptr;
	req.payloadSize = USER_NAME_LEN + pubKeyLen;
	this->nameLen = nameLen;
}
int SendPublicKey::sendRequest() {
	int err = Request::sendRequest();
	return err;
}