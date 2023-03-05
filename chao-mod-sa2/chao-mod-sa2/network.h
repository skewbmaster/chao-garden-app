#pragma once

class Network
{
public:
	void runBroadcaster();
	void setupServer();
	void cleanupNetwork();

	Network();


private:
	int findValidLocalBroadcastIP(std::string& broadcastIP);
	void runConnection(SOCKET connectionSocket, sockaddr* connectionAddr, int addrLen);
	static void setNonBlocking(SOCKET* socket, bool state);


	bool isSetup = false;
	bool wantToClose = false;

	SOCKET broadcastSocket, commsSocket;
	addrinfo* socket_addrinfo_sender;
	addrinfo* socket_addrinfo_recver;


	enum receivedMessageStatus
	{
		NotChaoADV,
		Failure,
		Closed,
		ConnectRequest,
		Keepalive,
		SeeChaoRequest,
		TransferToPC,
		TransferToPhone
	};

	receivedMessageStatus handleIncomingMessage(char* buffer, int recvResult);
};
