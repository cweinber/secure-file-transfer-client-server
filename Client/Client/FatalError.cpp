#include "FatalError.h"

FatalError::FatalError(ClientSocket* socket, response* res) : Response( socket,res) {
	payload = NULL;
}
int FatalError::recResponse() {
	int err = Response::recResponse();
	return err;
}