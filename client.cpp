#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
	// Create a socket
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1) {
		std::cerr << "Failed to create socket." << std::endl;
		return 1;
	}

	// Set up server details
	std::string serverIP = "10.11.6.6"; // Replace with your IRC server IP
	int serverPort = 8000; // Replace with your IRC server port

	// Set up server address
	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverPort);
	if (inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr)) <= 0) {
		std::cerr << "Invalid address/Address not supported." << std::endl;
		return 1;
	}

	// Connect to the server
	if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		std::cerr << "Connection failed." << std::endl;
		return 1;
	}

	// Send and receive data
	std::string message;
	while (true) {
		std::cout << "Enter a message to send: ";
		if (std::getline(std::cin, message).eof()) {
			break;
		}


		// Send the message to the server
		if (send(clientSocket, message.c_str(), message.size(), 0) < 0) {
			std::cerr << "Failed to send message." << std::endl;
			return 1;
		}

		// Receive a response from the server
		// char buffer[4096] = {0};
		// int valread = read(clientSocket, buffer, 4096);
		// if (valread > 0) {
		// 	std::cout << "Server: " << buffer << std::endl;
		// }
	}

	// Close the socket
	close(clientSocket);

	return 0;
}