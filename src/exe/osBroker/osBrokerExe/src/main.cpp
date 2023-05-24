// \brief Declaration of the broker

#include "osBrokerApi/Settings.h"
#include "osData/IBroker.h"
#include "osData/INetwork.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include "osApplication/Runner.h"

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();

namespace nsdata   = NS_OSBASE::data;
namespace nsapp    = NS_OSBASE::application;
namespace nsbroker = NS_OSBASE::broker;

int main(int argc, char **argv) {
    auto const pBroker = nsdata::makeBroker();
    nsapp::Runner runner(argc, argv);

    std::condition_variable cvStop;
    std::mutex mutStop;
    bool bStop = false;

    return runner.run(
        [&runner, &pBroker, &mutStop, &cvStop, &bStop]() {
            auto settings   = runner.getData<nsbroker::Settings>();
            auto const port = pBroker->start(settings.input.value_or(nsbroker::Input{ 8080 }).port);

            auto const pNetwork = nsdata::makeNetwork();
            settings.output     = nsbroker::Output{ nsdata::Uri{
                nsdata::Uri::schemeWebsocket(), nsdata::Uri::Authority{ {}, pNetwork->getLocalHost(), port } } };
            runner.sendData(settings);

            std::unique_lock lock(mutStop);
            cvStop.wait(lock, [&bStop] { return bStop; });

            return 0;
        },
        [&pBroker, &bStop, &mutStop, &cvStop]() {
            pBroker->stop();

            std::lock_guard lock(mutStop);
            bStop = true;
            cvStop.notify_one();
        });
}
