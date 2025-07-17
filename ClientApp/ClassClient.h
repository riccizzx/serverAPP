#pragma once

#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

const int PORT = 9909; // Port number for the server
//std::string ip = "127.0.0.1";

const int MAX_BUFFER_SIZE = 4096; // Maximum buffer size for sending/receiving data

namespace Cl {
	class Client {
	public:
		void init();
		void connectToServer();
		void stop();

	private:
		int var = 0;
		WSADATA WSAData;
		SOCKET clientSock;
		sockaddr_in clientAddr;

	};
}