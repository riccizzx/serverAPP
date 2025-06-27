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

//function to close and cleanup SOCKETs
void Net::cleanup() {
	//close all connected client sockets
	for (SOCKET sock : clientSockets) {
		if (sock != INVALID_SOCKET) {
			closesocket(sock);
		}
	}
	clientSockets.clear();//clear all vectors
	if (serverSock != INVALID_SOCKET) {
		closesocket(serverSock);
		serverSock = INVALID_SOCKET;
	}
	WSACleanup();
	std::cout << "Winsock cleaned up and all sockets closed.\n";
}

//initialize winsock in windows
void Net::initialize() {
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
		std::cerr << "ERROR! " << WSAGetLastError();
		WSACleanup();
	}
	else {
		std::cout << "WSADATA initialize sucefull\n";
	}
}

//create server SOCKET
void Net::createSocket() {
	serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSock == INVALID_SOCKET) {
		std::cerr << "ERROR: can't initialize SOCKET " << WSAGetLastError();
		WSACleanup();
	}
	else {
		std::cout << "SOCKET creation sucefull\n";
	}
}

//bind server addr and listen for connections
void Net::bindandListen() {
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);//convert port to network byte order
	serverAddr.sin_addr.s_addr = INADDR_ANY;//bind to get the local IP

	nRet = bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (nRet == SOCKET_ERROR) {
		std::cerr << "ERROR: can't bind the socketAddr " << WSAGetLastError();
		WSACleanup();
		closesocket(serverSock);
	}
	else {
		std::cout << "bind sucefull\n";
	}

	nRet = listen(serverSock, SOMAXCONN);
	if (nRet == SOCKET_ERROR) {
		std::cerr << "ERROR: can't listen the socketAddr " << WSAGetLastError();
		WSACleanup();
		closesocket(serverSock);
	}
}

//start loop event for incomming connections
void Net::startEventLoop() {
	maxFd = serverSock;
	std::cout << "Starting server loop. Press Ctrl+C to stop\n";

	while (1) {
		// Create temporary FD_SETs for select()
		// select() modifies the sets, so we use temporary copies of the master sets.
		FD_SET tempReadSet = masterReadSet;
		FD_SET tempExceptSet = masterExceptSet;
		FD_SET(serverSock, &tempReadSet);
		FD_SET(serverSock, &tempExceptSet);
		if (serverSock > maxFd) maxFd = serverSock;
		
		// crete a temporariy FD_SETs for select()
		// select() modifies the sets, so we use temporary copies of the master sets.

		for (SOCKET client_sock : clientSockets) {
			FD_SET(client_sock, &tempReadSet);
			FD_SET(client_sock, &tempExceptSet);
			if (client_sock > maxFd) maxFd = client_sock;
		}

		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		nRet = select(maxFd + 1, &tempReadSet, NULL, &tempExceptSet, &tv);
		if (nRet == SOCKET_ERROR) {
			std::cerr << "ERROR: Select failed with error: " << WSAGetLastError() << std::endl;
			break; // Break the loop on a critical error, leading to server shutdown
		}
		if (nRet == 0) {
			// No sockets are ready (timeout occurred)
			std::cout << "No activity on sockets. Waiting..." << std::endl; // Can be too chatty
			continue; // Continue to the next iteration of the loop
		}
		

		if (FD_ISSET(serverSock, &tempReadSet)) {
			sockaddr_in clientAddr;
			int clientAddrSize = sizeof(clientAddr);
			SOCKET client_sock = accept(serverSock, (sockaddr*)&clientAddr, &clientAddrSize);
			if (client_sock == INVALID_SOCKET) {
				std::cerr << "ERROR: Accept failed with error: " << WSAGetLastError();
				WSACleanup();
			}
			else {
				clientSockets.push_back(client_sock);
				FD_SET(client_sock, &masterReadSet);
				FD_SET(client_sock, &masterExceptSet);

				if (client_sock > maxFd) maxFd = client_sock;

				char host[NI_MAXHOST]; //buffer for client HostName
				char service[NI_MAXSERV]; //buffer for client PORT

				ZeroMemory(host, NI_MAXHOST);
				ZeroMemory(service, NI_MAXSERV);

				if (getnameinfo((sockaddr*)&clientAddr, sizeof(clientAddr),
					host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
					std::cout << "Client connected from: " << host << " on port " << service << "\n";
				}
				else {
					// Fallback to numeric IP if hostname resolution fails
					inet_ntop(AF_INET, &clientAddr.sin_addr, host, NI_MAXHOST);
					std::cout << "Client connected from " << host << " on port " << ntohs(clientAddr.sin_port) << "\n";
				}
				//msg to the connected client
				std::string welcomeMsg = "Welcome to the echo server! type exit to disconnected.\r\n";
				send(client_sock, welcomeMsg.c_str(), welcomeMsg.length(), 0);
			}
			nRet--; // decrement nRet if client disconect, (-1 client in server)
		}					
		
		
		// Iterate through all connected client sockets to check for data or disconnections.
		// Using a traditional for loop with index for safe removal of elements.
		for (size_t i = 0;i < clientSockets.size() && nRet > 0;) {
			SOCKET client_sock = clientSockets[i];

			if (FD_ISSET(client_sock, &tempReadSet)) {
				// This client socket has data to read (or has disconnected)
				char buffer[4096]; //buffer for recived data
				ZeroMemory(buffer, 4096);// clear buffer

				//recive datas from client 
				int bytesRecived = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
				if (bytesRecived < 0) {
					std::cout << "Client: " << client_sock << "has disconnected. \n" << WSAGetLastError();
				}
				else {
					std::cerr << "ERROR: recv failed for socket " << client_sock << " ERROR: " << WSAGetLastError() << "\n";
				}

				//clean-up and remove the disconnected client
				FD_CLR(client_sock, &masterReadSet);
				FD_CLR(client_sock, &masterExceptSet);
				closesocket(client_sock);
				clientSockets.erase(clientSockets.begin() + i);//remove from our vector of active clients

				//if disconnected socket was the highest descriptor, recalculate maxFd
				if (client_sock == maxFd) {
					maxFd == serverSock;//start recalculate from the server
					for (SOCKET s : clientSockets) {
						if (s > maxFd) maxFd == s;
					}
				}
				else {
					//data recived successfully
					buffer[bytesRecived] = '\0'; // Null-terminate the received data
					std::string recivedMsg(buffer);

					std::cout << "Recived from Client: " << client_sock << ": " << recivedMsg << "\n";

					//if client sent "exit", gracefully disconnected them
					if (recivedMsg.find("exit") != std::string::npos || recivedMsg.find("EXIT") != std::string::npos) {
						std::string goodbyeMsg = "Goodbye!\r\n";
						send(client_sock, goodbyeMsg.c_str(), goodbyeMsg.length(), 0);

						FD_CLR(client_sock, &masterReadSet);
						FD_CLR(client_sock, &masterExceptSet);
						closesocket(client_sock);
						clientSockets.erase(clientSockets.begin() + i);
						if (client_sock == maxFd) {
							maxFd = serverSock;
							for (SOCKET s : clientSockets) {
								if (s > maxFd) maxFd = s;
							}
						}
						//	do not increment
					}
					else {
						//echo back the recived data to the same client
						int bytesSent = send(client_sock, buffer, bytesRecived, 0);
						if (bytesSent == SOCKET_ERROR) {
							std::cerr << "ERROR: send failed for socket \n" << client_sock << " with error: " << WSAGetLastError << "\n";
						}
						i++;
					}
				}
				nRet--;
			}
			else if (FD_ISSET(client_sock, &tempExceptSet)) {
				std::cerr << "Exceptional condition on socket: " << client_sock << " with error: " << WSAGetLastError() << "\n";
				FD_CLR(client_sock, &tempReadSet);
				FD_CLR(client_sock, &tempExceptSet);
				closesocket(client_sock);
				clientSockets.erase(clientSockets.begin() + i);
				if (client_sock == maxFd) {
					maxFd = serverSock;
					for (SOCKET s : clientSockets) {
						if (s > maxFd) maxFd = s;
					}
				}
				nRet--; //decrement nRet
			}
			else {
				i++; //no activity on this socket, and move to the next
			}
		}
	}
}

//function to call all necessary methods to run the code
void Net::run() {
	std::cout << "* * RUN * *\n";
	Net net;
	net.initialize(); //Step 1: Initialize winsock
	net.createSocket(); //Step 2: Create the server socket
	net.bindandListen(); //Step 3: Bind the socket and start listening
	net.startEventLoop(); //Step 4: Enter the main server loop
}