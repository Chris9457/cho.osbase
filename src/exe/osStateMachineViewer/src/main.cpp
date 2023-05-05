// \brief Service FrontEnd
#include "osApplication/Runner.h"
#include "osData/IMessaging.h"
#include "StateMachineWindow.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include <QApplication>

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();
OS_STATEMACHINE_LINK();

using namespace std::chrono_literals;

namespace nsdata   = NS_OSBASE::data;
namespace nsapp    = NS_OSBASE::application;
namespace nsviewer = NS_OSBASE::statemachineviewer;

int main(int argc, char **argv) {
    static constexpr auto timeout = 10s;

    nsapp::Runner runner(argc, argv);
    return runner.run(
        [options = runner.getOptions(), &argc, &argv]() {
            QApplication app(argc, argv);
            QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
            QCoreApplication::setApplicationName("State Machine Viewer");
            QCoreApplication::setOrganizationName(OS_TO_STR(OSBASE));
            QCoreApplication::setApplicationVersion("1.0.0");

            auto const pStateMachineWindow = options.brokerUrl.has_value()
                                                 ? std::make_unique<nsviewer::StateMachineWindow>(
                                                       options.brokerUrl.value(), options.realm.value_or(nsdata::IMessaging::DEFAULT_REALM))
                                                 : std::make_unique<nsviewer::StateMachineWindow>();
            pStateMachineWindow->show();
            return app.exec();
        },
        []() { QCoreApplication::quit(); });
}
