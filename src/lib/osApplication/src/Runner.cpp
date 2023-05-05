// \brief Helper class to run a Servic// \brief Declaration of the class Runner

#include "osApplication/Runner.h"
#include "osData/IMessaging.h"
#include <iostream>

namespace NS_OSBASE::application {

    /*
     * \class ServiceRunner<void>
     */
    Runner::Runner(int argc, char **argv) : m_options(ServiceCommandParser(argc, argv).getOptions()), m_timeout(s_defaultTimeout) {
    }

    Runner::~Runner() {
    }

    const std::chrono::milliseconds &Runner::getTimeout() const {
        return m_timeout;
    }

    void Runner::setTimeout(const std::chrono::milliseconds &timeout) {
        m_timeout = timeout;
    }

    void Runner::onData(std::string &&data) {
        std::cout << data << std::endl;
        if (m_onStop) {
            m_onStop();
        }
    }

    int Runner::run(TCallbackRun &&onRun, TCallbackStop &&onStop) {
        auto const guard = core::make_scope_exit([this] { m_data.reset(); });
        m_onRun          = std::move(onRun);
        m_onStop         = std::move(onStop);

        if (m_options.dataUrl.has_value()) {
            auto const dataUrl = m_options.dataUrl.value();
            m_data             = data::AsyncData<std::string>(dataUrl);
            m_data.get([this](std::string &&data) { onData(std::move(data)); });
        }

        if (m_onRun) {
            try {
                return m_onRun();
            } catch (const RunnerException &e) {
                std::cout << "Unable to start the runner" << std::endl;
                std::cout << e.what() << std::endl;
            }
        }

        return -1;
    }

    const ServiceOptions &Runner::getOptions() const {
        return m_options;
    }
} // namespace NS_OSBASE::application
