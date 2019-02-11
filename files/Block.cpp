#include "Main.h"

std::string convertBlock(Block block) {


	return NULL;
}


std::ostream &operator<<(std::ostream& stream, const Block &block) {
	stream << block.time << std::endl;
	stream << block.number << " " << block.computationPower << std::endl;
	stream << block.prevHash << std::endl << block.data << std::endl;
	stream << block.nonce << " " << block.hash << std::endl;
	return stream;
};

Block buildBlock() {
	Block b;
	b.number = 0;
	b.nonce = 0;
	b.computationPower = 0;
	b.time = 0;
	b.data = "";
	b.hash = "";
	b.prevHash = "";
	b.prev = NULL;
	return b;
}