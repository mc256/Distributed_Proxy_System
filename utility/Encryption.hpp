//
// Created by mc on 2/23/18.
//

#ifndef PRPR_ENCRYPTION_HPP
#define PRPR_ENCRYPTION_HPP

#include "../main.hpp"

class Encryption {
private:
    AES_KEY aes_enc_key, aes_dec_key;
    int num_enc, num_dec;
    vector<unsigned char> encrypt_vector, decrypt_vector;
public:
    void encrypt(char * buffer, size_t size);
    void decrypt(char * buffer, size_t size);

    Encryption(string keyEncrypt, string keyDecrypt);

    static string sha_hash(string input);
    static bool chance(int p);
    static int get_random(int mean, int variance);
    static long long get_current_time(int error);
};


#endif //PRPR_ENCRYPTION_HPP
