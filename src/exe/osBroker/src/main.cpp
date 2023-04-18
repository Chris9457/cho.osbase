// \brief Declaration of the broker

#include "osData/IBroker.h"
#include "osDataImpl/osDataImpl.h"
#include "osApplication/Runner.h"
#include <future>

OS_DATA_IMPL_LINK();

namespace nsosbase = cho::osbase;
namespace nsdata   = nsosbase::data;
namespace nsapp    = nsosbase::application;

int main(int argc, char **argv) {
    auto const pBroker = nsdata::makeBroker();
    std::promise<void> promiseStop;
    nsapp::Runner runner(argc, argv);
    return runner.run(
        [port = runner.getBrokerPort(), &pBroker, &promiseStop]() {
            auto const futStop = promiseStop.get_future();
            pBroker->start(port);
            futStop.wait();
        },
        [&pBroker, &promiseStop]() {
            pBroker->stop();
            promiseStop.set_value();
        });
}
