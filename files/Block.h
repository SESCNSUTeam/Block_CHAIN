#pragma once
#include "external_headers.h"

struct Block {
	int number;
	int nonce;
	int computationPower;
	std::string prevHash;
	std::string data;
	std::string hash;
	time_t time;
}; 

Block buildBlock();
Block convertStringToBlock(std::string hexBlock);

std::ostream &operator<<(std::ostream& stream, const Block &block);
std::string convertBlockToString(Block block);
