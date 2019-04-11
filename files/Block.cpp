#include "external_headers.h"
#include "Block.h"


std::string convertBlockToString(Block block) {
	std::string s;
	s = std::to_string(block.number) + "|" + std::to_string(block.nonce) + "|" + std::to_string(block.computationPower) + "|" + block.prevHash + "|" + block.data + "|" + block.hash + "|" + std::to_string(block.time);
	return s;
}

Block convertStringToBlock(std::string hexBlock) {
	Block block;
	std::string s;
	int pos;
	int lastpos = hexBlock.find(";");
	s = hexBlock.substr(0, lastpos);
	
	pos = hexBlock.find("|");
	s = hexBlock.substr(0, pos);
	block.number = std::stoi(s);
	hexBlock = hexBlock.substr(pos + 1);

	pos = hexBlock.find("|");
	s = hexBlock.substr(0, pos);
	block.nonce = std::stoi(s);
	hexBlock = hexBlock.substr(pos + 1);

	pos = hexBlock.find("|");
	s = hexBlock.substr(0, pos);
	block.computationPower= std::stoi(s);
	hexBlock = hexBlock.substr(pos + 1);

	pos = hexBlock.find("|");
	s = hexBlock.substr(0, pos);
	block.prevHash = s;
	hexBlock = hexBlock.substr(pos + 1);

	pos = hexBlock.find("|");
	s = hexBlock.substr(0, pos);
	block.data = s;
	hexBlock = hexBlock.substr(pos + 1);

	pos = hexBlock.find("|");
	s = hexBlock.substr(0, pos);
	block.hash = s;
	hexBlock = hexBlock.substr(pos + 1);

	pos = hexBlock.find("|");
	s = hexBlock.substr(0, pos);
	block.time = (time_t)std::stoi(s);
	hexBlock = hexBlock.substr(pos + 1);

	return block;
}

Block buildBlock() {
	Block b;
	b.number = 0;
	b.nonce = 0;
	b.computationPower = 0;
	b.time = 0;
	b.data = "None";
	b.hash = "None";
	b.prevHash = "None";
	return b;
}