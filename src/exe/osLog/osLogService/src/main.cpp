// \brief Service Monitoring
#include "logimpl/LogServiceImpl.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include "osApplication/ServiceRunner.h"

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();

namespace nsapp = NS_OSBASE::application;
namespace nslog = NS_OSBASE::log;

int main(int argc, char **argv) {
    nsapp::ServiceRunner<nslog::LogServiceImpl> runner(argc, argv);
    return runner.run();
}
