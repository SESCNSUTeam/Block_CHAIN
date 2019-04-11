#include "server.h"

; CryptoPP::AutoSeededRandomPool rng;

int data_counter = 0;

void main() {
	std::cout << "Server started\n";

	block_chain.push_back(buildBlock());

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
	}
}



void sendComputationPower() {
	double calculationPower = 0;

	for (auto iter = miners.begin(); iter != miners.end(); ++iter) {
		calculationPower += iter->second;
	}

	int pow = ceil(log(calculationPower*TIME) / log(2));
	std::string s = "Complexity:" + std::to_string(pow) + ";";
	char const *sendbuf = s.c_str();

	for (auto iter = miners.begin(); iter != miners.end(); ++iter) {
		std::cout << sendbuf << "\t" << iter->first << std::endl;
		send(iter->first, sendbuf, (int)strlen(sendbuf), 0);
	}
	std::cout << std::endl;
}

bool checkBlock(Block block) {
	std::cout << "Checking block on correction..." << std::endl;
	Block b = block;
	std::string data = b.prevHash + std::to_string(b.nonce) + b.data + std::to_string(b.number) + std::to_string(b.time);
	std::string hash = SHA256Hash(data);

	if (b.number != block_chain.size()) {

		std::cout << "This block from the past of the future: " << std::to_string(b.number) << std::endl;
		return false;

	}

	if (b.hash == hash) {

		std::string d = b.data;

		if (d != "") {
			// 0~123!~
			while (int tilda_pos = d.find('~') != -1) {
				std::string n = d.substr(0, tilda_pos);
				int num = std::stoi(n);
				transactions_un.erase(num);
				d = d.substr(tilda_pos + 1);
				tilda_pos = d.find('~');
				d = d.substr(tilda_pos + 1);
			}

		}

		return true;
	}

	return false;
}

void sendLastBlockToAll() {
	std::string s = "Block:" + convertBlockToString(block_chain[block_chain.size() - 1]) + ";";
	char const *sendbuf = s.c_str();
	for (auto iter = miners.begin(); iter != miners.end(); ++iter) {
		send(iter->first, sendbuf, (int)strlen(sendbuf), 0);
	}
	std::cout << "Last block sended to all: " << s << std::endl;
}

void sendDataToAll(int num) {
	std::string s = "Transactions:" + std::to_string(num) + "~" + transactions_un[num] + "~" + ";";
	char const *sendbuf = s.c_str();
	for (auto iter = miners.begin(); iter != miners.end(); ++iter) {
		std::cout << "Sending transaction to " << iter->first << "\nData: " << s << std::endl;
		send(iter->first, sendbuf, (int)strlen(sendbuf), 0);
	}
}

void sendAllData(SOCKET ClientSession) {
	std::cout << "Sending all transactions availible to " << ClientSession << std::endl;
	for (auto iter = transactions_un.begin(); iter != transactions_un.end(); ++iter) {
		std::string s = "Transactions:" + std::to_string(iter->first) + "~" + iter->second + "~" + ";";
		std::cout << "Sending " << s << "to " << ClientSession << std::endl;
		char const *sendbuf = s.c_str();
		send(ClientSession, sendbuf, (int)strlen(sendbuf), 0);
		Sleep(100);
	}
}

void sendLastBlock(SOCKET ClientSocket) {
	Block block = block_chain[block_chain.size() - 1];
	std::string hexBlock = "Block:" + convertBlockToString(block_chain[block_chain.size() - 1]) + ";";
	std::cout << "Sending last block to " << ClientSocket << "\tLast Block: " << hexBlock << std::endl;

	char const *sendbuf = hexBlock.c_str();
	send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
	std::cout << std::endl;
}


void miner(SOCKET ClientSocket) {


	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;
	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

	if (iResult > 0) {
		std::cout << "HashSpeed received: " << std::stod(std::string(recvbuf)) << std::endl;
		double hashSpeed = std::stod(std::string(recvbuf));

		miners[ClientSocket] = hashSpeed;
		sendComputationPower();
		sendAllData(ClientSocket);
		sendLastBlock(ClientSocket);
	}
	else if (iResult == 0)
		printf("Connection closing...\n");
	else {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		//WSACleanup();

	}


	do {
		std::cout << "Waiting for an incoming data... \n";
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			//data handling
			std::cout << "Data received: " << cut(std::string(recvbuf)) << std::endl;
			Block block = convertStringToBlock(cut(std::string(recvbuf)));
			if (checkBlock(block)) {
				block_chain.push_back(block);
				std::cout << "Correct block" << std::endl;
				sendLastBlockToAll();
			}
			else {
				std::cout << "Incorrect block" << std::endl;
			}
			std::cout << std::endl;
		}
		// data sending
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
		}

	} while (iResult > 0);


	iResult = shutdown(ClientSocket, SD_SEND);
	miners.erase(ClientSocket);
	sendComputationPower();
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
	}

	closesocket(ClientSocket);

	std::cout << "Miner disconnected\n";

}

bool transactions(std::string tran) {
	// Transaction; info; 
	int pos = tran.find(':');
	std::string s;
	s = tran.substr(0, pos);
	if (s == "Transactions") {
		s = tran.substr(pos + 1);
		pos = s.find('!');
		std::string data = s.substr(0, pos);
		std::string trans_data = data;
		s = s.substr(pos + 1);
		std::string curr_username = "";
		std::string curr_password = "";
		bool do_next = false;

		do {
			pos = s.find('!');
			if (pos < 0) {
				do_next = true;
				break;
			}
			curr_username = s.substr(0, pos);
			s = s.substr(pos + 1);
			pos = s.find('!');
			
			if (pos < 0) {
				return false;
				break;
			}
			curr_password = s.substr(0, pos);
			s = s.substr(pos + 1);
			std::string additional_data = create_digital_signature(trans_data, curr_username, curr_password, log_pas, log_dsa, rng);
			if (additional_data == "") {
				return false;
				break;
			}
			else {
				data += "!" + additional_data;
				do_next = true;
			}
		} while (true);
		if (do_next) {
			transactions_un[data_counter] = data;
			sendDataToAll(data_counter);
			data_counter++;
		}
		
	}
	else {
		return false;
	}
	return true;
}

bool user_register(std::string str, SOCKET Client) {
	// Transaction; info; 
	int pos = str.find(':');
	std::string s;
	s = str.substr(0, pos);
	if (s == "Register") {
		s = str.substr(pos + 1);
		pos = s.find(';');
		s = s.substr(0, pos);
		pos = s.find('|');
		std::string log = s.substr(0, pos);
		s = s.substr(pos + 1);
		if (log_pas.count(log) > 0) {
			std::cout << "This login exists: " << log << std::endl;
			return false;
		}
		log_pas[log] = s;
		DSA::PrivateKey key;
		key.GenerateRandomWithKeySize(rng, 1024);
		log_dsa[log] = key;
		DSA::PublicKey pKey;
		pKey.AssignFrom(key);
		std::string hexpk;
		pKey.Save(StringSink(hexpk).Ref());
		std::cout << "Registered " << log << std::endl;

		std::string message = "RegisterConfirmation:True|" + log + "|" + string_to_hex(hexpk) + ";";
		char const *sendbuf = message.c_str();
		send(Client, sendbuf, (int)strlen(sendbuf), 0);
		return true;
	}
	return false;
}

void user(SOCKET ClientSocket) {
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;

	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			std::string d = std::string(recvbuf);
			std::cout << "Data received: " << cut(d) << std::endl;
			
			if (transactions(d)) {
				std::string s = "Confirmation:True;";
				char const *sendbuf = s.c_str();
				send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
				continue;
			}
			else {
				std::string s = "Confirmation:False;";
				char const *sendbuf = s.c_str();
				send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
			}
			//ПЕредеелать парсер с нуля
			if (!user_register(d, ClientSocket)) {
				std::string message = "RegisterConfirmation:False;";
				char const *sendbuf = message.c_str();
				send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
			}

		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
		}

	} while (iResult > 0);


	iResult = shutdown(ClientSocket, SD_SEND);
	users.erase(ClientSocket);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
	}

	closesocket(ClientSocket);

	std::cout << "User disconnected\n";


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
	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

	if (iResult > 0) {
		if (cut(std::string(recvbuf)) == "User") {
			std::cout << "User connected: " << ClientSocket << std::endl;
			users[ClientSocket] = "name";
			user(ClientSocket);
		}
		else if (cut(std::string(recvbuf)) == "Miner") {
			std::cout << "Miner connected" << std::endl;
			miners[ClientSocket] = 0;
			miner(ClientSocket);
		}
		else {
			std::cout << "Alien detected" << std::endl;
		}
	}


	// shutdown the connection since we're done

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

