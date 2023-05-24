// \brief Service Monitoring
#include "logimpl/LogServiceImpl.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include "osApplication/Runner.h"
#include "osApplication/ServiceConfiguration.h"
#include "osApplication/ServiceSettings.h"

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();

namespace nscore = NS_OSBASE::core;
namespace nsdata = NS_OSBASE::data;
namespace nsapp  = NS_OSBASE::application;
namespace nslog  = NS_OSBASE::log;

int main(int argc, char **argv) {
    nsapp::Runner runner(argc, argv);
    return runner.run(
        [&runner]() {
            auto const guard = nscore::make_scope_exit([]() { nslog::TheLogServiceImpl.disconnect(); });

            auto const settings = runner.getData<nsapp::ServiceSettings>();
            auto const input    = settings.serviceInput.value_or(
                nsapp::ServiceSettingsInput{ {}, nsdata::Uri("ws://127.0.0.1:8080"), nsdata::IMessaging::DEFAULT_REALM });

            nsapp::TheServiceConfiguration.setBrokerUri(input.brokerUrl);
            nsapp::TheServiceConfiguration.setRealm(input.realm);
            nslog::TheLogServiceImpl.connect();
            nslog::TheLogServiceImpl.run();
            return 0;
        },
        []() {
            nslog::TheLogServiceImpl.stop();
            return;
        });
}
