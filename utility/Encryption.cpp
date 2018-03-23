//
// Created by mc on 2/23/18.
//

#include "Encryption.hpp"

// Method
void Encryption::encrypt(char *buffer, size_t size) {
    AES_cfb128_encrypt((unsigned char *)buffer, (unsigned char *)buffer, size, &aes_enc_key, encrypt_vector.data(), &num_enc, AES_ENCRYPT);
}

void Encryption::decrypt(char *buffer, size_t size) {
    AES_cfb128_encrypt((unsigned char *)buffer, (unsigned char *)buffer, size, &aes_dec_key, decrypt_vector.data(), &num_dec, AES_DECRYPT);
}

// Constructor
Encryption::Encryption(string keyEncrypt, string keyDecrypt) {
    AES_set_encrypt_key((unsigned char *) keyEncrypt.c_str(), 256, &aes_enc_key);
    AES_set_decrypt_key((unsigned char *) keyDecrypt.c_str(), 256, &aes_dec_key);
    num_dec = num_enc = 0;

    string init_vector = "IdolMasterMillionLiveStage.";
    encrypt_vector = vector<unsigned char>(init_vector.begin(),init_vector.end());
    decrypt_vector = vector<unsigned char>(init_vector.begin(),init_vector.end());
}

// Static
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

bool Encryption::chance(int p) {
    return ((rand() % 100) <= p);
}

int Encryption::get_random(int mean, int variance) {
    return (mean + rand() % (2 * variance) - variance);
}

long long Encryption::get_current_time(int error){
    return time(NULL) / (error <= 0 ? 1: error);
}