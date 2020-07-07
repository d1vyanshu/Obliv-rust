#ifndef UTILS_H
#define UTILS_H

#include<gmp.h>
#include<gmpxx.h>

struct CorrWordEquality {
    unsigned char cs[2][16];
    unsigned char ct[2];
};

struct KeyEqual {
    unsigned char s[2][16];
    unsigned char t[2];
    CorrWordEquality* cw[2];
    mpz_class w;
};

// Assumes integers are 64 bits
inline int get_bit(uint64_t n, uint64_t pos) {
    return (n & ( 1 << (64-pos))) >> (64-pos);
}

#endif