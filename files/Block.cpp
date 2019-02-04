#include "Main.h"

using namespace std;

ostream &operator<<(ostream& stream, const Block &block) {
	stream << block.time << endl;
	stream << block.number << " " << block.computationPower << endl;
	stream << block.prevHash << endl << block.data << endl;
	stream << block.nonce << " " << block.hash << endl;
	return stream;
}
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