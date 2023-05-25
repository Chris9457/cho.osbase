// \brief Service FrontEnd
#include "osApplication/Runner.h"
#include "osData/IMessaging.h"
#include "StateMachineWindow.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include "osApplication/ServiceSettings.h"
#include <QApplication>

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();
OS_STATEMACHINE_LINK();

using namespace std::chrono_literals;

namespace nsdata   = NS_OSBASE::data;
namespace nsapp    = NS_OSBASE::application;
namespace nsviewer = NS_OSBASE::statemachineviewer;

int main(int argc, char **argv) {
    nsapp::Runner runner(argc, argv);
    return runner.run(
        [&runner, &argc, &argv]() {
            QApplication app(argc, argv);
            QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
            QCoreApplication::setApplicationName("State Machine Viewer");
            QCoreApplication::setOrganizationName(OS_TO_STR(OSBASE));
            QCoreApplication::setApplicationVersion("1.0.0");

            auto const settings = runner.getData<nsapp::ServiceSettings>();
            auto const input    = settings.serviceInput.value_or(
                nsapp::ServiceSettingsInput{ {}, nsdata::Uri("ws://127.0.0.1:8080"), nsdata::IMessaging::DEFAULT_REALM });

            auto const pStateMachineWindow = settings.serviceInput.has_value()
                                                 ? std::make_unique<nsviewer::StateMachineWindow>(input.brokerUrl, input.realm)
                                                 : std::make_unique<nsviewer::StateMachineWindow>();
            pStateMachineWindow->show();
            return app.exec();
        },
        []() { QCoreApplication::quit(); });
}
