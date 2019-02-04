#include "Main.h"
using namespace std;

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

int main() {

	srand(time(NULL));
	Block *first, *curr;
	int cp = getComputationPower(1, 2);
	first = &buildBlock();

	curr = &createBlock(*first, cp);
	
	while (true) {
		curr = &createBlock(*curr, cp);
		cout << "Something happened" << endl;
	}
}