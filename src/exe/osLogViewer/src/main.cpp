// \brief Declaration of the logviewer

#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include "logviewer.h"

OS_CORE_IMPL_LINK()
OS_DATA_IMPL_LINK()

namespace nslogviewer = NS_OSBASE::logviewer;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    const nslogviewer::LogViewer logViewer;
    logViewer.run();
    return 0;
}
