#include "StateMachine.h"
//Requests
#include "CorrectCRC.h"
#include "FatalWrongCRC.h"
#include "Reconnect.h"
#include "Registeration.h"
#include "SendFile.h"
#include "SendPublicKey.h"
#include "WrongCRC.h"
//Responses
#include "ConfirmMessage.h"
#include "ConfirmReconnect.h"
#include "FatalError.h"
#include "FileRec.h"
#include "PubKeyRec.h"
#include "ReconnectFail.h"
#include "RegDone.h"
#include "RegFailed.h"
#include <vector>
StateMachine::StateMachine(bool userExists, Client* client) {
	if (!userExists) {
		curr_client_state = StateMachineClientCases::CLIENT_STATE_REGISTERATION_REQUEST;
	}
	else {
		curr_client_state = StateMachineClientCases::CLIENT_STATE_RECONNECT_REQUEST;
	}
	
	last_server_event = StateMachineServerCases::SERVER_STATE_UNKNOWN;
	retyCalcChecksum = 0;
	validChecksum = false;
	this->waitForResponse = false;
	this->client = client;
}
bool StateMachine::calcNextState() {
	bool validEv = false;
	switch (last_server_event)
	{
	case StateMachineServerCases::SERVER_STATE_REGISTERATION_ACCEPTEED:
		if (curr_client_state == StateMachineClientCases::CLIENT_STATE_REGISTERATION_REQUEST) {
			validEv = true;
			curr_client_state = StateMachineClientCases::CLIENT_STATE_GENERATE_RSA_SEND_PUB_KEY;
		}
		break;
	case StateMachineServerCases::SERVER_STATE_SEND_PUB_KEY_TO_CLIENT:
		if (curr_client_state == StateMachineClientCases::CLIENT_STATE_GENERATE_RSA_SEND_PUB_KEY ||
			curr_client_state == StateMachineClientCases::CLIENT_STATE_RECONNECT_REQUEST) {
			validEv = true;
			curr_client_state = StateMachineClientCases::CLIENT_STATE_SEND_ENC_FILE_TO_SERVER;
		}
		break;
	case StateMachineServerCases::SERVER_STATE_DECRYPT_FILE_CALC_CHECKSUM: 
		if (curr_client_state == StateMachineClientCases::CLIENT_STATE_SEND_ENC_FILE_TO_SERVER) {
			validEv = true;
			curr_client_state = StateMachineClientCases::CLIENT_STATE_CHECK_SUM_CHECK;
			retyCalcChecksum = 0;
		}
		break;
	case StateMachineServerCases::SERVER_STATE_DECRYPT_IS_VALID_CHECKSUM:
		if (curr_client_state == StateMachineClientCases::CLIENT_STATE_CHECK_SUM_CHECK) {
			validEv = true;
			if (validChecksum) {
				curr_client_state = StateMachineClientCases::CLIENT_STATE_SEND_ACCEPT_MESSAGE;
			}
			else if (retyCalcChecksum++ < 3) {
				curr_client_state = StateMachineClientCases::CLIENT_STATE_SEND_RETRY_MESSAGE;
			}
			else {
				curr_client_state = StateMachineClientCases::CLIENT_STATE_SEND_ABORT_MESSAGE;
			}
		}
		break;
	case StateMachineServerCases::SERVER_STATE_RECONNECT_REJECTED:
		if (curr_client_state == StateMachineClientCases::CLIENT_STATE_RECONNECT_REQUEST) {
			validEv = true;
			curr_client_state = StateMachineClientCases::CLIENT_STATE_REGISTERATION_REQUEST;
		}
		break;
	case StateMachineServerCases::SERVER_STATE_RECONNECT_ACCEPTED:
		if (curr_client_state == StateMachineClientCases::CLIENT_STATE_RECONNECT_REQUEST) {
			validEv = true;
			curr_client_state = StateMachineClientCases::CLIENT_STATE_SEND_ENC_FILE_TO_SERVER;
		}
		break;
	case StateMachineServerCases::SERVER_STATE_DONE:
		curr_client_state = StateMachineClientCases::CLIENT_STATE_DONE; 
		validEv = true;
		break;
	case StateMachineServerCases::SERVER_STATE_UNKNOWN:
		curr_client_state = StateMachineClientCases::CLIENT_STATE_UNKNOWN;
		break;
	default:
		break;
	}
	return validEv;
}
int StateMachine::doNextState () {
	Request* req = NULL;
	request r;
	memset(&r, 0, sizeof(r));
	int err = 0;
	std::vector<uint8_t> fileContent;
	if (client->getClientId() != nullptr) {
		memcpy_s(r.clientID, CLIENT_ID_LEN, client->getClientId(), CLIENT_ID_LEN);
	}
	r.version = client->getVersion();
	switch (curr_client_state) {
	case StateMachineClientCases::CLIENT_STATE_REGISTERATION_REQUEST:
		r.code = REQUEST_REGISTERATION;
		req = new Registeration(r, client->getSocket(), (uint8_t*)client->getClientName(), client->getClientNameLen());
		break;
	case StateMachineClientCases::CLIENT_STATE_GENERATE_RSA_SEND_PUB_KEY:
		r.code = REQUEST_SEND_PUB_KEY;
		err = client->create_rsa_key();
		if (!err) {
			req = new SendPublicKey(r, client->getSocket(), (uint8_t*)client->getClientName(), client->getPubKey(),client->getPubKeyLen(), client->getClientNameLen());
		}
		break;
	case StateMachineClientCases::CLIENT_STATE_SEND_ENC_FILE_TO_SERVER:
		r.code = REQUEST_SEND_FILE;
		uint32_t fileLen;
		fileContent = client->getFileContent(&fileLen);
		req = new SendFile(r, client->getSocket(), fileLen, (char*)(client->getFileName().c_str()), fileContent.data());
		break;
	case StateMachineClientCases::CLIENT_STATE_CHECK_SUM_CHECK:
		validChecksum = client->calcChecksum();
		last_server_event = StateMachineServerCases::SERVER_STATE_DECRYPT_IS_VALID_CHECKSUM;
		break;
	case StateMachineClientCases::CLIENT_STATE_SEND_ACCEPT_MESSAGE:
		r.code = REQUEST_CORRECT_CRC;
		req = new CorrectCRC(r, client->getSocket(), (char*)client->getFileName().c_str());
		break;
	case StateMachineClientCases::CLIENT_STATE_SEND_RETRY_MESSAGE:
		r.code = REQUEST_WRONG_CRC;
		req = new WrongCRC(r, client->getSocket(), (char*)client->getFileName().c_str());
		break;
	case StateMachineClientCases::CLIENT_STATE_SEND_ABORT_MESSAGE:
		r.code = REQUEST_FATAL_WRONG_CRC;
		req = new FatalWrongCRC(r, client->getSocket(), (char*)client->getFileName().c_str());
		last_server_event = StateMachineServerCases::SERVER_STATE_DONE;
		break;
	case StateMachineClientCases::CLIENT_STATE_RECONNECT_REQUEST:
		r.code = REQUEST_RECONNECT;
		req = new Reconnect(r, client->getSocket(), (uint8_t*)client->getClientName(), client->getClientNameLen());
		break;
	case StateMachineClientCases::CLIENT_STATE_DONE:
		client->closeSession();
		last_server_event = StateMachineServerCases::SERVER_STATE_UNKNOWN;
		break;
	case StateMachineClientCases::CLIENT_STATE_UNKNOWN:
		
		break;
	default:
		break;
	}
	if (req) {
		err = req->sendRequest();
	}
	if (req && curr_client_state != StateMachineClientCases::CLIENT_STATE_SEND_ABORT_MESSAGE) {
		waitForResponse = true;
	}
	else {
		waitForResponse = false;
	}
	return err;
}

bool StateMachine::isWaitForResponse() {
	return waitForResponse;
}
int StateMachine::findNextEvent() {
	response r;
	Response* res = NULL;
	int err = client->recResponseHeader(&r);
	if (!err) {
		switch (r.code)
		{
		case RESPONSE_REG_SUCCEEDED:
			res = new RegDone(client->getSocket(), &r);
			err = res->recResponse();
			if (!err) {
				client->setClientId(((RegDone*)res)->getClientId());
				last_server_event = StateMachineServerCases::SERVER_STATE_REGISTERATION_ACCEPTEED;
			}
			break;
		case RESPONSE_REG_FAILED:
			res = new RegFailed(client->getSocket(), &r);
			err = res->recResponse();
			if (!err) {
				last_server_event = StateMachineServerCases::SERVER_STATE_UNKNOWN;
			}
			break;
		case RESPONSE_PUB_KEY_REC:
			res = new PubKeyRec(client->getSocket(), &r);
			err = res->recResponse();
			if (!err) {
				client->setAesKeyLen(((PubKeyRec*)res)->getAESKeyLen());
				client->setAesKey(((PubKeyRec*)res)->getAESKey());
				
				if (strcmp(((PubKeyRec*)res)->getHexClientId().c_str(), client->getHexClientId().c_str()) == 0) {
					last_server_event = StateMachineServerCases::SERVER_STATE_SEND_PUB_KEY_TO_CLIENT;
				}
				else {
					last_server_event = StateMachineServerCases::SERVER_STATE_UNKNOWN;
				}
			}
			break;
		case RESPONSE_FILE_REC:
			res = new FileRec(client->getSocket(), &r);
			err = res->recResponse();
			if (!err) {
				client->setChecksum(((FileRec*)res)->getChecksum());
				client->setRecFileSize(((FileRec*)res)->getContentSize());
				client->setRecFileSize(((FileRec*)res)->getContentSize());
				if (strcmp(((FileRec*)res)->getHexClientId().c_str(), client->getHexClientId().c_str()) == 0 &&
					!memcmp(((FileRec*)res)->getFileName(), (uint8_t*)client->getFileName().c_str(), CLIENT_ID)) {
					last_server_event = StateMachineServerCases::SERVER_STATE_DECRYPT_FILE_CALC_CHECKSUM;
				}
				else {
					last_server_event = StateMachineServerCases::SERVER_STATE_UNKNOWN;
				}
				
			}
			break;
		case RESPONSE_CONFIRM_MESSAGE:
			res = new ConfirmMessage(client->getSocket(), &r);
			err = res->recResponse();
			if (!err) {
				if (strcmp(((ConfirmMessage*)res)->getHexClientId().c_str(), client->getHexClientId().c_str()) == 0) {
					last_server_event = StateMachineServerCases::SERVER_STATE_DONE;
				}
				else {
					last_server_event = StateMachineServerCases::SERVER_STATE_UNKNOWN;
				}
			}
			break;
		case RESPONSE_CONFIRM_RECONNECT:
			res = new ConfirmReconnect(client->getSocket(), &r);
			err = res->recResponse();
			if (!err) {
				client->setAesKeyLen(((ConfirmReconnect*)res)->getAESKeyLen());
				client->setAesKey(((ConfirmReconnect*)res)->getAESKey());

				if (strcmp(((ConfirmReconnect*)res)->getHexClientId().c_str(), client->getHexClientId().c_str()) == 0) {
					client->setClientId(((ConfirmReconnect*)res)->getClientId());
					last_server_event = StateMachineServerCases::SERVER_STATE_SEND_PUB_KEY_TO_CLIENT;
				}
				else {
					last_server_event = StateMachineServerCases::SERVER_STATE_UNKNOWN;
				}
			}
			break;
		case RESPONSE_FAIL_RECONNECT:
			res = new ReconnectFail(client->getSocket(), &r);
			last_server_event = StateMachineServerCases::SERVER_STATE_RECONNECT_REJECTED;
			break;
		case RESPONSE_FATAL_ERROR:
			res = new FatalError(client->getSocket(), &r);
			last_server_event = StateMachineServerCases::SERVER_STATE_DONE;
			break;
		default:
			return ERROR_ACCESS_DENIED;
			last_server_event = StateMachineServerCases::SERVER_STATE_UNKNOWN;
		}
		


	}
	return err;
}

StateMachineClientCases StateMachine::getNextState() {
	return curr_client_state;
}