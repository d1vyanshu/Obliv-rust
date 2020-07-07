#include "fss-server.h"

FssServer::FssServer(FssClient *fclient) {
    domain_bits = fclient->domain_bits;
    num_parties = fclient->num_parties;
    prime = fclient->prime;
}

mpz_class FssServer::evaluate_equal(PseudoRandomGenerator* prg, KeyEqual* k, uint64_t x) {
    int xi = get_bit(x, (64-domain_bits+1));
    unsigned char s[16];
    memcpy(s, k->s[xi], 16);
    unsigned char t = k->t[xi];

    unsigned char s_bits0and1[32];
    unsigned char randombits[2];
    unsigned char out[48];

    for(uint32_t i=1; i<domain_bits+1; i++) {
        if(i!=domain_bits) {
            xi = get_bit(x, (64-domain_bits+i+1));
        }
        else xi = 0;

        prg->generate_random_number(out, s, 48);
        memcpy(s_bits0and1, out, 32);
        randombits[0] = out[32]%2;
        randombits[1] = out[33]%2;

        if(i==domain_bits) break;

        int x_start = 16*xi;
        memcpy(s, (unsigned char*)(s_bits0and1 + x_start), 16);

        for(uint32_t j=0; j<16; j++)
            s[j] = s[j] ^ k->cw[t][i-1].cs[xi][j];
        
        t = randombits[xi] ^ k->cw[t][i-1].ct[xi];


    }

    mpz_class ans;
    unsigned char last_s_out[34];

    memcpy(last_s_out, s_bits0and1, 32);
    last_s_out[32] = randombits[0];
    last_s_out[33] = randombits[1];

    mpz_import(ans.get_mpz_t(), 34, 1, sizeof(last_s_out[0]), 0, 0, last_s_out);
    ans = ans * k->w;
    ans = ans%prime;
    return ans;

}