#include "server.h"

//function main to run the server
int main() {
	//Net net;
	Networking::Net net;
	try {
		net.run();
	}
	catch (const std::exception& e) {
		std::cerr << "An error occurred: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}