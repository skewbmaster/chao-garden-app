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
	void runConnection(SOCKET connectionSocket, int);

	static void setNonBlocking(SOCKET* socket, bool state);


	bool isSetup = false;

	SOCKET broadcastSocket, commsSocket;
	addrinfo* socket_addrinfo_sender;
	addrinfo* socket_addrinfo_recver;
};
