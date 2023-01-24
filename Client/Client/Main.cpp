#include "ClientSocket.h"
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
#include "StateMachine.h"
#include "Client.h"
#include <fstream>
#include <string>
using namespace std;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")

int main() {
	string userName = "";
	string fileName = "";
	string serverIp = "";
	string userId = "";
	int port = 1234;
	bool userExists = false;
	try
	{
		string line;
		ifstream myfile;
		myfile.open("transfer.info");

		if (!myfile.is_open()) {
			perror("Error open");
			exit(EXIT_FAILURE);
		}
		getline(myfile, line);
		string subStr = ":";
		serverIp = line.substr(0, line.find(subStr));
		port = stoi(line.substr(line.find(subStr) + 1, line.length()));
		getline(myfile, userName);
		getline(myfile, fileName);

		ifstream meFile("me.info");
		if (!meFile.fail()) {
			getline(meFile, userName);
			getline(meFile, userId);
			userExists = true;
		}

	}
	catch (const std::exception&)
	{
		perror("Unexpected File Error");
		exit(EXIT_FAILURE);
	}

	try
	{
		ClientSocket clientsocket(serverIp, port);
		int err = clientsocket.CreateSession();
		if (err) {
			perror("Unexpected Connection Error");
			exit(EXIT_FAILURE);
		}
		const uint8_t* un = reinterpret_cast<const uint8_t*>(userName.c_str());
		Client* c = new Client((uint8_t*)un,strlen(userName.c_str()), &clientsocket);
		if (userExists) {
			c->setHexClientId(userId);
		}
		const uint8_t* fn = reinterpret_cast<const uint8_t*>(fileName.c_str());
		c->setFileName((char*)fn);
		c->setFilePath((uint8_t*)"");
		StateMachine* sm = new StateMachine(userExists, c);
		while (sm->getNextState() != StateMachineClientCases::CLIENT_STATE_UNKNOWN) {
			sm->doNextState();
			if (sm->isWaitForResponse()) {
				sm->findNextEvent();
			}
			sm->calcNextState();
		}
	}
	catch (const std::exception&)
	{
		perror("Unexpected Socket Error");
		exit(EXIT_FAILURE);
	}


	return 0;
}