#ifndef FSS_CLIENT_H
#define FSS_CLIENT_H

#include "prg.h"
#include<gmp.h>
#include<gmpxx.h>
#include "utils.h"


class FssClient {
    public:
        FssClient(uint32_t num_bits, uint32_t numparties);

        void generate_tree_equal(PseudoRandomGenerator* prg, KeyEqual* k0, 
        KeyEqual* k1, uint64_t input, uint64_t output);

        mpz_class prime;
        uint32_t domain_bits;
        uint32_t num_parties;

};

#endif