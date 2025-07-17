#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include <string_view>

#pragma comment(lib, "ws2_32.lib")

constexpr int PORT = 9909; // Port number for the server

namespace Networking {
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
}