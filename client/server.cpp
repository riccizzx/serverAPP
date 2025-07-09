#include "server.h"

/**
  *							  *
  * 	SERVER PROGRAM		***
  *						      *
  */

//Initializes member variables to default states and clears FD_SETs.

Net::Net() : serverSock(INVALID_SOCKET), nRet(0), maxFd(0) {
	FD_ZERO(&masterReadSet);
	FD_ZERO(&masterExceptSet);
}

Net::~Net() {
	cleanup();
}

void Net::cleanup() {
	for (SOCKET sock : clientSockets) {
		if (sock != INVALID_SOCKET) closesocket(sock);
	}
	clientSockets.clear();

	if (serverSock != INVALID_SOCKET) {
		closesocket(serverSock);
		serverSock = INVALID_SOCKET;
	}

	WSACleanup();
	std::cout << "Winsock cleaned up and all sockets closed.\n";
}

void Net::initialize() {
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
		std::cerr << "ERROR! " << WSAGetLastError() << "\n";
		WSACleanup();
		exit(1);
	}
	std::cout << "WSADATA initialization successful.\n";
}

void Net::createSocket() {
	serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSock == INVALID_SOCKET) {
		std::cerr << "ERROR: can't create socket: " << WSAGetLastError() << "\n";
		WSACleanup();
		exit(1);
	}
	std::cout << "SOCKET creation successful.\n";
}

void Net::bindandListen() {
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	try {
		nRet = bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
		if (nRet == SOCKET_ERROR) {
			std::cerr << "ERROR: bind failed: " << WSAGetLastError() << "\n";
			cleanup();
			exit(1);
		}
		std::cout << "Bind successful.\n";

		nRet = listen(serverSock, SOMAXCONN);
		if (nRet == SOCKET_ERROR) {
			std::cerr << "ERROR: listen failed: " << WSAGetLastError() << "\n";
			cleanup();
			exit(1);
		}
		std::cout << "Listening on port " << PORT << "\n";
	}
	catch (const std::exception& e) {
		std::cerr << "Exception during address setup: " << e.what() << "\n";
		cleanup();
		exit(1);
	}
}

void Net::startEventLoop() {
	FD_SET(serverSock, &masterReadSet);
	FD_SET(serverSock, &masterExceptSet);
	maxFd = serverSock;

	//std::cout << "Server event loop started.\n";

	while (true) {
		fd_set tempReadSet = masterReadSet;
		fd_set tempExceptSet = masterExceptSet;

		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		nRet = select(maxFd + 1, &tempReadSet, nullptr, &tempExceptSet, &tv);

		if (nRet == SOCKET_ERROR) {
			std::cerr << "Select failed: " << WSAGetLastError() << "\n";
			break;
		}
		if (nRet == 0) continue;

		// Accept new client
		if (FD_ISSET(serverSock, &tempReadSet)) {
			sockaddr_in clientAddr;
			int addrLen = sizeof(clientAddr);
			SOCKET clientSock = accept(serverSock, (sockaddr*)&clientAddr, &addrLen);

			if (clientSock == INVALID_SOCKET) {
				std::cerr << "Accept failed: " << WSAGetLastError() << "\n";
			}
			else {
				std::cout << "New client connected (socket: " << clientSock << ").\n";
				clientSockets.push_back(clientSock);
				FD_SET(clientSock, &masterReadSet);
				FD_SET(clientSock, &masterExceptSet);
				if (clientSock > maxFd) maxFd = clientSock;

				char host[NI_MAXHOST], service[NI_MAXSERV];
				ZeroMemory(host, NI_MAXHOST);
				ZeroMemory(service, NI_MAXSERV);

				if (getnameinfo((sockaddr*)&clientAddr, sizeof(clientAddr), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
					std::cout << "New connection from " << host << ": " << service << "\n";
				}
				else {
					inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
					std::cout << "New connection from " << host << ": " << ntohs(clientAddr.sin_port) << "\n";
				}

				std::string welcomeMsg = "Welcome to the echo server! Type 'exit' to disconnect.\r\n";
				send(clientSock, welcomeMsg.c_str(), welcomeMsg.length(), 0);
			}
			nRet--;
		}

		// Client communication
		for (size_t i = 0; i < clientSockets.size() && nRet > 0;) {
			SOCKET clientSock = clientSockets[i];

			if (FD_ISSET(clientSock, &tempReadSet)) {
				char buffer[4096];
				ZeroMemory(buffer, sizeof(buffer));

				int bytesReceived = recv(clientSock, buffer, sizeof(buffer) - 1, 0);

				if (bytesReceived <= 0) {
					std::cout << "Client disconnected (socket: " << clientSock << ").\n";
					FD_CLR(clientSock, &masterReadSet);
					FD_CLR(clientSock, &masterExceptSet);
					closesocket(clientSock);
					clientSockets.erase(clientSockets.begin() + i);
					if (clientSock == maxFd) {
						maxFd = serverSock;
						for (SOCKET s : clientSockets) if (s > maxFd) maxFd = s;
					}
					nRet--;
					continue;
				}

				buffer[bytesReceived] = '\0';
				std::string msg(buffer);

				if (msg.find("exit") != std::string::npos || msg.find("EXIT") != std::string::npos) {
					std::string goodbye = "Goodbye!\r\n";
					send(clientSock, goodbye.c_str(), goodbye.size(), 0);
					FD_CLR(clientSock, &masterReadSet);
					FD_CLR(clientSock, &masterExceptSet);
					closesocket(clientSock);
					clientSockets.erase(clientSockets.begin() + i);
					if (clientSock == maxFd) {
						maxFd = serverSock;
						for (SOCKET s : clientSockets) if (s > maxFd) maxFd = s;
					}
					nRet--;
					continue;
				}

				std::cout << "Client " << clientSock << ": " << msg << "\n";
				send(clientSock, buffer, bytesReceived, 0);
				i++;
				nRet--;
			}
			else if (FD_ISSET(clientSock, &tempExceptSet)) {
				std::cerr << "Exception on socket " << clientSock << ".\n";
				FD_CLR(clientSock, &masterReadSet);
				FD_CLR(clientSock, &masterExceptSet);
				closesocket(clientSock);
				clientSockets.erase(clientSockets.begin() + i);
				if (clientSock == maxFd) {
					maxFd = serverSock;
					for (SOCKET s : clientSockets) if (s > maxFd) maxFd = s;
				}
				nRet--;
			}
			else {
				i++;
			}
		}
	}
}

void Net::run() {
	std::cout << "* * RUN * *\n";
	initialize();
	createSocket();
	bindandListen();
	startEventLoop();
}
