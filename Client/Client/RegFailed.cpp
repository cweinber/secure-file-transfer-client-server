#include "RegFailed.h"

RegFailed::RegFailed( ClientSocket* socket, response* res) : Response(socket,res) {
	payload = NULL;
}
int RegFailed::recResponse() {
	int err = Response::recResponse();
	return err;
}