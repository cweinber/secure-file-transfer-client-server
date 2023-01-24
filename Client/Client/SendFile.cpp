#include "SendFile.h"

SendFile::SendFile(request r, ClientSocket* socket, uint32_t contentSize, char* name,
	uint8_t* messageContent) : Request(r, socket) {
	memset(&sendPayload, 0, sizeof(sendPayload));
	sendPayload.sendFilePayload.contentSize = contentSize;
	memcpy_s(sendPayload.sendFilePayload.fileName, strlen(name), name, strlen(name));
	this->messageContent = messageContent;
	uint8_t* ptr = (uint8_t*)malloc((FILE_PAYLOAD_DATA_LEN + contentSize) * sizeof(uint8_t));
	payload = ptr;
	req.payloadSize = FILE_PAYLOAD_DATA_LEN + contentSize;
	fullContent = ptr;
	if (ptr == NULL) {
		return;
	}
	memcpy_s(ptr, FILE_PAYLOAD_DATA_LEN, &sendPayload.data, FILE_PAYLOAD_DATA_LEN);
	memcpy_s(ptr + FILE_PAYLOAD_DATA_LEN, contentSize, messageContent, contentSize);
}
int SendFile::sendRequest() {
	int err = Request::sendRequest();
	return err;
}