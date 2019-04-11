#pragma once
#include "external_headers.h"
using namespace CryptoPP;

std::string cut(std::string str);
std::string cut_s(std::string str);
std::string SHA256Hash(std::string input);
std::string string_to_hex(const std::string& input);

std::string create_digital_signature(std::string transaction, std::string username, std::string password, std::map <std::string, std::string> &log_pas, std::map <std::string, DSA::PrivateKey> &log_dsa, AutoSeededRandomPool &rng);