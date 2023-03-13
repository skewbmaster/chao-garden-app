#pragma once

class Network
{
public:
	void runBroadcaster();
	void setupServer();
	void closeBroadcaster();
	void cleanupNetwork();

	void sendChao(CHAO_PARAM_GC* chaoToSend);
	CHAO_PARAM_GC* receiveChao();

	bool getIsConnected();
	int getIsSentChao();
	void confirmChaoMessage();

	Network(std::string gameName);


private:
	int findValidLocalBroadcastIP(std::string& broadcastIP);
	void runConnection(SOCKET connectionSocket, sockaddr* connectionAddr, int addrLen);
	static void setNonBlocking(SOCKET* socket, bool state);


	std::string gameName;

	bool isSetup = false;
	bool isConnected = false;
	bool wantToSendChao = false;
	bool chaoSendFail = false;
	bool chaoSent = false;
	bool wantToClose = false;

	CHAO_PARAM_GC* chaobuffer;
	uint32_t chaobufferHash;

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
		TransferToPCRequest,
		TransferToPhoneSuccess,
		TransferToPhoneFailure
	};

	receivedMessageStatus handleIncomingMessage(char* buffer, int recvResult);
};
