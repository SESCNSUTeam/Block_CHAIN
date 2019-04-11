#include "signature.h"

std::string string_to_hex(const std::string& input)
{
	static const char* const lut = "0123456789ABCDEF";
	size_t len = input.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i)
	{
		const unsigned char c = input[i];
		output.push_back(lut[c >> 4]);
		output.push_back(lut[c & 15]);
	}
	return output;
}

std::string SHA256Hash(std::string input) {
	byte digest[SHA256::DIGESTSIZE];
	SHA256 hash;
	hash.CalculateDigest(digest, (const byte*)input.c_str(), input.length());
	HexEncoder encoder;
	std::string output;
	encoder.Attach(new StringSink(output));
	encoder.Put(digest, sizeof(digest));
	encoder.MessageEnd();
	return output;
}

std::string cut_s(std::string str) {
	std::string result = "";
	int len = (int)strlen(str.c_str());
	for (int i = 0; i < len; ++i) {
		if (str[i] == ';') {
			result = str[i + 1, len];
			break;
		}
	}
	return result;
}

std::string cut(std::string str) {
	int len = (int)strlen(str.c_str());
	std::string result = "";
	for (int i = 0; i < len; ++i) {
		if (str[i] == ';') break;
		result += str[i];
	}
	return result;
}


std::string create_digital_signature(std::string transaction, std::string username, std::string password, std::map <std::string, std::string> &log_pas, std::map <std::string, CryptoPP::DSA::PrivateKey> &log_dsa, CryptoPP::AutoSeededRandomPool &rng) 
{
	if (log_pas.count(username) == 0) {
		std::cout << "Failed to verify transaction because user with name " + username + " doesn't exist!\n";
		return "";
	}
	if (log_pas[username] != password) {
		std::cout << "Failed to verify transaction because user " + username + " entered wrong password!\n";
		return "";
	}

	CryptoPP::DSA::Signer signer(log_dsa[username]);


	std::string signature;
	CryptoPP::StringSource ss1(transaction, true, new CryptoPP::SignerFilter(rng, signer, new CryptoPP::StringSink(signature))); // StringSource
	std::cout << "Signature of the user " << username << ": " << string_to_hex(signature) << std::endl;
	return string_to_hex(signature);
}