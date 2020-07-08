#include "fss-client.h"


FssClient::FssClient(uint32_t num_bits, uint32_t numparties) {
    domain_bits = num_bits;
    num_parties = numparties;

    mpz_class power2;
    mpz_ui_pow_ui(power2.get_mpz_t(), 2, 32);
    mpz_nextprime(prime.get_mpz_t(), power2.get_mpz_t());

}


void FssClient::generate_tree_equal(PseudoRandomGenerator* prg, KeyEqual* k0, KeyEqual* k1, uint64_t input, uint64_t output) {
    k0->cw[0] = (CorrWordEquality*) malloc(sizeof(CorrWordEquality) * (domain_bits-1));
    k0->cw[1] = (CorrWordEquality*) malloc(sizeof(CorrWordEquality) * (domain_bits-1));
    k1->cw[0] = (CorrWordEquality*) malloc(sizeof(CorrWordEquality) * (domain_bits-1));
    k1->cw[1] = (CorrWordEquality*) malloc(sizeof(CorrWordEquality) * (domain_bits-1));

    int a = get_bit(input, (64-domain_bits+1));
    int not_a = a ^ 1;

    unsigned char s_party0[32];
    unsigned char s_party1[32];
    int a_start = 16 * a;
    int not_a_start = 16 *not_a;          //Used to distinguish the two seeds stored in 32 bits

    //Setting initial seeds for PRF
    if(!RAND_bytes((unsigned char*) (s_party0 + a_start), 16)) {
        printf("Random bytes failed\n");
        exit(1);
    }
    if (!RAND_bytes((unsigned char*) (s_party1 + a_start), 16)) {
        printf("Random bytes failed\n");
        exit(1);
    }
    if (!RAND_bytes((unsigned char*) (s_party0 + not_a_start), 16)) {
        printf("Random bytes failed\n");
        exit(1);
    }
    memcpy((unsigned char*)(s_party1 + not_a_start), (unsigned char*)(s_party0 + not_a_start), 16);

    unsigned char t0[2];
    unsigned char t1[2];

    unsigned char randombits[2];

    if (!RAND_bytes((unsigned char*) randombits, 2)) {
        printf("Random bytes failed\n");
        exit(1);
    }

    // Figure out initial ts
    // Make sure t0a and t1a are different
    t0[a] = randombits[0] % 2;
    t1[a] = (t0[a] + 1) % 2;

    // Make sure t0na = t1na
    t0[not_a] = randombits[1] % 2;
    t1[not_a] = t0[not_a];

    memcpy(k0->s[0], s_party0, 16);
    memcpy(k0->s[1], (unsigned char*)(s_party0 + 16), 16);
    memcpy(k1->s[0], s_party1, 16);
    memcpy(k1->s[1], (unsigned char*) (s_party1 + 16), 16);
    k0->t[0] = t0[0];
    k0->t[1] = t0[1];
    k1->t[0] = t1[0];
    k1->t[1] = t1[1];

    //Picking right keys to generate the rest of the tree
    unsigned char key0[16];
    unsigned char key1[16];
    memcpy(key0, (unsigned char*) (s_party0 + a_start), 16);
    memcpy(key1, (unsigned char*) (s_party1 + a_start), 16);

    unsigned char tbit0 = t0[a];
    unsigned char tbit1 = t1[a];

    unsigned char cs0[32];
    unsigned char cs1[32];
    unsigned char ct0[2];
    unsigned char ct1[2];
    unsigned char out0[48];
    unsigned char out1[48];

    for (uint32_t i = 0; i < domain_bits; i++) {
        prg->generate_random_number(out0, key0, 48);
        prg->generate_random_number(out1, key1, 48);

        memcpy(s_party0, out0, 32);
        memcpy(s_party1, out1, 32);
        t0[0] = out0[32] % 2;
        t0[1] = out0[33] % 2;
        t1[0] = out1[32] % 2;
        t1[1] = out1[33] % 2;


        //The last bit will be handled differently.
        if (i == domain_bits-1) {
            break;
        }

        // Reset a and not_a bits
        a = get_bit(input, (64-domain_bits+i+2));
        not_a = a ^ 1;

        // Redefine aStart and naStart based on new a's
        a_start = 16 * a;
        not_a_start = 16 * not_a;

        // Create cs and ct for next bit
        if (!RAND_bytes((unsigned char*) (cs0 + a_start), 16)) {
            printf("Random bytes failed.\n");
            exit(1);
        }
        if (!RAND_bytes((unsigned char*) (cs1 + a_start), 16)) {
            printf("Random bytes failed.\n");
            exit(1);
        }
        if (!RAND_bytes((unsigned char*) (cs0 + not_a_start), 16)) {
            printf("Random bytes failed.\n");
            exit(1);
        }

        for (uint32_t j = 0; j < 16; j++) {
            cs1[not_a_start+j] = s_party0[not_a_start+j] ^ s_party1[not_a_start+j] ^ cs0[not_a_start+j];
        }

        if (!RAND_bytes(randombits, 2)) {
            printf("Random bytes failed.\n");
            exit(1);
        }
        ct0[a] = randombits[0] % 2;
        ct1[a] = ct0[a] ^ t0[a] ^ t1[a] ^ 1;

        ct0[not_a] = randombits[1] % 2;
        ct1[not_a] = ct0[not_a] ^ t0[not_a] ^ t1[not_a];

        // Copy appropriate values into key
        memcpy(k0->cw[0][i].cs[0], cs0, 16);
        memcpy(k0->cw[0][i].cs[1], (unsigned char*) (cs0 + 16), 16);
        k0->cw[0][i].ct[0] = ct0[0];
        k0->cw[0][i].ct[1] = ct0[1];
        memcpy(k0->cw[1][i].cs[0], cs1, 16);
        memcpy(k0->cw[1][i].cs[1], (unsigned char*) (cs1 + 16), 16);
        k0->cw[1][i].ct[0] = ct1[0];
        k0->cw[1][i].ct[1] = ct1[1];

        memcpy(k1->cw[0][i].cs[0], cs0, 16);
        memcpy(k1->cw[0][i].cs[1], (unsigned char*) (cs0 + 16), 16);
        k1->cw[0][i].ct[0] = ct0[0];
        k1->cw[0][i].ct[1] = ct0[1];
        memcpy(k1->cw[1][i].cs[0], cs1, 16);
        memcpy(k1->cw[1][i].cs[1], (unsigned char*) (cs1 + 16), 16);
        k1->cw[1][i].ct[0] = ct1[0];
        k1->cw[1][i].ct[1] = ct1[1];

        unsigned char* cs;
        unsigned char* ct;

        if (tbit0 == 1) {
            cs = cs1;
            ct = ct1;
        } else {
            cs = cs0;
            ct = ct0;
        }
        for (uint32_t j = 0; j < 16; j++) {
            key0[j] = s_party0[a_start+j] ^ cs[a_start+j];
        }
        tbit0 = t0[a] ^ ct[a];

        if (tbit1 == 1) {
            cs = cs1;
            ct = ct1;
        } else {
            cs = cs0;
            ct = ct0;
        }
        for (uint32_t j = 0; j < 16; j++) {
            key1[j] = s_party1[a_start+j] ^ cs[a_start+j];
        }
        tbit1 = t1[a] ^ ct[a];
    }

    //Set w in the key
    unsigned char intArray0[34];
    unsigned char intArray1[34];

    memcpy(intArray0, s_party0, 32);
    intArray0[32] = t0[0];
    intArray0[33] = t0[1];
    memcpy(intArray1, s_party1, 32);
    intArray1[32] = t1[0];
    intArray1[33] = t1[1];

    mpz_class s_int0, s_int1;

    mpz_import(s_int0.get_mpz_t(), 34, 1, sizeof(intArray0[0]), 0, 0, intArray0);
    mpz_import(s_int1.get_mpz_t(), 34, 1, sizeof(intArray1[0]), 0, 0, intArray1);

    if(s_int0 != s_int1) {
        mpz_class diff = s_int0 - s_int1;
        mpz_invert(diff.get_mpz_t(), diff.get_mpz_t(), prime.get_mpz_t());
        mpz_class b;
        mpz_import(b.get_mpz_t(), 1, -1, sizeof (uint64_t), 0, 0, &output);
        k0->w = (diff * b) %prime;
        k1->w = k0->w;
    }
    else {
        k0->w = 0;
        k1->w = 0;
    }

}

void FssClient::generate_tree_lessthan(PseudoRandomGenerator* prg, KeyLessThan* k0, KeyLessThan* k1, uint64_t input, uint64_t output) {
    k0->cw[0] = (CorrWordLessThan*) malloc(sizeof(CorrWordLessThan) * (domain_bits-1));
    k0->cw[1] = (CorrWordLessThan*) malloc(sizeof(CorrWordLessThan) * (domain_bits-1));
    k1->cw[0] = (CorrWordLessThan*) malloc(sizeof(CorrWordLessThan) * (domain_bits-1));
    k1->cw[1] = (CorrWordLessThan*) malloc(sizeof(CorrWordLessThan) * (domain_bits-1));

    int a = get_bit(input, (64-domain_bits+1));
    int not_a = a ^ 1;

    unsigned char s_party0[32];
    unsigned char s_party1[32];
    int a_start = 16 * a;
    int not_a_start = 16 *not_a;          //Used to distinguish the two seeds stored in 32 bits

    //Setting initial seeds for PRF
    if(!RAND_bytes((unsigned char*) (s_party0 + a_start), 16)) {
        printf("Random bytes failed\n");
        exit(1);
    }
    if (!RAND_bytes((unsigned char*) (s_party1 + a_start), 16)) {
        printf("Random bytes failed\n");
        exit(1);
    }
    if (!RAND_bytes((unsigned char*) (s_party0 + not_a_start), 16)) {
        printf("Random bytes failed\n");
        exit(1);
    }
    memcpy((unsigned char*)(s_party1 + not_a_start), (unsigned char*)(s_party0 + not_a_start), 16);

    unsigned char t0[2];
    unsigned char t1[2];

    unsigned char randombits[2];

    if (!RAND_bytes((unsigned char*) randombits, 2)) {
        printf("Random bytes failed\n");
        exit(1);
    }

    // Figure out initial ts
    // Make sure t0a and t1a are different
    t0[a] = randombits[0] % 2;
    t1[a] = (t0[a] + 1) % 2;

    // Make sure t0na = t1na
    t0[not_a] = randombits[1] % 2;
    t1[not_a] = t0[not_a];

    // Set initial v's
    unsigned char temp_v[8];
    if (!RAND_bytes(temp_v, 8)) {
        printf("Random bytes failed.\n");
        exit(1);
    }
    k0->v[a] = byteArr2Int64(temp_v);
    k1->v[a] = k0->v[a];

    if (!RAND_bytes(temp_v, 8)) {
        printf("Random bytes failed.\n");
        exit(1);
    }
    k0->v[not_a] = byteArr2Int64(temp_v);
    k1->v[not_a] = k0->v[not_a] - output*a;

    memcpy(k0->s[0], s_party0, 16);
    memcpy(k0->s[1], (unsigned char*)(s_party0 + 16), 16);
    memcpy(k1->s[0], s_party1, 16);
    memcpy(k1->s[1], (unsigned char*) (s_party1 + 16), 16);
    k0->t[0] = t0[0];
    k0->t[1] = t0[1];
    k1->t[0] = t1[0];
    k1->t[1] = t1[1];

    // Pick right keys to put into cipher
    unsigned char key0[16];
    unsigned char key1[16];
    memcpy(key0, (unsigned char*) (s_party0 + a_start), 16);
    memcpy(key1, (unsigned char*) (s_party1 + a_start), 16);

    unsigned char tbit0 = t0[a];
    unsigned char tbit1 = t1[a];

    unsigned char cs0[32];
    unsigned char cs1[32];
    unsigned char ct0[2];
    unsigned char ct1[2];
    unsigned char out0[64];
    unsigned char out1[64];

    uint64_t v0[2];
    uint64_t v1[2];
    uint64_t cv[2][2];
    uint64_t v;

    for (uint32_t i = 0; i < domain_bits-1; i++) {
        prg->generate_random_number(out0, key0, 64);
        prg->generate_random_number(out1, key1, 64);

        memcpy(s_party0, out0, 32);
        memcpy(s_party1, out1, 32);
        t0[0] = out0[32] % 2;
        t0[1] = out0[33] % 2;
        t1[0] = out1[32] % 2;
        t1[1] = out1[33] % 2;

        v0[0] = byteArr2Int64((unsigned char*) (out0 + 40));
        v0[1] = byteArr2Int64((unsigned char*) (out0 + 48));
        v1[0] = byteArr2Int64((unsigned char*) (out1 + 40));
        v1[1] = byteArr2Int64((unsigned char*) (out1 + 48));


        // Reset a and not_a bits
        a = get_bit(input, (64-domain_bits+i+2));
        not_a = a ^ 1;

        // Redefine a_start and not_a_start based on new a's
        a_start = 16 * a;
        not_a_start = 16 * not_a;

        // Create cs and ct for next bit
        if (!RAND_bytes((unsigned char*) (cs0 + a_start), 16)) {
            printf("Random bytes failed.\n");
            exit(1);
        }
        if (!RAND_bytes((unsigned char*) (cs1 + a_start), 16)) {
            printf("Random bytes failed.\n");
            exit(1);
        }
        if (!RAND_bytes((unsigned char*) (cs0 + not_a_start), 16)) {
            printf("Random bytes failed.\n");
            exit(1);
        }

        for (uint32_t j = 0; j < 16; j++) {
            cs1[not_a_start+j] = s_party0[not_a_start+j] ^ s_party1[not_a_start+j] ^ cs0[not_a_start+j];
        }

        if (!RAND_bytes(randombits, 2)) {
            printf("Random bytes failed.\n");
            exit(1);
        }

        ct0[a] = randombits[0] % 2;
        ct1[a] = ct0[a] ^ t0[a] ^ t1[a] ^ 1;

        ct0[not_a] = randombits[1] % 2;
        ct1[not_a] = ct0[not_a] ^ t0[not_a] ^ t1[not_a];

        if (!RAND_bytes(temp_v, 8)) {
            printf("Random bytes failed.\n");
            exit(1);
        }

        cv[tbit0][a] = byteArr2Int64(temp_v);
        v = (cv[tbit0][a] + v0[a]);


        v = (v - v1[a]);
        cv[tbit1][a] = v;
        if (!RAND_bytes(temp_v, 8)) {
            printf("Random bytes failed.\n");
            exit(1);
        }

        cv[tbit0][not_a] = byteArr2Int64(temp_v);
        v = (cv[tbit0][not_a] + v0[not_a]);

        v = (v - v1[not_a]);
        cv[tbit1][not_a] = (v - output*a);

        // Copy appropriate values into key
        memcpy(k0->cw[0][i].cs[0], cs0, 16);
        memcpy(k0->cw[0][i].cs[1], (unsigned char*) (cs0 + 16), 16);
        k0->cw[0][i].ct[0] = ct0[0];
        k0->cw[0][i].ct[1] = ct0[1];
        memcpy(k0->cw[1][i].cs[0], cs1, 16);
        memcpy(k0->cw[1][i].cs[1], (unsigned char*) (cs1 + 16), 16);
        k0->cw[1][i].ct[0] = ct1[0];
        k0->cw[1][i].ct[1] = ct1[1];

        k0->cw[0][i].cv[0] = cv[0][0];
        k0->cw[0][i].cv[1] = cv[0][1];
        k0->cw[1][i].cv[0] = cv[1][0];
        k0->cw[1][i].cv[1] = cv[1][1];

        memcpy(k1->cw[0][i].cs[0], cs0, 16);
        memcpy(k1->cw[0][i].cs[1], (unsigned char*) (cs0 + 16), 16);
        k1->cw[0][i].ct[0] = ct0[0];
        k1->cw[0][i].ct[1] = ct0[1];
        memcpy(k1->cw[1][i].cs[0], cs1, 16);
        memcpy(k1->cw[1][i].cs[1], (unsigned char*) (cs1 + 16), 16);
        k1->cw[1][i].ct[0] = ct1[0];
        k1->cw[1][i].ct[1] = ct1[1];

        k1->cw[0][i].cv[0] = cv[0][0];
        k1->cw[0][i].cv[1] = cv[0][1];
        k1->cw[1][i].cv[0] = cv[1][0];
        k1->cw[1][i].cv[1] = cv[1][1];

        unsigned char* cs;
        unsigned char* ct;

        if (tbit0 == 1) {
            cs = cs1;
            ct = ct1;
        } else {
            cs = cs0;
            ct = ct0;
        }
        for (uint32_t j = 0; j < 16; j++) {
            key0[j] = s_party0[a_start+j] ^ cs[a_start+j];
        }
        tbit0 = t0[a] ^ ct[a];

        //printf("After XOR: ");
        //printByteArray(key0, 16);
        if (tbit1 == 1) {
            cs = cs1;
            ct = ct1;
        } else {
            cs = cs0;
            ct = ct0;

        }

        for (uint32_t j = 0; j < 16; j++) {
            key1[j] = s_party1[a_start+j] ^ cs[a_start+j];
        }
        tbit1 = t1[a] ^ ct[a];
    }

}


void FssClient::generate_tree_equal_Mparty(PseudoRandomGenerator* prg, uint64_t a, uint64_t b, MultiPartyKey* keys) {
    uint32_t n = domain_bits;
    uint32_t p = num_parties;
    uint32_t m = 4; // Assume messages are 4 bytes long

    // store 2^p-1
    uint32_t p2 = (uint32_t)(pow(2, p-1));
    uint64_t mu = (uint64_t)ceil((pow(2, n/2.0) * pow(2,(p-1)/2.0)));
    uint64_t v = (uint64_t)ceil((pow(2,n))/mu);

    // sigma is last n/2 bits
    uint32_t delta = a & ((1 << (n/2)) - 1);
    uint32_t gamma = (a & (((1 << (n+1)/2) - 1) << n/2)) >> n/2;
    unsigned char*** aArr = (unsigned char***) malloc(sizeof(unsigned char**)*v);
    for (int i = 0; i < v; i++) {
        aArr[i] = (unsigned char**) malloc(sizeof(unsigned char*)*p);
        for (int j = 0; j < p; j++) {
            aArr[i][j] = (unsigned char*) malloc(p2);
        }
    }
    for (int i = 0; i < v; i++) {
        for (int j = 0; j < p; j++) {
            if (j != (p-1)) {
                if(!RAND_bytes((unsigned char*) aArr[i][j], p2)) {
                    printf("Random bytes failed.\n");
                    exit(1);
                }
                for (int k = 0; k < p2; k++) {
                    aArr[i][j][k] = aArr[i][j][k] % 2;
                }
            } else {
                // Set the last row so that the columns have odd or even number
                // of bits
                for (int k = 0; k < p2; k++) {
                    uint32_t curr_bits = 0;
                    for (int l = 0; l < p-1; l++) {
                        curr_bits += aArr[i][l][k];
                    }
                    curr_bits = curr_bits % 2;
                    // If array index is not gamma, just make sure the p's sum up to even
                    if (i != gamma) {
                        if (curr_bits == 0) {
                            aArr[i][j][k] = 0;
                        } else {
                            aArr[i][j][k] = 1;
                        }
                    } else {
                        // Make sure the array at gamma are odd binaries
                        if (curr_bits == 0) {
                            aArr[i][j][k] = 1;
                        } else {
                            aArr[i][j][k] = 0;
                        }
                    }
                }
            }
        }
    }
    unsigned char *** s = (unsigned char***) malloc(sizeof(unsigned char**)*v);
    for (int i = 0; i < v; i++) {
            s[i] = (unsigned char**) malloc(sizeof(unsigned char*)*p2);
        for (int j = 0; j < p2; j++) {
            s[i][j] = (unsigned char*) malloc(16);
            if (!RAND_bytes((unsigned char*)s[i][j], 16)) {
                printf("Random bytes failed.\n");
                exit(1);
            }
        }
    }
    uint32_t m_bytes = m*mu;
    uint32_t** cw = (uint32_t**) malloc(sizeof(uint32_t*)*p2);
    uint32_t* cw_temp = (uint32_t*) malloc(m_bytes);
    memset(cw_temp, 0, m_bytes);

    // Create cws
    for (int i = 0; i < p2; i++) {
        unsigned char* temp_out = (unsigned char*) malloc(m_bytes);
        prg->generate_random_number(temp_out, s[gamma][i], m_bytes);
        prg->num_keys = m_bytes/16;
        for (int k = 0; k < mu; k++) {
            unsigned char tempIntBytes[4];
            memcpy(tempIntBytes, &temp_out[4*k], 4);
            cw_temp[k] = cw_temp[k] ^ byteArr2Int32(tempIntBytes);
        }
        cw[i] = (uint32_t*) malloc(m_bytes);

        // The last CW has to fulfill a certain condition
        // So we deal with it separately
        if (i == (p2-1)) {
            break;
        }

        if (!RAND_bytes((unsigned char*) cw[i], m_bytes)) {
            printf("Random bytes failed.\n");
            exit(1);
        }

        for (int j = 0; j < mu; j++) {
            cw_temp[j] = cw_temp[j] ^ cw[i][j];
        }
        free(temp_out);
    }
    for (int i = 0; i < mu; i++) {
        if (i == (delta)) {
            cw[p2-1][i] = b ^ cw_temp[i];
        } else {
            cw[p2-1][i] = cw_temp[i];
        }
    }

    free(cw_temp);
    unsigned char *** sigma = (unsigned char***) malloc(sizeof(unsigned char**)* p);
    for (int i = 0; i < p; i++) {
        sigma[i] = (unsigned char**) malloc(sizeof(unsigned char*)*v);
        for (int j = 0; j < v; j++) {
            sigma[i][j] = (unsigned char*) malloc(16*p2);
            for (int k = 0; k < p2; k++) {
                // If the a array has a 0 bit, then set the seed to 0
                if (aArr[j][i][k] == 0) {
                    memset(sigma[i][j] + k*16, 0, 16);
                } else {
                    memcpy(sigma[i][j] + k*16, s[j][k], 16);
                }
            }
        }
    }
    for (int i = 0; i < p; i++) {
        keys[i].sigma = sigma[i];
        keys[i].cw = cw;
    }

    // Clean up unused memory

    for (int i = 0; i < v; i++) {
        for (int j = 0; j < p; j++) {
            free(aArr[i][j]);
        }
        free(aArr[i]);
    }

    free(aArr);
    for (int i = 0; i < v; i++) {
        for (int j = 0; j < p2; j++) {
            free(s[i][j]);
        }
        free(s[i]);
    }
    free(s);
}
