#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <iostream>

#include "Main.h"

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "43012"
#define TIME 10

std::string cut(std::string str) {
	int len = (int)strlen(str.c_str());
	std::string result = "";
	for (int i = 0; i < len; ++i) {
		if (str[i] == ';') break;
		result += str[i];
	}
	return result;
}

std::map <SOCKET, double> connections;

void sendComputationPower() {
	double calculationPower = 0;
	
	for (auto iter = connections.begin(); iter != connections.end(); ++iter) {
		calculationPower += iter->second;
	}

	int pow = ceil(log(calculationPower*TIME) / log(2));
	std::string s = "Complexity:" + std::to_string(pow) + ";";
	char const *sendbuf = s.c_str();

	for (auto iter = connections.begin(); iter != connections.end(); ++iter) {
		std::cout << sendbuf << "\t" << iter->first << std::endl;
		send(iter->first, sendbuf, (int)strlen(sendbuf), 0);
	}
}

DWORD WINAPI ClientSession(LPVOID data) {
	// Process the client.
	std::cout << "Connected..." << std::endl;
	SOCKET ClientSocket = (SOCKET)data;

	int iSendResult;
	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
	}
	
	// Получаем мощность и отправляем всем новую сложность pow
	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		std::cout << "HashSpeed received: " << std::stod(std::string(recvbuf)) << std::endl;
		double hashSpeed = std::stod(std::string(recvbuf));
		
		connections[ClientSocket] = hashSpeed;
		sendComputationPower();
	}
	else if (iResult == 0)
		printf("Connection closing...\n");
	else {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		//WSACleanup();
	}
  
	// Receiving all types of new data until connection is closed
	
  do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			std::cout << "Data received: " << recvbuf << std::endl;
      //Handle this data somehow
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			//WSACleanup();
		}

	} while (iResult > 0);


	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	connections.erase(ClientSocket);
	sendComputationPower();
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		//WSACleanup();
	}

	// cleanup
	closesocket(ClientSocket);
	//WSACleanup();

	std::cout << "Client disconnected\n";

	return 0;

}

// Просто создает слушателя:
SOCKET SetUpListeningSocket() {
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return INVALID_SOCKET;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return INVALID_SOCKET;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return INVALID_SOCKET;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return INVALID_SOCKET;
	}

	freeaddrinfo(result);

	std::cout << "Listening for an incoming connection\n";

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return INVALID_SOCKET;
	}

	return ListenSocket;
}
 
int main(void)
{
	std::cout << "Server started\n";

	WSADATA wsaData;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	ListenSocket = SetUpListeningSocket();

	// Accept a client socket

	while ((ClientSocket = accept(ListenSocket, NULL, NULL))) {
		// Create a new thread for the accepted client (also pass the accepted client socket).
		DWORD dwThreadId;
		CreateThread(NULL, 0, ClientSession, (LPVOID)ClientSocket, 0, &dwThreadId);
		std::pair <SOCKET, double> session;
		session.first = ClientSocket;
		session.second = 0;
		connections.insert(session);
	}
}