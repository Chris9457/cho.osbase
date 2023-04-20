// \brief WebViewer

#include "osWeb/link.h"
#include "osApplication/Runner.h"
#include "osWebViewerImpl/WebViewerServiceImpl.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"

OS_CORE_LINK_KEYSTREAM_JSON()
OS_DATA_IMPL_LINK();
OS_WEBENGINE_LINK();

namespace nsosbase    = cho::osbase;
namespace nscore      = nsosbase::core;
namespace nsapp       = nsosbase::application;
namespace nswebviewer = nsosbase::webviewer;

int main(int argc, char **argv) {
    nsapp::Runner runner(argc, argv);
    return runner.run(
        [url = runner.getBrokerUrl(), port = runner.getBrokerPort()]() {
            auto const guard = nscore::make_scope_exit([]() { nswebviewer::TheWebViewerServiceImpl.disconnect(); });
            nswebviewer::TheWebViewerServiceImpl.connect(url, port);
            nswebviewer::TheWebViewerServiceImpl.run();
        },
        []() { nswebviewer::TheWebViewerServiceImpl.stop(); });
}