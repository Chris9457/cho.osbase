// \brief Declaration of the broker

#include "osData/IBroker.h"
#include "osData/INetwork.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include "osApplication/Runner.h"

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();

namespace nsosbase = NS_OSBASE;
namespace nsdata   = nsosbase::data;
namespace nsapp    = nsosbase::application;

int main(int argc, char **argv) {
    auto const pBroker = nsdata::makeBroker();
    nsapp::Runner runner(argc, argv);

    std::condition_variable cvStop;
    std::mutex mutStop;
    bool bStop = false;

    return runner.run(
        [&runner, &pBroker, &mutStop, &cvStop, &bStop]() {
            auto const port = pBroker->start(
                runner.getOptions().brokerUrl.value_or(nsdata::Uri("ws://127.0.0.1:8080")).authority.value().port.value_or(8080));

            auto const pNetwork = nsdata::makeNetwork();
            auto const brokerUri =
                nsdata::Uri{ nsdata::Uri::schemeWebsocket(), nsdata::Uri::Authority{ {}, pNetwork->getLocalHost(), port } };
            runner.sendData(brokerUri);

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
