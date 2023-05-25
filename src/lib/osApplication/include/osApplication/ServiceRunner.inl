// \brief Declaration of the class ServiceRunner

#pragma once
#include "ServiceSettings.h"
#include "ServiceConfiguration.h"

namespace NS_OSBASE::application {

    /*
     * \class ServiceRunner
     */
    template <class TService>
    ServiceRunner<TService>::ServiceRunner(int argc, char **argv) : Runner(argc, argv) {
    }

    template <class TService>
    int ServiceRunner<TService>::run() {
        return Runner::run(
            [this]() {
                auto const guard = core::make_scope_exit([]() { TService::getInstance().disconnect(); });

                auto const settings = getData<ServiceSettings>();
                auto const input    = settings.serviceInput.value_or(
                    ServiceSettingsInput{ {}, data::Uri("ws://127.0.0.1:8080"), data::IMessaging::DEFAULT_REALM });

                TheServiceConfiguration.setBrokerUri(input.brokerUrl);
                TheServiceConfiguration.setRealm(input.realm);
                TService::getInstance().connect();
                TService::getInstance().run();
                return 0;
            },
            []() { TService::getInstance().stop(); });
    }

} // namespace NS_OSBASE::application
