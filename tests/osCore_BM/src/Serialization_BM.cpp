// \brief Declaration of the benchmarking tests of the serialization

#include "osCore/Serialization/Serializer.h"
#include "benchmark/benchmark.h"
#include <sstream>

namespace NS_OSBASE::core::bm {

    void serializeVector(std::ostream &os, const size_t size) {
        os << makeBinaryValue(std::vector<int>(size, 2));
    }
} // namespace NS_OSBASE::core::bm

void BM_SerializeVector(benchmark::State &state) {
    std::stringstream ss;
    auto out = NS_OSBASE::core::makeBinaryValue<std::vector<int>>();

    for (auto _ : state) {
        auto const in = NS_OSBASE::core::makeBinaryValue(std::vector<int>(state.range(0), 2));

        ss << in;
        ss >> out;

        std::stringstream temp;
        ss.swap(temp);
    }
}

BENCHMARK(BM_SerializeVector)->RangeMultiplier(2)->Range(1 << 10, 1 << 20)->Unit(benchmark::kMillisecond);
