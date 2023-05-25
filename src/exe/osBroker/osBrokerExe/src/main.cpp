// \brief Declaration of the broker

#include "BrokerRunner.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();

namespace nsbroker = NS_OSBASE::broker;

int main(int argc, char **argv) {
    nsbroker::BrokerRunner runner(argc, argv);
    return runner.run();
}
