#pragma once

class Network
{
public:
	void runBroadcaster();
	void setupServer();
	void closeBroadcaster();
	void cleanupNetwork();

	void sendChao(CHAO_PARAM_GC* chaoToSend);
	void receiveChao(CHAO_PARAM_GC* newChaoSlot);

	bool getIsConnected();
	int getIsSentChao();
	bool getIsPhoneRequestingSend();
	void confirmChaoMessage();

	Network(std::string gameName);

private:
	int findValidLocalBroadcastIP(std::string& broadcastIP);
	void runConnection(SOCKET connectionSocket, sockaddr* connectionAddr, int addrLen);
	static void setNonBlocking(SOCKET* socket, bool state);

	void loadReceivedChao(char* buffer);

	std::string gameName;

	bool isSetup = false;
	bool isConnected = false;
	bool wantToSendChao = false;
	bool chaoSendFail = false;
	bool chaoSent = false;
	bool wantToClose = false;
	bool phoneSendRequesting = false;
	bool acceptSendRequest = false;
	bool receivedChaoSuccessfully = false;
	int sendChaoAttempts = 0;
	int receiveChaoAttempts = 0;

	CHAO_PARAM_GC* chaobuffer;
	CHAO_PARAM_GC* createNewChaoPointer;
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
		TransferToPCConfirmed,
		TransferToPhoneSuccess,
		TransferToPhoneFailure
	};

	receivedMessageStatus handleIncomingMessage(char* buffer, int recvResult);
};
