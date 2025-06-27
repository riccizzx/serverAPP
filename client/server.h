#pragma once

#include <iostream>
#include <WS2tcpip.h> // For inet_ntop
#include <string>
#include <vector>     // For managing multiple clients
#include <winsock2.h> // For Winsock functions

#pragma comment(lib, "ws2_32.lib") // Link with the Winsock library

#define PORT 9909 // Default port for the server

class Net {
public:
	Net();
	~Net();

	void initialize();
	void createSocket();
	void bindandListen();
	void startEventLoop();
	void run();

private:
	void cleanup();
	int nRet;

	SOCKET serverSock;
	FD_SET masterReadSet;   // Master set of socket descriptors for reading
	FD_SET masterExceptSet; // Master set of socket descriptors for exceptional conditions
	int maxFd;              // The highest-valued socket descriptor in any set, plus 1

	std::vector<SOCKET> clientSockets; // Dynamically stores all connected client sockets
};