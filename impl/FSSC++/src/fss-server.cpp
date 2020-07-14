#include "fss-server.h"

FssServer::FssServer(FssClient *fclient) {
    domain_bits = fclient->domain_bits;
    num_parties = fclient->num_parties;
}

uint64_t FssServer::evaluate_equal(PseudoRandomGenerator*prg, int party, KeyEqual* k, uint64_t x) {
    bool t;
    if(party) t=true;
    else t=false;

    unsigned char s[16];
    memcpy(s, k->s, 16);
    for(int i=0; i<domain_bits; i++) {
        unsigned char out[48];
        prg->generate_random_number(out, s, 48);

        int a = get_bit(x, (64-domain_bits+i+1));
        if(t) {
            unsigned char tempxor[34];
            memcpy(tempxor, k->cw[i].cs, 16);
            memcpy((unsigned char*) (tempxor+17), k->cw[i].cs, 16);
            tempxor[16] = k->cw[i].tl;
            tempxor[33] = k->cw[i].tr;
            for(int j=0; j<34; j++) {
                out[j] = out[j] ^ tempxor[j];
            }
        }
        if(a) {
            memcpy(s, (unsigned char*)(out+17), 16);
            t = out[33]%2;
        }
        else {
            memcpy(s, out, 16);
            t = out[16]%2;
        }
    }

    uint64_t convs = convert(s, domain_bits);
    uint64_t ans;
    if(t) {
        if(party) {
            ans = ((1<<domain_bits) - convs)%(1<<domain_bits);
            ans = (ans + (1<<domain_bits) - k->w)%(1<<domain_bits);
        }
        else 
            ans = (convs + k->w)%(1<<domain_bits);
    }
    else {
        if(party) 
            ans = ((1<<domain_bits) - convs)%(1<<domain_bits);
        else
            ans = convs%(1<<domain_bits);
    }
    return ans;
}

uint64_t FssServer::evaluate_lessthan(PseudoRandomGenerator* prg, KeyLessThan* k, uint64_t x) {
    int xi = get_bit(x, (64-domain_bits+1));
    unsigned char s[16];
    memcpy(s, k->s[xi], 16);
    unsigned char t = k->t[xi];
    uint64_t v = k->v[xi];

     unsigned char s_bits0and1[32];
    unsigned char randombits[2];
    unsigned char out[64];
    uint64_t temp_v;

    for (uint32_t i = 1; i < domain_bits; i++) {
        if(i!=domain_bits) {
            xi = get_bit(x, (64-domain_bits+i+1));
        } else {
            xi = 0;
        }
        prg->generate_random_number(out, s, 64);
        memcpy(s_bits0and1, out, 32);
        randombits[0] = out[32] % 2;
        randombits[1] = out[33] % 2;

        temp_v = byteArr2Int64((unsigned char*) (out + 40 + (8*xi)));
        int x_start = 16 * xi;
        memcpy(s, (unsigned char*) (s_bits0and1 + x_start), 16);
        for (uint32_t j = 0; j < 16; j++) {
            s[j] = s[j] ^ k->cw[t][i-1].cs[xi][j];
        }

        v = (v + temp_v);
        v = (v + k->cw[t][i-1].cv[xi]);
        t = randombits[xi] ^ k->cw[t][i-1].ct[xi];
    }

    return v;
}

uint32_t FssServer::evaluate_equal_Mparty(PseudoRandomGenerator* prg, MultiPartyKey* key, uint32_t x) {
    uint32_t m = 4; // Assume messages are 4 bytes long 
    uint64_t n = domain_bits;
    uint32_t p = num_parties;
    uint32_t p2 = (uint32_t)(pow(2, p-1));
    uint64_t mu = (uint64_t)ceil((pow(2, n/2.0) * pow(2,(p-1)/2.0)));

    // sigma is last n/2 bits
    uint32_t delta = x & ((1 << (n/2)) - 1);
    uint32_t gamma = (x & (((1 << (n+1)/2) - 1) << n/2)) >> n/2;

    unsigned char** sigma = key->sigma;
    uint32_t** cw = key->cw;
    uint32_t m_bytes = m*mu;

    uint32_t* y = (uint32_t*) malloc(m_bytes);
    unsigned char* temp_out = (unsigned char*) malloc(m_bytes);
    memset(y, 0, m_bytes);
    prg->num_keys = mu;
    for (int i = 0; i < p2; i++) {
        unsigned char* s = (unsigned char*)sigma[gamma] + i*16;
        bool all_zero_bytes = true;
        for (int j = 0; j < 16; j++) {
            if (s[j] != 0) {
                all_zero_bytes = false;
                break;
            }
        }
        if (!all_zero_bytes) {
            prg->generate_random_number(temp_out, s, m_bytes);
            for (int k = 0; k < mu; k++) {
                unsigned char tempIntBytes[4];
                memcpy(tempIntBytes, &temp_out[4*k], 4);
                y[k] = y[k] ^ byteArr2Int32(tempIntBytes);
            }

            for (int j = 0; j < mu; j++) {
                y[j] = cw[i][j] ^ y[j];
            }
        }
    }

    uint32_t final_ans = y[delta];
    free(y);
    free(temp_out);
    return final_ans;
}
