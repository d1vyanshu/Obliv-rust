#ifndef FSS_SERVER_H
#define FSS_SERVER_H

#include<cmath>
#include "utils.h"
#include "fss-client.h"

class FssServer {
    public:
        FssServer(FssClient* fclient);

        mpz_class evaluate_equal(PseudoRandomGenerator* prg, KeyEqual* k, uint64_t x);
        
        mpz_class prime;
        uint32_t domain_bits;
        uint32_t num_parties;
};

#endif