#include "benchmark/benchmark.h"
#include "osApplication/Task.h"

using namespace cho::osbase::application;

namespace {

    static int s_myI = 0;
    static std::string s_myStr;

    void doVoid(int i, const std::string str) {
        s_myI   = i;
        s_myStr = str;
    };

    int doInt(int i, const std::string str) {
        s_myI   = i + 1;
        s_myStr = "_" + str;
        return i;
    }
} // namespace

static void BM_makeFunction(benchmark::State &state) {
    for (auto _ : state) {
        auto const pFnTask = makeTask(&doVoid, 2, "toto");
    }
}
BENCHMARK(BM_makeFunction);

static void BM_makeTypedFunction(benchmark::State &state) {
    for (auto _ : state) {
        auto const pFnTask = makeTypedTask<int>(&doInt, 2, "toto");
    }
}
BENCHMARK(BM_makeTypedFunction);

static void BM_executeFunction(benchmark::State &state) {
    auto const pFnTask = makeTask(&doVoid, 2, "toto");

    for (auto _ : state) {
        pFnTask->execute();
    }
}
BENCHMARK(BM_executeFunction);

static void BM_executeFunctionWithoutTask(benchmark::State &state) {
    auto const pFnTask = makeTask(&doVoid, 2, "toto");

    for (auto _ : state) {
        doVoid(2, "toto");
    }
}
BENCHMARK(BM_executeFunctionWithoutTask);
