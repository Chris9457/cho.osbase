// \brief entry point function
#include "WebViewerRes.h"
#include "osApplication/Process.h"
#include "osData/IBroker.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include <thread>
#include <gtest/gtest.h>

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();

using namespace std::chrono_literals;
namespace nsosbase = cho::osbase;
namespace nsdata   = nsosbase::data;
namespace nsapp    = nsosbase::application;

namespace cho::osbase::webviewer::ut {
    class WebViewerLauncher {
    public:
        WebViewerLauncher() : m_pBroker(nsdata::makeBroker()) {
            m_pBroker->start(brokerPort);
            m_pProcess = nsapp::Process::create({ WEBVIEWER_NAME }, { brokerUrl, brokerPort });
            std::this_thread::sleep_for(1s);
        }

        ~WebViewerLauncher() {
            m_pProcess.reset();
            m_pBroker->stop();
        }

    private:
        nsapp::ProcessPtr m_pProcess;
        nsdata::IBrokerPtr m_pBroker;
    };
} // namespace cho::osbase::webviewer::ut

int main(int argc, char **argv) {
    cho::osbase::webviewer::ut::WebViewerLauncher launcher;

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
