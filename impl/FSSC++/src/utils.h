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

struct CorrWordLessThan {
    unsigned char cs[2][16];
    unsigned char ct[2];
    uint64_t cv[2];
};

struct KeyLessThan {
    unsigned char s[2][16];
    unsigned char t[2];
    uint64_t v[2];
    CorrWordLessThan* cw[2];
};

struct MultiPartyKey {
    unsigned char** sigma;
    uint32_t** cw;
};

// Assumes integers are 64 bits
inline int get_bit(uint64_t n, uint64_t pos) {
    return (n & ( 1 << (64-pos))) >> (64-pos);
}


inline uint64_t byteArr2Int64(unsigned char* arr) {
    uint64_t i = ((unsigned long) arr[7] << 56) | ((unsigned long)arr[6] << 48) | ((unsigned long)arr[5] << 40) |
                ((unsigned long) arr[4] << 32) | ((unsigned long) arr[3] << 24) |
                ((unsigned long) arr[2] << 16) | ((unsigned long) arr[1] << 8) | ((unsigned long) arr[0]);
    return i;
}

inline uint32_t byteArr2Int32(unsigned char* arr) {
    uint32_t a = uint32_t((unsigned char)(arr[0]) << 24 |
            (unsigned char)(arr[1]) << 16 |
            (unsigned char)(arr[2]) << 8 |
            (unsigned char)(arr[3]));
    return a;
}

#endif