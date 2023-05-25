// \brief Declaration of the class BrokerRunner

#pragma once
#include "BrokerRunner.h"
#include "osBrokerApi/Settings.h"
#include "osData/INetwork.h"
#include "osData/IBroker.h"

namespace NS_OSBASE::broker {

    /*
     * \class BrokerRunner
     */
    BrokerRunner::BrokerRunner(int argc, char **argv) : Runner(argc, argv) {
    }

    int BrokerRunner::run() {
        auto const pBroker = data::makeBroker();
        std::condition_variable cvStop;
        std::mutex mutStop;
        bool bStop = false;

        return Runner::run(
            [this, &pBroker, &mutStop, &cvStop, &bStop]() {
                auto settings   = getData<Settings>();
                auto const port = pBroker->start(settings.input.value_or(Input{ 8080 }).port);

                auto const pNetwork = data::makeNetwork();
                settings.output =
                    Output{ data::Uri{ data::Uri::schemeWebsocket(), data::Uri::Authority{ {}, pNetwork->getLocalHost(), port } } };
                sendData(settings);

                std::unique_lock lock(mutStop);
                cvStop.wait(lock, [&bStop] { return bStop; });

                return 0;
            },
            [&pBroker, &bStop, &mutStop, &cvStop]() {
                pBroker->stop();

                std::lock_guard lock(mutStop);
                bStop = true;
                cvStop.notify_one();
            });
    }

} // namespace NS_OSBASE::broker
