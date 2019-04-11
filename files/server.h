#pragma once

#include "Block.h"
#include "signature.h"
#include "external_headers.h"

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "43012"
#define TIME 10

using namespace CryptoPP;

std::vector <Block> block_chain;
std::map <SOCKET, double> miners;
std::map <unsigned int, std::string> transactions_un;
std::map <SOCKET, std::string> users;
std::map <std::string, std::string> log_pas;
std::map <std::string, DSA::PrivateKey> log_dsa;

DWORD WINAPI ClientSession(LPVOID data);
SOCKET SetUpListeningSocket();



void sendComputationPower();

bool checkBlock(Block block);
bool transactions(std::string transaction);

void sendLastBlockToAll();
void sendLastBlock(SOCKET ClientSocket);
void sendDataToAll(int num);
void sendAllData(SOCKET ClientSession);
void miner(SOCKET ClientSocket);

void user(SOCKET ClientSocket);
bool user_register(std::string str, SOCKET Client);

void start_()