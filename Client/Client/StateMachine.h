#pragma once
#include "Request.h"
#include "Response.h"
#include "Client.h"

enum class StateMachineClientCases
{
	CLIENT_STATE_REGISTERATION_REQUEST = 0,
	CLIENT_STATE_GENERATE_RSA_SEND_PUB_KEY,
	CLIENT_STATE_SEND_ENC_FILE_TO_SERVER,
	CLIENT_STATE_CHECK_SUM_CHECK,
	CLIENT_STATE_SEND_ACCEPT_MESSAGE,
	CLIENT_STATE_SEND_RETRY_MESSAGE,
	CLIENT_STATE_SEND_ABORT_MESSAGE,
	CLIENT_STATE_RECONNECT_REQUEST,
	CLIENT_STATE_DONE,
	CLIENT_STATE_UNKNOWN
};

enum class StateMachineServerCases
{
	SERVER_STATE_REGISTERATION_ACCEPTEED = 0,
	SERVER_STATE_SEND_PUB_KEY_TO_CLIENT,
	SERVER_STATE_DECRYPT_FILE_CALC_CHECKSUM,
	SERVER_STATE_DECRYPT_IS_VALID_CHECKSUM,
	SERVER_STATE_RECONNECT_REJECTED,
	SERVER_STATE_RECONNECT_ACCEPTED,
	SERVER_STATE_DONE,
	SERVER_STATE_UNKNOWN
};

class StateMachine
{
private:
	StateMachineClientCases curr_client_state;
	StateMachineServerCases last_server_event;
	int retyCalcChecksum;
	bool validChecksum;
	bool waitForResponse;
	Client* client;
public:
	StateMachine(bool userExists, Client* c);
	bool calcNextState();
	int doNextState();
	bool isWaitForResponse();
	int findNextEvent();
	StateMachineClientCases getNextState();
};

