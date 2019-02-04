#pragma once
struct Block {
	int number;
	int nonce;
	int computationPower;
	string prevHash;
	string data;
	string hash;
	time_t time;
	Block *prev;
}; 
Block buildBlock();

std::ostream &operator<<(std::ostream& stream, const Block &block);
