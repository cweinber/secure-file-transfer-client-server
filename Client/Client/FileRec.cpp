#include "FileRec.h"
FileRec::FileRec(ClientSocket* socket, response* res) : Response( socket,res) {
	memset(filePayload.data, 0, sizeof(recFilePayload));
}
int FileRec::recResponse() {
	int err = Response::recResponse();
	if (err || res->payloadSize < sizeof(recFilePayload)) {
		return  ERROR_BAD_COMPRESSION_BUFFER;
	}
	memcpy_s(filePayload.data, sizeof(recFilePayload), payload, res->payloadSize);
	return err;
}
uint32_t FileRec::getChecksum() {
	return this->filePayload.recFilePayload.checksum;
}
uint8_t* FileRec::getFileName() {
	return this->filePayload.recFilePayload.fileName;
}
uint32_t FileRec::getContentSize() {
	return this->filePayload.recFilePayload.contentSize;
}
uint8_t* FileRec::getClientId() {
	return this->filePayload.recFilePayload.clientId;
}

string FileRec::getHexClientId() {
	return convert_to_hex_string(this->filePayload.recFilePayload.clientId, CLIENT_ID);
}