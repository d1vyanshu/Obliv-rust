#include "prg.h"
#include "utils.h"
#include "fss-client.h"
#include "fss-server.h"

#include<iostream>
#include<gtest/gtest.h>
#include<ctime>
#include<cstdlib>

using namespace std;

namespace {
    //Test for DPF

    //a and b are 32 bit numbers;
    TEST(DPF, bits32) {
        PseudoRandomGenerator prg;
        for(int i=0; i<1000; i++) {
            uint32_t a,b, input, output, domain_bits = 32, num_parties =2;

            KeyEqual k0, k1;


            FssClient fclient(domain_bits, num_parties);
            FssServer fserver(&fclient);
            
            a = rand();
            b = rand();

            fclient.generate_tree_equal(&prg, &k0, &k1, a, b);

            for(int j=0; j<100; j++) {
                uint32_t ans0, ans1, fin;

                input = rand();

                if(input==a) output = b;
                else output = 0;

                ans0 = fserver.evaluate_equal(&prg, 0, &k0, input);
                ans1 = fserver.evaluate_equal(&prg, 1, &k1, input);

                fin = (ans0+ans1)%(1<<domain_bits);

                EXPECT_EQ(output, fin) << "Output different for a = "<<a<<" b = "<<b<<"for input = "<<input<<endl;
            }

        }


    }

    //a and b are 64 bit numbers
    TEST(DPF, bits64) {
        PseudoRandomGenerator prg;
        for(int i=0; i<1000; i++) {
            uint64_t a,b, input, output, domain_bits = 64, num_parties = 2;

            KeyEqual k0, k1;


            FssClient fclient(domain_bits, num_parties);
            FssServer fserver(&fclient);
            
            a = rand();
            b = rand();

            fclient.generate_tree_equal(&prg, &k0, &k1, a, b);

            for(int j=0; j<100; j++) {
                uint64_t ans0, ans1, fin;

                input = rand();

                if(input==a) output = b;
                else output = 0;

                ans0 = fserver.evaluate_equal(&prg, 0, &k0, input);
                ans1 = fserver.evaluate_equal(&prg, 1, &k1, input);

                fin = (ans0+ans1)%(1<<domain_bits);

                EXPECT_EQ(output, fin) << "Output different for a = "<<a<<" b = "<<b<<"for input = "<<input<<endl;
            }

        }
    }

    //Test for DIF

    //a and b are 32 bit numbers
    TEST(DIF, bits32) {
        PseudoRandomGenerator prg;
        for(int i=0; i<1000; i++) {
            uint32_t a,b, input, output, domain_bits = 32, num_parties =2;

            KeyLessThan k0, k1;


            FssClient fclient(domain_bits, num_parties);
            FssServer fserver(&fclient);
            
            a = rand();
            b = rand();

            fclient.generate_tree_lessthan(&prg, &k0, &k1, a, b);

            for(int j=0; j<100; j++) {
                uint32_t ans0, ans1, fin;

                input = rand();

                if(input<=a) output = b;
                else output = 0;

                ans0 = fserver.evaluate_lessthan(&prg, &k0, input);
                ans1 = fserver.evaluate_lessthan(&prg, &k1, input);

                fin = ans0-ans1;

                EXPECT_EQ(output, fin) << "Output different for a = "<<a<<" b = "<<b<<"for input = "<<input<<endl;
            }

        }


    }

    TEST(DIF, bits64) {
        PseudoRandomGenerator prg;
        for(int i=0; i<1000; i++) {
            uint64_t a,b, input, output, domain_bits = 32, num_parties =2;

            KeyLessThan k0, k1;


            FssClient fclient(domain_bits, num_parties);
            FssServer fserver(&fclient);
            
            a = rand();
            b = rand();

            fclient.generate_tree_lessthan(&prg, &k0, &k1, a, b);

            for(int j=0; j<100; j++) {
                uint64_t ans0, ans1, fin;

                input = rand();

                if(input<=a) output = b;
                else output = 0;

                ans0 = fserver.evaluate_lessthan(&prg, &k0, input);
                ans1 = fserver.evaluate_lessthan(&prg, &k1, input);

                fin = ans0-ans1;

                EXPECT_EQ(output, fin) << "Output different for a = "<<a<<" b = "<<b<<"for input = "<<input<<endl;
            }

        }


    }

    // TEST(MultiParty, bits64) {
    //     PseudoRandomGenerator prg;
    //     for(int i=0; i<1000; i++) {
    //         uint64_t a,b, input, output, domain_bits = 32, num_parties = 3;

    //         MultiPartyKey keys[3];


    //         FssClient fclient(domain_bits, num_parties);
    //         FssServer fserver(&fclient);
            
    //         a = rand();
    //         b = rand();

    //         fclient.generate_tree_equal_Mparty(&prg, a, b, keys);

    //         for(int j=0; j<100; j++) {
    //             uint64_t ans0, ans1, ans2, fin;

    //             input = rand();

    //             if(input==a) output = b;
    //             else output = 0;

    //             ans0 = fserver.evaluate_equal_Mparty(&prg, &keys[0], input);
    //             ans1 = fserver.evaluate_equal_Mparty(&prg, &keys[1], input);
    //             ans2 = fserver.evaluate_equal_Mparty(&prg, &keys[2], input);

    //             fin = ans0 ^ ans1 ^ ans2;

    //             EXPECT_EQ(output, fin) << "Output different for a = "<<a<<" b = "<<b<<"for input = "<<input<<endl;
    //         }

    //     }
    // }

}

int main(int argc, char** argv) {
    srand(time(nullptr));
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}