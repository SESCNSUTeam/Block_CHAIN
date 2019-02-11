#include "Main.h"
using namespace std;

int complexity;
bool isServerRunning;

double getHashSpeed() {
	int counter = 0;
	string hash = SHA256Hash("0");
	auto start = std::chrono::high_resolution_clock::now();
	do {
		hash = SHA256Hash(hash);
		++counter;
	} while (counter < 100);
	auto end = std::chrono::high_resolution_clock::now();
	return counter / (std::chrono::duration<double>(end - start)).count();
}

Block createBlock(Block curr, int cp) {
	
	Block b = buildBlock();
	ofstream fout("blocks\\" + SHA256Hash(to_string(rand())) + ".txt");
	b.prev = &curr;
	b.number = curr.number + 1;
	b.computationPower = cp;
	b.prevHash = curr.hash;
	b.data = openFile("transactions\\input.txt");
	do {
		b.nonce = rand()*RAND_MAX + rand();
		b.time = time(NULL);
		b.hash = b.prevHash + to_string(b.nonce) + b.data + to_string(b.number) + to_string(b.time);
		b.hash = SHA256Hash(b.hash);
	} while (!if_pow(b.hash, cp));
	fout << b;
	fout.close();
	return b;
}

bool if_pow(string data, int c) {
	string d = binHash(data);
	for (int i = 0; i < c; ++i) {
		if (d[i] != '0') return false;
	}
	return true;
}

double getComputationPower(float time, int power) {
	return log(getHashSpeed()*time) / log(power);
}

DWORD WINAPI SessionWithServer(LPVOID data) {

	SOCKET ConnectSocket = (SOCKET)data;
	// Process the client.

	int iSendResult;
	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	std::string receivedData;

	// Receiving all kinds of data
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			std::cout << "Data received: " << recvbuf << std::endl;
			//Handle this data somehow
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
		}

	} while (iResult > 0);


	// shutdown the connection since we're done
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		//WSACleanup();
	}

	// cleanup
	closesocket(ConnectSocket);
	//WSACleanup();

	std::cout << "Server disconnected, shutting down mining\n";
	isServerRunning = false;
	return 0;

}

SOCKET ConnectToServer(char* address) {
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return INVALID_SOCKET;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(address, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return INVALID_SOCKET;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return INVALID_SOCKET;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return INVALID_SOCKET;
	}
	isServerRunning = true;
	return ConnectSocket;

}


int pos(char *s, char *c, int n)
{
	int i, j; // Счетчики для циклов
	int lenC, lenS; // Длины строк

	//Находим размеры строки исходника и искомого
	for (lenC = 0; c[lenC]; lenC++);
	for (lenS = 0; s[lenS]; lenS++);

	for (i = 0; i <= lenS - lenC; i++) // Пока есть возможность поиска
	{
		for (j = 0; s[i + j] == c[j]; j++); // Проверяем совпадение посимвольно
		// Если посимвольно совпадает по длине искомого
		// Вернем из функции номер ячейки, откуда начинается совпадение
		// Учитывать 0-терминатор  ( '\0' )
		if (j - lenC == 1 && i == lenS - lenC && !(n - 1)) return i;
		if (j == lenC)
			if (n - 1) n--;
			else return i;
	}
	//Иначе вернем -1 как результат отсутствия подстроки
	return -1;
}


int parsing(std::string buf) {
	int lenComplexity = 11;
	int compexity;

	size_t startCompexity = buf.find("Complexity:");
	size_t endComplexity = buf.find(";");

	complexity = std::stoi(buf.substr(startCompexity + lenComplexity, endComplexity - startCompexity - lenComplexity));
	return 0;
}


int initialDataExchange(SOCKET socket) {
	double speed;
	int iSendResult;
	int iResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	speed = getHashSpeed();

	std::string s = std::to_string(speed) + ";";
	char const *sendbuf = s.c_str();

	std::cout << "Sending out data: " << sendbuf << std::endl;

	iSendResult = send(socket, sendbuf, (int)strlen(sendbuf), 0);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(socket);
	}

	iResult = recv(socket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		std::cout << recvbuf;
		parsing(string(recvbuf));
	}
	else if (iResult == 0)
		printf("Connection closing...\n");
	else {
		printf("recv failed with error: %d\n", WSAGetLastError());
		closesocket(socket);
	}
	return 1;

}

int main(int argc, char **argv) {

	if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}

	std::cout << "Connecting to server..." << endl;

	SOCKET ConnectSocket = ConnectToServer(argv[1]);

	DWORD dwThreadId;
	initialDataExchange(ConnectSocket);
	CreateThread(NULL, 0, SessionWithServer, (LPVOID)ConnectSocket, 0, &dwThreadId);

	// Here goes mining and other stuff


	srand(time(NULL));
	Block *first, *curr;
	first = &buildBlock();

	curr = &createBlock(*first, complexity);
		
	while (isServerRunning) {
		curr = &createBlock(*curr, complexity);
		cout << "Something happened" << endl;
	}
}