//
// Created by mc on 2/23/18.
//

#ifndef PRPR_ENCRYPTION_HPP
#define PRPR_ENCRYPTION_HPP

#include "../main.hpp"

class Encryption {
private:
public:
    AES_KEY aes_enc_key;
    AES_KEY aes_dec_key;
    int num_enc;
    int num_dec;
    char * encrypt_vector;
    char * decrypt_vector;

    void encrypt(char * buffer, size_t size);
    void decrypt(char * buffer, size_t size);

    Encryption(string enc_key, string dec_key);

    static string sha_hash(string input);
    static bool chance(int p);
    static int get_random(int mean, int variance);
    static long long get_current_time(int error);
};


#endif //PRPR_ENCRYPTION_HPP
