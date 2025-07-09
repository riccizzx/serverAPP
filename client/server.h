#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define PORT 9909

#ifdef _WIN32
#define GETSOCKETERRNO() WSAGetLastError()
#else
#define GETSOCKETERRNO() errno

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
	SOCKET serverSock;
	std::vector<SOCKET> clientSockets;

	fd_set masterReadSet;
	fd_set masterExceptSet;

	int nRet;
	int maxFd;

	void cleanup();
};
