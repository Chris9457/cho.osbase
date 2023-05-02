// \brief Service Monitoring
#include "logimpl/LogServiceImpl.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include "osApplication/Runner.h"
#include "osApplication/ServiceConfiguration.h"

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();

namespace nslog    = NS_OSBASE::log;
namespace nsosbase = NS_OSBASE;
namespace nscore   = nsosbase::core;
namespace nsapp    = nsosbase::application;

int main(int argc, char **argv) {
    nsapp::Runner runner(argc, argv);
    return runner.run(
        [url = runner.getBrokerUrl(), port = runner.getBrokerPort()]() {
            auto const guard = nscore::make_scope_exit([]() { nslog::TheLogServiceImpl.disconnect(); });

            nsapp::TheServiceConfiguration.setBrokerUri(std::string{ "ws://" + url + ":" + std::to_string(port) });
            nsapp::TheServiceConfiguration.setRealm("");
            nslog::TheLogServiceImpl.connect();
            nslog::TheLogServiceImpl.run();
        },
        []() { nslog::TheLogServiceImpl.stop(); });
}
