#include "prg.h"
#include "utils.h"
#include "fss-client.h"
#include "fss-server.h"

#include<iostream>

using namespace std;

int main() {
    uint64_t a=3;
    uint64_t b=2;
    uint32_t domain_bits = 10;
    uint32_t num_parties = 2;
    KeyEqual k0;
    KeyEqual k1;

    PseudoRandomGenerator prg;
    FssClient fclient(domain_bits, num_parties);
    FssServer fserver(&fclient);


    fclient.generate_tree_equal(&prg, &k0, &k1, a, b);

    mpz_class ans0, ans1;

    ans0 = fserver.evaluate_equal(&prg, &k0, a);
    ans1 = fserver.evaluate_equal(&prg, &k1, a);
    mpz_class fin;
    fin = ans0-ans1;

    cout << "FSS Eq Match (should be non-zero): " << fin << endl;

    ans0 = fserver.evaluate_equal(&prg, &k0, a-1);
    ans1 = fserver.evaluate_equal(&prg, &k1, a-1);

    fin = ans0-ans1;
    cout<< "Should be zero" << fin<<endl;

}