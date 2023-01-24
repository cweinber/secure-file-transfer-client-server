#include "ReconnectFail.h"
ReconnectFail::ReconnectFail(ClientSocket* socket, response* res) : Response(socket,res) {
	memset(this->clientId, 0, CLIENT_ID);

}
int ReconnectFail::recResponse() {
	int err = Response::recResponse();
	/*user ID can be missing*/
	//if (err || res->payloadSize < CLIENT_ID) {
	//	return ERROR_BAD_COMPRESSION_BUFFER;
	//}
	//memcpy_s(this->clientId, CLIENT_ID, payload, res->payloadSize);
	return err;
}