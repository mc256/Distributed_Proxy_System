//
// Created by mc on 2/23/18.
//

#include "Encryption.hpp"


string Encryption::sha_hash(string input) {
    vector<unsigned char> plain = vector<unsigned char>(input.begin(), input.end());
    vector<unsigned char> hash = vector<unsigned char>(32);

    SHA256(plain.data(), plain.size(), hash.data());

    stringstream hash_hex;
    for (size_t i = 0; i < hash.size(); i++) {
        hash_hex << hex << +hash.at(i);
    }
    return hash_hex.str();
}