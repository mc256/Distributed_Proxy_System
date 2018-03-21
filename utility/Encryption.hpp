//
// Created by mc on 2/23/18.
//

#ifndef PRPR_ENCRYPTION_HPP
#define PRPR_ENCRYPTION_HPP

#include "../main.hpp"

class Encryption {

public:
    static string sha_hash(string input);
    static bool chance(int p);
    static int get_random(int mean, int variance);
};


#endif //PRPR_ENCRYPTION_HPP
