#include "ClassClient.h"

void Cl::Client::init() {
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		std::cerr << "ERROR! " << WSAGetLastError() << "\n";
		WSACleanup();
	}

	//client socket cfg
	SOCKET clientSock;
	clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSock == INVALID_SOCKET) {
		std::cerr << "ERROR! " << WSAGetLastError() << "\n";
		WSACleanup();
		return;
	}

	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &clientAddr.sin_addr);

	
}

void Cl::Client::stop() {
	if (clientSock != INVALID_SOCKET) {
		closesocket(clientSock);
		clientSock = INVALID_SOCKET;
	}
	WSACleanup();
	std::cout << "Client stopped.\n";
}

void Cl::Client::connectToServer() {
	if (connect(clientSock, (sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
		std::cerr << "ERROR! " << WSAGetLastError() << "\n";
		closesocket(clientSock);
		WSACleanup();
		return;
	}
	std::cout << "Connected to server at " << "127.0.0.1" << ":" << PORT << "\n";
	// Send a message to the server

	char buffer[MAX_BUFFER_SIZE] = { 0 }; // Buffer for sending data
	std::string msg;
	
	while (true) {
		std::cout << "Enter a message to the server: ";
		std::getline(std::cin, msg);
		if (msg == "exit") {
			std::cout << "You quit the server!\n";
			break;
		}
		else {
			buffer[msg.size()] = '\0';
			var = send(clientSock, msg.c_str(), msg.size(), 0);
			if (var != SOCKET_ERROR) {
				ZeroMemory(buffer, MAX_BUFFER_SIZE);
				var = recv(clientSock, buffer, MAX_BUFFER_SIZE, 0);
				if (var > 0) {
					std::cout << "Server response : " << std::string(buffer, 0, var) << "\n";
				}
			}
			else{
				std::cout << "Error sending message: " << WSAGetLastError() << "\n";
				closesocket(clientSock);
				WSACleanup();
				break;
			}
		}
	}
	std::cout << "Connections closed!\n";
	Cl::Client::stop();
}

int main() {
	Cl::Client client;
	client.init();
	client.connectToServer();
	return 0;
}