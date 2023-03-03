#include "pch.h"
#include "network.h"

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")

#define BROADCAST_PORT 8888
#define TCP_PORT 8889

//std::thread connectionThread;

Network::Network()
{
	broadcastSocket = INVALID_SOCKET;
	commsSocket = INVALID_SOCKET;

	socket_addrinfo_sender = new addrinfo;
	socket_addrinfo_recver = new addrinfo;
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

void Network::runConnection(SOCKET connectionSocket, int)
{
	const int bufferlen = 1024;
	char recvbuffer[bufferlen];
	char sendbuffer[bufferlen];

	int result = recv(connectionSocket, recvbuffer, bufferlen, 0);

	if (result > 0)
	{
		std::string recvString = (std::string)recvbuffer;

		printf("Data received: %s\n", recvbuffer);
		if (recvString.substr(0, 15) != "CHAOADV CONNECT")
		{
			printf("Incorrect signal to connect\n");
			shutdown(connectionSocket, SD_SEND);
			closesocket(connectionSocket);
			return;
		}
		printf("Connection Established\n");
		sprintf_s(sendbuffer, bufferlen, "CHAOGARDEN ACCEPT");
		result = send(connectionSocket, sendbuffer, strlen(sendbuffer), 0);
		if (result == SOCKET_ERROR)
		{
			printf("Send failed: %d\n", WSAGetLastError());
			closesocket(connectionSocket);
			return;
		}
	}
	else if (result < 1)
	{
		shutdown(connectionSocket, SD_SEND);
		closesocket(connectionSocket);
		return;
	}

	printf("Sent back confirmation of connection\n");
	while (true)
	{
		result = recv(connectionSocket, recvbuffer, bufferlen, 0);
		if (result < 1)
		{
			printf("Keepalive Receive failed: %d\n", WSAGetLastError());
			closesocket(connectionSocket);
			return;
		}

		std::string recvString = (std::string)recvbuffer;

		if (recvString.substr(0, 17) != "CHAOADV KEEPALIVE")
		{
			printf("Didn't receive keepalive message\n");
			closesocket(connectionSocket);
			return;
		}

		printf("Received keepalive message: %s\n", recvbuffer);

		sprintf_s(sendbuffer, bufferlen, "CHAOGARDEN KEEPALIVE");
		result = send(connectionSocket, sendbuffer, strlen(sendbuffer), 0);
		if (result == SOCKET_ERROR)
		{
			printf("Keepalive Send failed: %d\n", WSAGetLastError());
			closesocket(connectionSocket);
			return;
		}
	}
}

void Network::runBroadcaster()
{
	SOCKET AcceptSocket = INVALID_SOCKET;

	int randNum = rand();

	char sendBuffer[1024];
	sprintf_s<1024>(sendBuffer, "CHAOGARDEN %x", randNum);

	setNonBlocking(&commsSocket, true);

	if (listen(commsSocket, 5) == SOCKET_ERROR)
	{
		printf("Receiver socket listen failed with error: %d\n", WSAGetLastError());
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
			printf("Attempted to broadcast the number %x\n", randNum);
		}

		AcceptSocket = accept(commsSocket, NULL, NULL);

		if (AcceptSocket != INVALID_SOCKET)
		{
			printf("Accepted socket\n");
			setNonBlocking(&commsSocket, false);
			runConnection(AcceptSocket, randNum);
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

void sendData()
{
	/*server.sin_addr.S_un.S_addr = inet_addr("192.168.31.255");
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);

	int randNum = rand();
	if (sendto(senderSocket, std::to_string(randNum).c_str(), std::to_string(randNum).size(), NULL, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		PrintDebug("sendto fucking failed: %d\n", WSAGetLastError());
	}
	else
	{
		PrintDebug("sent this shit: %d\n", randNum);
	}*/

	
}

void Network::cleanupNetwork()
{
	closesocket(broadcastSocket);
	closesocket(commsSocket);
	WSACleanup();

	isSetup = false;
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
