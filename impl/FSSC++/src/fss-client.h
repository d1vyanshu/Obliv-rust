#ifndef FSS_CLIENT_H
#define FSS_CLIENT_H

#include "prg.h"
#include<gmp.h>
#include<gmpxx.h>
#include<cmath>
#include "utils.h"
// #include<iostream>
// using namespace std;


class FssClient {
    public:
        FssClient(uint32_t num_bits, uint32_t numparties);

        void generate_tree_equal(PseudoRandomGenerator* prg, KeyEqual* k0, 
        KeyEqual* k1, uint64_t input, uint64_t output);

        void generate_tree_lessthan(PseudoRandomGenerator* prg, KeyLessThan* k0,
         KeyLessThan* k1, uint64_t input, uint64_t output);

        void generate_tree_equal_Mparty(PseudoRandomGenerator* prg, uint64_t a,
        uint64_t b, MultiPartyKey* keys);

        mpz_class prime;
        uint32_t domain_bits;
        uint32_t num_parties;

};

#endif