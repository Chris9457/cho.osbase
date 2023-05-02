// \brief Service FrontEnd
#include "StateMachineWindow.h"
#include "osApplication/ServiceCommandParser.h"
#include "osApplication/ServiceException.h"
#include "osApplication/ServiceHandshake.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include "osStateMachine/osStateMachine.h"
#include <QApplication>

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();
OS_STATEMACHINE_LINK();

using namespace std::chrono_literals;

namespace nsosbase = NS_OSBASE;
namespace nscore   = nsosbase::core;
namespace nsdata   = nsosbase::data;
namespace nsapp    = nsosbase::application;
namespace nsviewer = NS_OSBASE::statemachineviewer;

class HandshakeDelegate : public nsapp::ServiceHandshake::IDelegate {
public:
    void onEngage() override {
    }
    void onDisengage() override {
        QCoreApplication::quit();
    }
};

nsapp::ServiceHandshakePtr pHandshake;
std::shared_ptr<HandshakeDelegate> pHandshakeDelegate;

int main(int argc, char **argv) {
    static constexpr auto timeout = 10s;
    auto const options            = type_cast<nsapp::ServiceOptions>(nsapp::ServiceCommandParser(argc, argv));
    const std::string url         = options.brokerUrl ? *options.brokerUrl : "127.0.0.1";
    const unsigned short port     = options.brokerPort ? *options.brokerPort : 8080;
    auto guard                    = nscore::make_scope_exit([]() {
        if (pHandshake != nullptr) {
            pHandshake->disengage();
        }
    });

    if (options.handshakeScheme) {
        pHandshake = nsapp::ServiceHandshake::create(
            nsdata::Uri({ *options.handshakeScheme, nsdata::Uri::Authority{ {}, *options.handshakeHost, *options.handshakePort } }));
        pHandshakeDelegate = std::make_shared<HandshakeDelegate>();
        pHandshake->setDelegate(pHandshakeDelegate);
        pHandshake->engage(timeout);
    }

    QApplication app(argc, argv);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setApplicationName("State Machine Viewer");
    QCoreApplication::setOrganizationName(OS_TO_STR(OSBASE));
    QCoreApplication::setApplicationVersion("1.0.0");

    auto const pStateMachineWindow = pHandshake != nullptr ? std::make_unique<nsviewer::StateMachineWindow>(
                                                                 std::string{ "ws://" + url + ":" + std::to_string(port) }, "")
                                                           : std::make_unique<nsviewer::StateMachineWindow>();
    pStateMachineWindow->show();
    return app.exec();
}
