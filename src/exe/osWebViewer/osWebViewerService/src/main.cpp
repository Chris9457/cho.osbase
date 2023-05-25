// \brief WebViewer

#include "osWebViewerImpl/WebViewerServiceImpl.h"
#include "osWeb/link.h"
#include "osApplication/ServiceRunner.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"

OS_CORE_LINK_KEYSTREAM_JSON()
OS_DATA_IMPL_LINK();
OS_WEBENGINE_LINK();

namespace nsapp       = NS_OSBASE::application;
namespace nswebviewer = NS_OSBASE::webviewer;

int main(int argc, char **argv) {
    nsapp::ServiceRunner<nswebviewer::WebViewerServiceImpl> runner(argc, argv);
    return runner.run();
}
