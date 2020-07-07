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