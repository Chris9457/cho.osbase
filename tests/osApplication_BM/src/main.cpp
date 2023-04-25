#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include "Server.h"
#include "benchmark/benchmark.h"

OS_CORE_IMPL_LINK()
OS_DATA_IMPL_LINK()

namespace nsosbase = NS_OSBASE;
namespace nscore   = nsosbase::core;
namespace nsappbm  = nsosbase::application::bm;

int main(int argc, char **argv) {
    auto const guard = nscore::make_scope_exit([]() { nsappbm::TheServer.disconnect(); });
    nsappbm::TheServer.connect();

    ::benchmark::Initialize(&argc, argv);
    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) {
        return 1;
    }

    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();

    return 0;
}
