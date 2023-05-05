// \brief Definition of the launcher

#include "LauncherSettings.h"
#include "osApplication//Process.h"
#include "osCoreImpl/CoreImpl.h"
#include "osDataImpl/osDataImpl.h"
#include "osData/Uri.h"
#include <fstream>
#include <iostream>

OS_CORE_IMPL_LINK();
OS_DATA_IMPL_LINK();

using namespace std::chrono_literals;
namespace nsdata = NS_OSBASE::data;
namespace nsapp  = NS_OSBASE::application;

int main(int argc, char **argv) {
    // This test is only to check in CI if the application can be launched
    if (argc > 1 && std::string(argv[1]) == "-h") {
        return 0;
    }

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

    nsapp::ServiceOptions options = { launcherSettings.brokerUrl, {} };

    // broker
    auto pBrokerProcess = nsapp::Process::create({ BROKER_NAME }, options);
    options.brokerUrl   = pBrokerProcess->getData<nsdata::Uri>();
    std::vector<nsapp::ProcessPtr> pProcesses;

    for (auto &&launcherSetting : launcherSettings.settings) {
        auto const pServiceProcess = nsapp::Process::create(launcherSetting, options);
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
