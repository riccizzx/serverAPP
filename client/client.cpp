#include <iostream>    // For console input/output
#include <WS2tcpip.h>  // For inet_pton
#include <string>      // For std::string
#include <winsock2.h>  // For Winsock functions

#pragma comment(lib, "ws2_32.lib") // Link with the Winsock library

#define SERVER_IP "127.0.0.1" // The IP address of the server (localhost)
#define SERVER_PORT 9909      // The port the server is listening on

//FOR RUN THIS CREATE ANOTHER C++ SOLUTION AND PASTE THIS CODE, THIS IS WORK LIKE A CLIENT MAKING REQUEST TO THE SERVER 
/*
/**
 * @brief Main function for the client application.
 * Initializes Winsock, creates a socket, connects to the server,
 * sends messages, and receives responses.
 * @return 0 on successful execution, 1 on error.
 */

/*
int main() {
	// 1. initialize client socket
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "ERROR: " << WSAGetLastError();
		return 1;
	}

	// 2. declare client SOCKET
	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET) {
		std::cerr << "ERROR: can't create socket " << WSAGetLastError();
		return 1;
	}

	// 3. fill a hint struct with server information
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(SERVER_PORT);
	//convert IP string in bytes
	inet_pton(AF_INET, SERVER_IP, &hint.sin_addr);

	// 4. connected to the server
	std::cout << "attempting to connect to server at: " << SERVER_IP << ": " << SERVER_PORT << "...\n";
	int connResult = connect(clientSock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR) {
		std::cerr << "ERROR: client can't connect to the serve! ERROR: " << WSAGetLastError();
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Connection succefull! \n";
	}

	// buffer for sending and receiving data
	char buffer[4096];
	ZeroMemory(buffer, 4096);
	std::string userInput;

	// first, receive the welcome message from the server
	int bytesReceived = recv(clientSock, buffer, sizeof(buffer), 0);
	if (bytesReceived > 0) {
		buffer[bytesReceived];
		std::cout << "Server: " << std::string(buffer) << "\n";
	}
	else if (bytesReceived == 0) {
		std::cout << "Server disconnected." << "\n";
		closesocket(clientSock);
		WSACleanup();
		return 0;
	}
	else {
		std::cerr << "ERROR: Error receiving welcome message: " << WSAGetLastError() << "\n";
		closesocket(clientSock);
		WSACleanup();
		return 1;
	}

	// 5. loop for send message and received echos
	do {

		std::cout << "Enter message ('exit' to quit): ";
		std::getline(std::cin, userInput); // Read user input from console

		if (userInput.length() > 0) {
			// send text to the server
			// add 1 to length for the null terminator (though not strictly necessary for Winsock send)
			int sendResult = send(clientSock, userInput.c_str(), userInput.length(), 0);
			if (sendResult == SOCKET_ERROR) {
				std::cerr << "ERROR: Can't send back to the server. ERROR: " << WSAGetLastError() << "\n";
				break;// exit loop
			}

			// wait the response from the server
			ZeroMemory(buffer, 4096);
			bytesReceived = recv(clientSock, buffer, sizeof(buffer), 0);
			if (bytesReceived > 0) {
				buffer[bytesReceived];
				std::cout << "Server echo: " << std::string(buffer) << "\n";
			}
			else if (bytesReceived == 0) {
				//server close the connection
				std::cout << "Server disconnected. \n";
				break;
			}
			else {
				std::cerr << "ERROR: Error receiving data! Error: " << WSAGetLastError() << "\n";
				break;
			}
		}
	} while (userInput != "exit" || "EXIT"); // continue untill user type "EXIT"

	closesocket(clientSock);
	WSACleanup();
	std::cout << "Client application finished. \n";

	return 0;
}*/
