#include "pch.h"
#include "network.h"

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")

#define BROADCAST_PORT 8888
#define TCP_PORT 8889

//struct sockaddr_in si_other;
SOCKET senderSocket, recverSocket;
addrinfo* socket_addrinfo_sender;
addrinfo* socket_addrinfo_recver;

//std::thread connectionThread;

void runConnection(SOCKET connectionSocket, int)
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

void runServer()
{
	/*udpReceiver = kn::udp_socket(kn::endpoint("0.0.0.0", 8889));
	PrintDebug("Made that other socket");
	udpReceiver.bind();
	PrintDebug("Binded");

	udpReceiver.set_non_blocking(true);

	kn::buffer<128> recv_buff;
	kn::addr_collection addr;
	while (true)
	{
		auto [received_bytes, status] = udpReceiver.recv(recv_buff);
		if (received_bytes > 0)
		{
			PrintDebug("Bytes available: %d\n", received_bytes);
		}
		//PrintDebug("Funny received");
		//if (!received_bytes || status != kissnet::socket_status::valid) {
			//break;
		//}
		//PrintDebug("Got %d\n", std::to_integer<int>(recv_buff[1]));
	}
	PrintDebug("Server fucked off\n");*/
	//kn::socket<kn::protocol::udp>

	SOCKET AcceptSocket;

	int randNum = rand();

	char sendBuffer[1024];
	sprintf_s<1024>(sendBuffer, "CHAOGARDEN %x", randNum);

	if (listen(recverSocket, 5) == SOCKET_ERROR)
	{
		printf("Receiver socket listen failed with error: %d\n", WSAGetLastError());
		return;
	}
	printf("Listening for connections\n");

	while (true)
	{
		if (sendto(senderSocket, sendBuffer, strlen(sendBuffer), 0, static_cast<SOCKADDR*>(socket_addrinfo_sender->ai_addr), socklen_t(socket_addrinfo_sender->ai_addrlen)) == SOCKET_ERROR)
		{
			printf("Didnt send shit: %ld\n", WSAGetLastError());
		} 
		else
		{
			printf("Attempted to broadcast the number %x\n", randNum);
		}

		AcceptSocket = accept(recverSocket, NULL, NULL);

		if (AcceptSocket == INVALID_SOCKET)
		{
			printf("Accept failed with error: %d\n", WSAGetLastError());
		}
		else
		{
			printf("Accepted socket\n");
			runConnection(AcceptSocket, randNum);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	}
}

void setupServer()
{
	std::string localIPandSubnet;
	if (int findResult = findValidLocalBroadcastIP(localIPandSubnet) != 0)
	{
		printf("Failed to create broadcast IP. IDK WHY HAHA, but I do here's the error code I arbitrarily made: %d\n", findResult);
		return;
	}
	else
	{
		printf("Broadcasting to: %s\n", localIPandSubnet.c_str());
	}

	WSADATA wsaData;
	if (int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup Failed : %d\n", iResult);
		WSACleanup();
		return;
	}
	else
	{
		printf("Winsock DLL status is %s\n", wsaData.szSystemStatus);
	}

	addrinfo getaddrinfo_hints_sender = {};
	addrinfo getaddrinfo_hints_recver = {};

	getaddrinfo_hints_sender.ai_family = AF_UNSPEC;
	getaddrinfo_hints_sender.ai_socktype = SOCK_DGRAM;
	getaddrinfo_hints_sender.ai_protocol = IPPROTO_UDP;
	getaddrinfo_hints_sender.ai_flags = AI_ADDRCONFIG;

	getaddrinfo_hints_recver.ai_family = AF_INET;
	getaddrinfo_hints_recver.ai_socktype = SOCK_STREAM;
	getaddrinfo_hints_recver.ai_protocol = IPPROTO_TCP;
	getaddrinfo_hints_recver.ai_flags = AI_PASSIVE;

	addrinfo* getaddrinfo_results = nullptr;
	if (getaddrinfo(localIPandSubnet.c_str(), std::to_string(BROADCAST_PORT).c_str(), &getaddrinfo_hints_sender, &getaddrinfo_results) != 0)
	{
		printf("getaddrinfo failed!\n");
		return;
	}
	for (addrinfo* addr = getaddrinfo_results; addr; addr = addr->ai_next)
	{
		senderSocket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (senderSocket != INVALID_SOCKET)
		{
			printf("Made sender socket\n");
			socket_addrinfo_sender = addr;
			break;
		}
	}
	if (senderSocket == INVALID_SOCKET)
	{
		printf("Unable to make sender socket\n");
		WSACleanup();
		return;
	}

	if (getaddrinfo(NULL, std::to_string(TCP_PORT).c_str(), &getaddrinfo_hints_recver, &getaddrinfo_results) != 0)
	{
		printf("getaddrinfo failed!\n");
		closesocket(senderSocket);
		WSACleanup();
		return;
	}
	for (addrinfo* addr = getaddrinfo_results; addr; addr = addr->ai_next)
	{
		recverSocket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (recverSocket != INVALID_SOCKET)
		{
			printf("Made receiver socket\n");
			socket_addrinfo_recver = addr;
			break;
		}
	}
	if (recverSocket == INVALID_SOCKET)
	{
		printf("Unable to make receiver socket\n");
		closesocket(senderSocket);
		WSACleanup();
		return;
	}
	//printf("funny binder: %x\n", socket_addrinfo_recver->ai_addr);
	if (bind(recverSocket, static_cast<SOCKADDR*>(socket_addrinfo_recver->ai_addr), socklen_t(socket_addrinfo_recver->ai_addrlen)) == SOCKET_ERROR)
	{
		printf("Couldn't bind receiver socket with error: %d\n", WSAGetLastError());
		closesocket(senderSocket);
		closesocket(recverSocket);
		WSACleanup();
		return;
	}
	else
	{
		printf("Binded receiver socket!\n");
	}

	freeaddrinfo(getaddrinfo_results);
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

void cleanupNetwork()
{
	closesocket(senderSocket);
	closesocket(recverSocket);
	WSACleanup();
}

int findValidLocalBroadcastIP(std::string &broadcastIP)
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
