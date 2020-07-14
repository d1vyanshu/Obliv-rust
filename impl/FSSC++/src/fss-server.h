#ifndef FSS_SERVER_H
#define FSS_SERVER_H

#include<cmath>
#include "utils.h"
#include "fss-client.h"
// #include<iostream>

// using namespace std;

class FssServer {
    public:
        FssServer(FssClient* fclient);

        mpz_class evaluate_equal(PseudoRandomGenerator* prg, KeyEqual* k, uint64_t x);
        uint64_t evaluate_lessthan(PseudoRandomGenerator* prg, KeyLessThan* k, uint64_t x);
        uint32_t evaluate_equal_Mparty(PseudoRandomGenerator* prg, MultiPartyKey* key, 
        uint32_t x);
        uint64_t evaluate_test_tree(PseudoRandomGenerator* prg, int party, KeyTest* k, uint64_t x);
        mpz_class prime;
        uint32_t domain_bits;
        uint32_t num_parties;
};

#endif