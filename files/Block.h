#pragma once
#include "Main.h"
struct Block {
	int number;
	int nonce;
	int computationPower;
	std::string prevHash;
	std::string data;
	std::string hash;
	time_t time;
	Block *prev;
}; 
Block buildBlock();

std::ostream &operator<<(std::ostream& stream, const Block &block);
