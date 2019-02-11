#include "Main.h"
using namespace CryptoPP;
using namespace std;

string SHA256Hash(string input) {
	byte digest[SHA256::DIGESTSIZE];
	SHA256 hash;
	hash.CalculateDigest(digest, (const byte*)input.c_str(), input.length());
	HexEncoder encoder;
	string output;
	encoder.Attach(new StringSink(output));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();
	return output;
}

string binHash(string hexHash) {

	map <char, string> hextobin;
	hextobin['0'] = "0000";
	hextobin['1'] = "0001";
	hextobin['2'] = "0010";
	hextobin['3'] = "0011";
	hextobin['4'] = "0100";
	hextobin['5'] = "0101";
	hextobin['6'] = "0110";
	hextobin['7'] = "0111";
	hextobin['8'] = "1000";
	hextobin['9'] = "1001";
	hextobin['a'] = "1010";
	hextobin['b'] = "1011";
	hextobin['c'] = "1100";
	hextobin['d'] = "1101";
	hextobin['e'] = "1110";
	hextobin['f'] = "1111";

	string binhash;

	for (char ch : hexHash) {
		binhash += hextobin[ch];
	}
	//cout << binhash << endl;
	return binhash;
}

string openFile(string dir) {
	string str = "";
	ifstream fin(dir);
	string s;
	while (fin >> s) {
		str += s;
	}
	return str;
}