#include<benchmark/benchmark.h>
#include<ctime>
#include<cstdlib>

#include "prg.h"
#include "fss-server.h"
#include "fss-client.h"
#include "utils.h"

void Initialise_prg(benchmark::State& state) {
    for(auto _:state)
        PseudoRandomGenerator prg;
}

BENCHMARK(Initialise_prg);

void DPF_Gen_Algo_32bit(benchmark::State& state) {
    KeyEqual k0, k1;
    uint32_t input,output;
    input = rand();
    output=rand();
    PseudoRandomGenerator prg;

    for(auto _:state) {
        FssClient fclient(32, 2);
        fclient.generate_tree_equal(&prg, &k0, &k1, input, output);
    }
}

BENCHMARK(DPF_Gen_Algo_32bit);

void DPF_Gen_Algo_64bit(benchmark::State& state) {
    KeyEqual k0, k1;
    uint64_t input,output;
    input = rand();
    output=rand();
    PseudoRandomGenerator prg;

    for(auto _:state) {
        FssClient fclient(64, 2);
        fclient.generate_tree_equal(&prg, &k0, &k1, input, output);
    }
}

BENCHMARK(DPF_Gen_Algo_64bit);

void DPF_Eval_Algo_32bit(benchmark::State& state) {
    KeyEqual k0, k1;
    uint32_t input,output;
    input=rand();
    output = rand();
    PseudoRandomGenerator prg;

    FssClient fclient(32, 2);
    fclient.generate_tree_equal(&prg, &k0, &k1, input, output);
    uint32_t a1,a2;

    for(auto _:state) {
        FssServer fserver(&fclient);
        a1 = fserver.evaluate_equal(&prg, 0, &k0, input);
        a2 = fserver.evaluate_equal(&prg, 1, &k1, input);
    }
}

BENCHMARK(DPF_Eval_Algo_32bit);

void DPF_Eval_Algo_64bit(benchmark::State& state) {
    KeyEqual k0, k1;
    uint64_t input,output;
    input=rand();
    output = rand();
    PseudoRandomGenerator prg;

    FssClient fclient(64, 2);
    fclient.generate_tree_equal(&prg, &k0, &k1, input, output);
    uint64_t a1,a2;

    for(auto _:state) {
        FssServer fserver(&fclient);
        a1 = fserver.evaluate_equal(&prg, 0, &k0, input);
        a2 = fserver.evaluate_equal(&prg, 1, &k1, input);
    }
}

BENCHMARK(DPF_Eval_Algo_64bit);

void DIF_Gen_Algo_32bit(benchmark::State& state) {
    KeyLessThan k2;
    KeyLessThan k3;
    uint32_t input, output;
    input = rand();
    output = rand();
    PseudoRandomGenerator prg;

    for(auto _:state) {
        FssClient fclient(32, 2);
        fclient.generate_tree_lessthan(&prg, &k2, &k3, input, output);
    }
}

BENCHMARK(DIF_Gen_Algo_32bit);

void DIF_Gen_Algo_64bit(benchmark::State& state) {
    KeyLessThan k2;
    KeyLessThan k3;
    uint64_t input, output;
    input = rand();
    output = rand();
    PseudoRandomGenerator prg;

    for(auto _:state) {
        FssClient fclient(64, 2);
        fclient.generate_tree_lessthan(&prg, &k2, &k3, input, output);
    }
}

BENCHMARK(DIF_Gen_Algo_64bit);

void DIF_Eval_Algo_32bit(benchmark::State& state) {
    KeyLessThan k2;
    KeyLessThan k3;
    uint32_t input, output;
    input = rand();
    output = rand();
    PseudoRandomGenerator prg;

    FssClient fclient(32, 2);
    fclient.generate_tree_lessthan(&prg, &k2, &k3, input, output);
    uint32_t ans1, ans2;

    for(auto _:state) {
        FssServer fserver(&fclient);
        ans1 = fserver.evaluate_lessthan(&prg, &k2, input);
        ans2 = fserver.evaluate_lessthan(&prg, &k2, input);
    }
    
}

BENCHMARK(DIF_Eval_Algo_32bit);

void DIF_Eval_Algo_64bit(benchmark::State& state) {
    KeyLessThan k2;
    KeyLessThan k3;
    uint64_t input, output;
    input = rand();
    output = rand();
    PseudoRandomGenerator prg;

    FssClient fclient(64, 2);
    fclient.generate_tree_lessthan(&prg, &k2, &k3, input, output);
    uint64_t ans1, ans2;

    for(auto _:state) {
        FssServer fserver(&fclient);
        ans1 = fserver.evaluate_lessthan(&prg, &k2, input);
        ans2 = fserver.evaluate_lessthan(&prg, &k2, input);
    }
    
}

BENCHMARK(DIF_Eval_Algo_64bit);

void DPF_3party_Gen(benchmark::State& state) {
    MultiPartyKey keys[3];
    uint32_t input, output;
    input = rand();
    output = rand();
    PseudoRandomGenerator prg;

    for(auto _:state) {
        FssClient fclient(32, 3);
        fclient.generate_tree_equal_Mparty(&prg, input, output, keys);
    }
}

BENCHMARK(DPF_3party_Gen);

void DPF_3party_Eval(benchmark::State& state) {
    MultiPartyKey keys[3];
    uint32_t input, output;
    input = rand();
    output = rand();
    PseudoRandomGenerator prg;
    FssClient fclient(32, 3);
    fclient.generate_tree_equal_Mparty(&prg, input, output, keys);
    uint32_t a1,a2,a3;

    for(auto _:state) {
        FssServer fserver(&fclient);
        a1 = fserver.evaluate_equal_Mparty(&prg, &keys[0], input);
        a2 = fserver.evaluate_equal_Mparty(&prg, &keys[1], input);
        a3 = fserver.evaluate_equal_Mparty(&prg, &keys[2], input);
    }
}

BENCHMARK(DPF_3party_Eval);

BENCHMARK_MAIN();
