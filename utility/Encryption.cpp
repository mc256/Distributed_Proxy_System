//
// Created by mc on 2/23/18.
//

#include "Encryption.hpp"

// Method
void Encryption::encrypt(char *buffer, size_t buffer_size) {
    AES_cfb128_encrypt(
            (unsigned char *) buffer,(unsigned char *) buffer,
            buffer_size,
            &aes_enc_key,
            (unsigned char *)encrypt_vector,
            &num_enc,
            AES_ENCRYPT
    );
}

void Encryption::decrypt(char *buffer, size_t buffer_size) {
    AES_cfb128_encrypt(
            (unsigned char *) buffer,(unsigned char *) buffer,
            buffer_size,
            &aes_dec_key,
            (unsigned char *)decrypt_vector,
            &num_dec,
            AES_DECRYPT
    );

}

// Constructor
Encryption::Encryption(string enc_key, string dec_key) {
    //key size must be greater than AES_BLOCK_SIZE
    AES_set_encrypt_key((unsigned char *) enc_key.c_str(), 256, &aes_enc_key);
    AES_set_encrypt_key((unsigned char *) dec_key.c_str(), 256, &aes_dec_key);
    num_enc = 0;
    num_dec = 0;

    string init_vector = "IdolMasterMillionLiveStage.";
    encrypt_vector = strdup(init_vector.c_str());
    decrypt_vector = strdup(init_vector.c_str());
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