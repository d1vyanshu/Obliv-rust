#include "../src/prg.h"
#include "../src/utils.h"
#include "../src/fss-client.h"
#include "../src/fss-server.h"

#include<iostream>

using namespace std;

int main() {
    uint64_t a=3;
    uint64_t b=2;
    uint32_t domain_bits = 10;
    uint32_t num_parties = 2;
    KeyEqual k0;
    KeyEqual k1;

    //PRG to use in schemes, can be initialised only once.
    PseudoRandomGenerator prg;

    //Code for point function sharing between two parties
    FssClient fclient_pointfunc(domain_bits, num_parties);
    FssServer fserver_pointfunc(&fclient_pointfunc);


    fclient_pointfunc.generate_tree_equal(&prg, &k0, &k1, a, b);

    mpz_class ans0, ans1;

    ans0 = fserver_pointfunc.evaluate_equal(&prg, &k0, a);
    ans1 = fserver_pointfunc.evaluate_equal(&prg, &k1, a);
    mpz_class fin;
    fin = ans0-ans1;

    cout << "FSS Eq Match (should be non-zero): " << fin << endl;

    ans0 = fserver_pointfunc.evaluate_equal(&prg, &k0, a-1);
    ans1 = fserver_pointfunc.evaluate_equal(&prg, &k1, a-1);

    fin = ans0-ans1;
    cout<< "Should be zero" << fin<<endl;
    //end 

    //Code for less than interval function sharing between two parties
    KeyLessThan k2;
    KeyLessThan k3;

    FssClient fclient_interval(domain_bits, num_parties);
    FssServer fserver_interval(&fclient_interval);

    fclient_interval.generate_tree_lessthan(&prg, &k2, &k3, a, b);
    uint64_t lt_ans0, lt_ans1;

    lt_ans0 = fserver_interval.evaluate_lessthan(&prg, &k2, (a));
    lt_ans1 = fserver_interval.evaluate_lessthan(&prg, &k3, (a));
    uint64_t lt_final = lt_ans0 - lt_ans1;
    cout << "FSS Lt Match (should be non-zero): " << lt_final << endl;

    lt_ans0 = fserver_interval.evaluate_lessthan(&prg, &k2, a+1);
    lt_ans1 = fserver_interval.evaluate_lessthan(&prg, &k3, a+1);
    lt_final = lt_ans0 - lt_ans1;
    cout << "FSS Lt No Match (should be 0): " << lt_final << endl;
    //end


    //Code for point function shared with m-parties.
    MultiPartyKey keys[3];
    domain_bits = 10;
    num_parties = 3;
    PseudoRandomGenerator pseudo;
    FssClient fclient_mparty(domain_bits, num_parties);
    FssServer fserver_mparty(&fclient_mparty);

    fclient_mparty.generate_tree_equal_Mparty(&pseudo, a, b, keys);

    uint32_t mp_ans0 = fserver_mparty.evaluate_equal_Mparty(&pseudo, &keys[0], a);
    uint32_t mp_ans1 = fserver_mparty.evaluate_equal_Mparty(&pseudo, &keys[1], a);
    uint32_t mp_ans2 = fserver_mparty.evaluate_equal_Mparty(&pseudo, &keys[2], a);

    uint32_t mp_final = mp_ans0 ^ mp_ans1 ^ mp_ans2;
    cout << "FSS Eq Multi-Party Match (should be non-zero): " << mp_final << endl;

    mp_ans0 = fserver_mparty.evaluate_equal_Mparty(&pseudo, &keys[0], a+1);
    mp_ans1 = fserver_mparty.evaluate_equal_Mparty(&pseudo, &keys[1], a+1);
    mp_ans2 = fserver_mparty.evaluate_equal_Mparty(&pseudo, &keys[2], a+1);
    mp_final = mp_ans0 ^ mp_ans1 ^ mp_ans2;
    cout << "FSS Eq Multi-Party No Match (should be 0): " << mp_final << endl;
    //end

    //Code for dpf with improvement
    KeyTest key0, key1;
    a=3,b=2;
    domain_bits = 10;
    num_parties = 2;
    PseudoRandomGenerator pse;
    FssClient fclient_improved(domain_bits, num_parties);
    FssServer fserver_improved(&fclient_improved);

    uint64_t a1,a2;
    fclient_improved.tree_test(&pse, &key0, &key1, a, b);
    a1 = fserver_improved.evaluate_test_tree(&pse, 0, &key0, a);
    a2 = fserver_improved.evaluate_test_tree(&pse, 1, &key1, a);

    cout<<"Answer should be non-zero: "<<(a1+a2)%(1<<domain_bits)<<endl;

    a1 = fserver_improved.evaluate_test_tree(&pse, 0, &key0, a-1);
    a2 = fserver_improved.evaluate_test_tree(&pse, 1, &key1, a-1);

    cout<<"Answer should be zero: "<<(a1+a2)%(1<<domain_bits)<<endl;

    //end


    return 0;
}
