#include "pch.h"
#include "network.h"
#include "skoobhash.h"

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")

#define BROADCAST_PORT 8888
#define TCP_PORT 8889

#define RETRYCOUNTMAX 5
#define CHAOSENDATTEMPTSMAX 3
#define CHAORECEIVEATTEMPTSMAX 10

//std::thread connectionThread;

Network::Network(std::string gameName)
{
	broadcastSocket = INVALID_SOCKET;
	commsSocket = INVALID_SOCKET;

	socket_addrinfo_sender = new addrinfo;
	socket_addrinfo_recver = new addrinfo;

	this->gameName = gameName;

	chaobuffer = new CHAO_PARAM_GC;
	createNewChaoPointer = nullptr;
	chaobufferHash = 0;
}

void Network::setNonBlocking(SOCKET* socket, bool state)
{
	u_long ulstate = (u_long)state;
	if (ioctlsocket(*socket, FIONBIO, &ulstate) != 0)
	{
		printf("Couldn't set non-blocking state of socket\n");
	}
	else
	{
		printf("Set non-blocking state of socket to %s\n", (state ? "on" : "off"));
	}
}

void Network::runConnection(SOCKET connectionSocket, sockaddr* connectionAddr, int addrLen)
{
	const int bufferlen = 2048;
	char recvbuffer[bufferlen];
	char sendbuffer[bufferlen];

	int result = recv(connectionSocket, recvbuffer, bufferlen, 0);

	printf("Address to recvbuffer: %X\n", (int)&recvbuffer);

	switch (handleIncomingMessage(recvbuffer, result))
	{
	case ConnectRequest:
		sprintf_s(sendbuffer, bufferlen, "CHAOGARDEN ACCEPT");
		result = send(connectionSocket, sendbuffer, strlen(sendbuffer), 0);
		if (result == SOCKET_ERROR)
		{
			printf("Send failed: %d\n", WSAGetLastError());
			closesocket(connectionSocket);
			return;
		}
		else
		{
			printf("Sent back confirmation of connection\n");
			break;
		}
	default:
		//shutdown(connectionSocket, SD_SEND);
		//closesocket(connectionSocket);
		return;
	}

	int retries = 0;
	isConnected = true;

	while (true)
	{
		if (wantToClose)
		{
			shutdown(connectionSocket, SD_SEND);
			closesocket(connectionSocket);
			printf("Closing connection\n");
			return;
		}

		if (wantToSendChao && sendChaoAttempts < CHAOSENDATTEMPTSMAX)
		{
			sprintf_s(sendbuffer, bufferlen, "CHAOGARDEN SENDCHAO %X ", chaobufferHash);
			int sizeOfBuffer = strlen(sendbuffer);
			memcpy_s(sendbuffer + sizeOfBuffer, bufferlen - sizeOfBuffer, chaobuffer, sizeof(CHAO_PARAM_GC));
			sizeOfBuffer += sizeof(CHAO_PARAM_GC);
			send(connectionSocket, sendbuffer, sizeOfBuffer, 0);
			if (result == SOCKET_ERROR)
			{
				printf("Chao send failed: %d\n", WSAGetLastError());
				closesocket(connectionSocket);
				chaoSendFail = true;
				return;
			}
			printf("Sent chao: %s\n", sendbuffer);
			sendChaoAttempts++;
		}
		else if (sendChaoAttempts == CHAOSENDATTEMPTSMAX)
		{
			chaoSendFail = true;
			wantToSendChao = false;
			sendChaoAttempts = 0;
		}

		if (phoneSendRequesting)
		{
			if (acceptSendRequest)
			{
				sprintf_s(sendbuffer, bufferlen, "CHAOGARDEN REQUESTACCEPT");
				send(connectionSocket, sendbuffer, strlen(sendbuffer), 0);
				if (result == SOCKET_ERROR)
				{
					printf("Chao receive failed: %d\n", WSAGetLastError());
					closesocket(connectionSocket);
					return;
				}
				printf("Chao request accepted\n");
			}
			else if (sendChaoAttempts < CHAORECEIVEATTEMPTSMAX)
			{
				sendChaoAttempts++;
			}
			else
			{
				sendChaoAttempts = 0;
				phoneSendRequesting = false;
			}
		}

		result = recv(connectionSocket, recvbuffer, bufferlen, 0);
		switch (handleIncomingMessage(recvbuffer, result))
		{
		case TransferToPhoneSuccess:
			wantToSendChao = false;
			chaoSent = true;
			sendChaoAttempts = 0;
		case Keepalive:
			retries = 0;

			if (wantToSendChao || acceptSendRequest)
				break;

			sprintf_s(sendbuffer, bufferlen, "CHAOGARDEN KEEPALIVE");
			result = send(connectionSocket, sendbuffer, strlen(sendbuffer), 0);
			if (result == SOCKET_ERROR)
			{
				printf("Keepalive Send failed: %d\n", WSAGetLastError());
				closesocket(connectionSocket);
				return;
			}
			break;
		case TransferToPCRequest:
			phoneSendRequesting = true;
			break;
		case TransferToPCConfirmed:
			if (createNewChaoPointer)
			{
				loadReceivedChao(recvbuffer);
				acceptSendRequest = false;
				phoneSendRequesting = false;
			}
			break;
		case TransferToPhoneFailure:
			if (retries == RETRYCOUNTMAX / 2)
			{
				chaoSendFail = true;
				wantToSendChao = false;
				retries = 0;
			}
			else
			{
				retries++;
			}
			break;
		case Closed:
		case Failure:
			if (retries == RETRYCOUNTMAX)
			{
				shutdown(connectionSocket, SD_SEND);
				closesocket(connectionSocket);
				return;
			}
			else
			{
				retries++;
				printf("Failure, retrying %d times out of %d\n", retries, RETRYCOUNTMAX);
				break;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

// Handles all recv calls after they've been made
// The pointer to the recieved data buffer and the result of the recv are passed into this function
// The function returns an appropriate status enum to the caller in runConnection
Network::receivedMessageStatus Network::handleIncomingMessage(char* buffer, int recvResult)
{
	if (recvResult < 0)
	{
		printf("Error received!\n");
		return Failure;
	}
	else if (recvResult == 0)
	{
		printf("Accepted socket closed connection\n");
		return Closed;
	}

	std::string recvString = (std::string)buffer;

	printf("Data received: %s\n", buffer);
	if (recvString.substr(0, 7) != "CHAOADV" || recvString.length() < 9)
	{
		printf("Incorrect prefix signal\n");
		return NotChaoADV;
	}
	int stringOffset = 8;

	std::string msgRequest = recvString.substr(stringOffset, recvString.find(' ', stringOffset) - stringOffset);
	stringOffset += msgRequest.length();

	if (msgRequest == "CONNECT")
	{
		printf("Phone wants to connect\n");
		return ConnectRequest;
	}
	else if (msgRequest == "KEEPALIVE")
	{
		//printf("Keepalive message received\n");
		return Keepalive;
	}
	else if (msgRequest == "CLOSE")
	{
		printf("Phone would like to close connection\n");
		return Closed;
	}
	else if (msgRequest == "CHAOGOOD")
	{
		printf("Chao sent successfully\n");
		return TransferToPhoneSuccess;
	}
	else if (msgRequest == "CHAOBAD")
	{
		printf("Chao sent wrong\n");
		return TransferToPhoneFailure;
	}
	else if (msgRequest == "REQTOSEND")
	{
		printf("Request to send chao\n");
		return TransferToPCRequest;
	}
	else if (msgRequest == "SENDCHAO")
	{
		printf("Got chao from phone\n");
		return TransferToPCConfirmed;
	}
}

void Network::runBroadcaster()
{
	SOCKET AcceptSocket = INVALID_SOCKET;
	sockaddr* acceptedAddr = new sockaddr;
	int acceptedAddrLen = sizeof(sockaddr);

	int randNum = rand();

	char hostname[256];
	gethostname(hostname, 256);

	char sendBuffer[1024];
	sprintf_s<1024>(sendBuffer, "CHAOGARDEN SADX %s", hostname);

	setNonBlocking(&commsSocket, true);

	if (listen(commsSocket, 5) == SOCKET_ERROR)
	{
		printf("Comms socket listen failed with error: %d\n", WSAGetLastError());
		return;
	}
	printf("Listening for connections\n");

	while (true)
	{
		if (sendto(broadcastSocket, sendBuffer, strlen(sendBuffer), 0, static_cast<SOCKADDR*>(socket_addrinfo_sender->ai_addr), socklen_t(socket_addrinfo_sender->ai_addrlen)) == SOCKET_ERROR)
		{
			printf("Didnt send shit: %ld\n", WSAGetLastError());
		}
		else
		{
			printf("Attempted to broadcast %s\n", sendBuffer);
		}

		AcceptSocket = accept(commsSocket, acceptedAddr, &acceptedAddrLen);

		if (AcceptSocket != INVALID_SOCKET)
		{
			printf("Accepted socket\n");
			//setNonBlocking(&commsSocket, false);
			runConnection(AcceptSocket, acceptedAddr, acceptedAddrLen);
			isConnected = false;
		}

		if (wantToClose)
		{
			wantToClose = false;
			printf("Closing broadcaster\n");
			return;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void Network::setupServer()
{
	std::string localBroadcastIP;
	int iResult = findValidLocalBroadcastIP(localBroadcastIP);
	if (iResult != 0)
	{
		printf("Failed to create broadcast IP. IDK WHY HAHA, but I do here's the error code I arbitrarily made: %d\n", iResult);
		return;
	}
	else
	{
		printf("Broadcasting to: %s\n", localBroadcastIP.c_str());
	}

	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	printf("Used WSAStartup!\n");
	if (iResult != 0)
	{
		printf("WSAStartup Failed : %d\n", iResult);
		WSACleanup();
		return;
	}
	else
	{
		printf("Winsock DLL status is %s\n", wsaData.szSystemStatus);
	}

	addrinfo hints_broadcast;
	addrinfo hints_comms;
	memset(&hints_broadcast, 0, sizeof(hints_broadcast));
	memset(&hints_comms, 0, sizeof(hints_comms));

	hints_broadcast.ai_family = AF_UNSPEC;
	hints_broadcast.ai_socktype = SOCK_DGRAM;
	hints_broadcast.ai_protocol = IPPROTO_UDP;
	hints_broadcast.ai_flags = AI_ADDRCONFIG;

	hints_comms.ai_family = AF_INET;
	hints_comms.ai_socktype = SOCK_STREAM;
	hints_comms.ai_protocol = IPPROTO_TCP;
	hints_comms.ai_flags = AI_PASSIVE;

	addrinfo* getaddrinfo_results = nullptr;
	iResult = getaddrinfo(localBroadcastIP.c_str(), std::to_string(BROADCAST_PORT).c_str(), &hints_broadcast, &getaddrinfo_results);
	if (iResult != 0)
	{
		printf("getaddrinfo failed! %d\n", WSAGetLastError());
		freeaddrinfo(getaddrinfo_results);
		WSACleanup();
		return;
	}
	broadcastSocket = socket(getaddrinfo_results->ai_family, getaddrinfo_results->ai_socktype, getaddrinfo_results->ai_protocol);
	if (broadcastSocket != INVALID_SOCKET)
	{
		printf("Made broadcast socket\n");
		memcpy_s(socket_addrinfo_sender, sizeof(addrinfo), getaddrinfo_results, sizeof(addrinfo));
	}
	else
	{
		printf("Unable to make broadcast socket: %d\n", WSAGetLastError());
		freeaddrinfo(getaddrinfo_results);
		WSACleanup();
		return;
	}

	iResult = getaddrinfo(NULL, std::to_string(TCP_PORT).c_str(), &hints_comms, &getaddrinfo_results);
	if (iResult != 0)
	{
		printf("getaddrinfo failed! %d\n", WSAGetLastError());
		freeaddrinfo(getaddrinfo_results);
		closesocket(broadcastSocket);
		WSACleanup();
		return;
	}
	commsSocket = socket(getaddrinfo_results->ai_family, getaddrinfo_results->ai_socktype, getaddrinfo_results->ai_protocol);
	if (commsSocket != INVALID_SOCKET)
	{
		printf("Made comms socket\n");
		memcpy_s(socket_addrinfo_recver, sizeof(addrinfo), getaddrinfo_results, sizeof(addrinfo));
	}
	else
	{
		printf("Unable to make comms socket: %d\n", WSAGetLastError());
		closesocket(broadcastSocket);
		WSACleanup();
		return;
	}

	iResult = bind(commsSocket, static_cast<SOCKADDR*>(socket_addrinfo_recver->ai_addr), socklen_t(socket_addrinfo_recver->ai_addrlen));
	if (iResult == SOCKET_ERROR)
	{
		printf("Couldn't bind comms socket with error: %d\n", WSAGetLastError());
		closesocket(broadcastSocket);
		closesocket(commsSocket);
		WSACleanup();
		return;
	}
	else
	{
		printf("Binded comms socket!\n");
	}

	freeaddrinfo(getaddrinfo_results);
	isSetup = true;
}

void Network::sendChao(CHAO_PARAM_GC* chaoToSend)
{
	memcpy_s(this->chaobuffer, sizeof(CHAO_PARAM_GC), chaoToSend, sizeof(CHAO_PARAM_GC));
	chaobufferHash = SkoobHashOnMem(this->chaobuffer, sizeof(CHAO_PARAM_GC), INITSEED);
	wantToSendChao = true;
}

void Network::receiveChao(CHAO_PARAM_GC* newChaoSlot)
{
	createNewChaoPointer = newChaoSlot;
	acceptSendRequest = true;
	//memcpy_s(newChaoSlot, sizeof(CHAO_PARAM_GC), this->chaobuffer, sizeof(CHAO_PARAM_GC));
}

void Network::loadReceivedChao(char* buffer)
{
	int offset = 17;
	std::string data = std::string(buffer);
	std::string correctHash = data.substr(offset, data.find(offset, ' ') - offset);
	offset += correctHash.length();

	memcpy_s(createNewChaoPointer, sizeof(CHAO_PARAM_GC), buffer + offset, sizeof(CHAO_PARAM_GC));

	uint32_t incomingHash = SkoobHashOnMem(createNewChaoPointer, sizeof(CHAO_PARAM_GC), INITSEED);
	printf("Hash of incoming chao %X\n", incomingHash);
	if (incomingHash == std::strtoul(correctHash.c_str(), NULL, 16))
	{
		printf("Correct chao data tranferred to PC\n");
		receivedChaoSuccessfully = true;
	}
	createNewChaoPointer = nullptr;
}

void Network::closeBroadcaster()
{
	wantToClose = true;
}

void Network::cleanupNetwork()
{
	isSetup = false;
	wantToClose = true;

	closesocket(broadcastSocket);
	closesocket(commsSocket);
	WSACleanup();

	free(this->chaobuffer);
}

int Network::findValidLocalBroadcastIP(std::string& broadcastIP)
{
	PMIB_IPADDRTABLE IPTable = (MIB_IPADDRTABLE*)malloc(sizeof(MIB_IPADDRTABLE));
	if (IPTable == NULL)
	{
		return 1;
	}

	ULONG dwSize = 0;
	if (GetIpAddrTable(IPTable, &dwSize, false) == ERROR_INSUFFICIENT_BUFFER)
	{
		free(IPTable);
		IPTable = (MIB_IPADDRTABLE*)malloc(sizeof(dwSize));
		if (IPTable == NULL)
		{
			return 1;
		}
	}
	if (GetIpAddrTable(IPTable, &dwSize, false) != NO_ERROR)
	{
		free(IPTable);
		return 2;
	}

	for (int i = 0; i < IPTable->dwNumEntries; i++)
	{
		IN_ADDR PrimaryIPAddr;
		ULONG primaryIP = (u_long)IPTable->table[i].dwAddr;
		IN_ADDR SubnetIPMask;
		ULONG subnetIP = (u_long)IPTable->table[i].dwMask;

		if (((primaryIP & 0xFFFF) ^ 0xA8C0) == 0 ||
			((primaryIP & 0xFF) ^ 0xAC) == 0 && ((primaryIP & 0xF000) >> 12) == 1 ||
			((primaryIP & 0xFF) ^ 0x0A) == 0)
		{
			PrimaryIPAddr.S_un.S_addr = (u_long)IPTable->table[i].dwAddr | (~subnetIP);
			char ipBuffer[64];
			inet_ntop(AF_INET, &PrimaryIPAddr, ipBuffer, 64);
			broadcastIP = std::string(ipBuffer);
			return 0;
		}
	}
	return 3;
}

bool Network::getIsConnected()
{
	return this->isConnected;
}

int Network::getIsSentChao()
{
	if (chaoSendFail || !isConnected)
		return 2; // Fail enum
	
	return chaoSent; // 0 for not sent yet, 1 for successfully sent
}

bool Network::getIsPhoneRequestingSend() 
{
	return phoneSendRequesting;
}

void Network::confirmChaoMessage()
{
	wantToSendChao = false;
	chaoSendFail = false;
	chaoSent = false;
}
