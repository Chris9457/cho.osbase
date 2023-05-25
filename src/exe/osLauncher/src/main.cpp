// \brief Definition of the launcher

#include "LauncherSettings.h"
#include "osBrokerApi/Settings.h"
#include "osApplication/Process.h"
#include "osApplication/ServiceSettings.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include "osData/Uri.h"
#include "osData/IMessaging.h"
#include <fstream>
#include <iostream>

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();

using namespace std::chrono_literals;
namespace nsdata   = NS_OSBASE::data;
namespace nsapp    = NS_OSBASE::application;
namespace nsbroker = NS_OSBASE::broker;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    std::ifstream ifs(SETTING_FILE_NAME, std::ios_base::in);
    if (!ifs.is_open()) {
        std::cout << "Unable to open the file " << SETTING_FILE_NAME << std::endl;
        return 1;
    }

    auto const pKeyStream       = NS_OSBASE::core::makeJsonStream(std::move(ifs));
    auto const launcherSettings = pKeyStream->getValue(NS_OSBASE::launcher::LauncherSettings{});

    std::cout << "******************************************************" << std::endl;
    std::cout << "***                     OSBASE                     ***" << std::endl;
    std::cout << "******************************************************" << std::endl;
    std::cout << std::endl;
    std::cout << "Launching processes: " << std::endl;

    // broker
    nsapp::ServiceSettings serviceSettings;
    auto const port = launcherSettings.brokerUrl.authority.value_or(nsdata::Uri::Authority{ {}, {}, 8080 }).port.value_or(8080);
    nsbroker::Settings brokerSettings{ nsbroker::Input{ port }, {} };
    auto pBrokerProcess = nsapp::Process::create({ BROKER_NAME }, brokerSettings);
    brokerSettings      = pBrokerProcess->getData<nsbroker::Settings>(10s);

    serviceSettings.serviceInput = nsapp::ServiceSettingsInput{ false,
        brokerSettings.output.value_or(nsbroker::Output{ type_cast<nsdata::Uri>(std::string("ws://127.0.0.1:8080")) }).uri,
        launcherSettings.realm };

    std::vector<nsapp::ProcessPtr> pProcesses;

    for (auto &&launcherSetting : launcherSettings.settings) {
        auto const pServiceProcess = nsapp::Process::create(launcherSetting, serviceSettings);
        pProcesses.push_back(pServiceProcess);
    }

    static const std::string strStop = "stop";
    std::cout << std::endl;
    while (true) {
        std::cout << "Type \"stop\" to stop osBase" << std::endl;
        std::string line;
        std::getline(std::cin, line);
        std::cout << std::endl;
        if (line == strStop)
            break;
    }

    return 0;
}
