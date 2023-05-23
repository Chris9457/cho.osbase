// \brief Helper class to run a Servic// \brief Declaration of the class Runner

#include "osApplication/Runner.h"
#include "osApplication/ServiceSettings.h"
#include "osData/IMessaging.h"
#include <iostream>

namespace NS_OSBASE::application {

    /*
     *\class Runner::ServiceOptionsDelegate
     */
    class Runner::ServiceOptionsDelegate : public ServiceOptions::IDelegate {
    public:
        ServiceOptionsDelegate(Runner &runner) : m_runner(runner) {
        }

        void onOptionsReceived() override {
            m_runner.onOptionsReceived();
        }

    private:
        Runner &m_runner;
    };

    /*
     * \class Runner>
     */
    Runner::Runner(int argc, char **argv)
        : m_options(ServiceOptions(argc, argv)),
          m_timeout(s_defaultTimeout),
          m_pServiceOptionsDelegate(std::make_shared<ServiceOptionsDelegate>(*this)) {
        m_options.setDelegate(m_pServiceOptionsDelegate);
    }

    Runner::~Runner() = default;

    const std::chrono::milliseconds &Runner::getTimeout() const {
        return m_timeout;
    }

    void Runner::setTimeout(const std::chrono::milliseconds &timeout) {
        m_timeout = timeout;
    }

    void Runner::onOptionsReceived() const {
        auto const pStream         = core::makeJsonStream(std::istringstream(m_options.getContent()));
        auto const serviceSettings = pStream->getValue(ServiceSettings{});
        auto const input           = serviceSettings.serviceInput.value_or(ServiceSettingsInput{ false, {}, {} });

        if (input.bStop && m_onStop) {
            m_onStop();
        }
    }

    int Runner::run(TCallbackRun &&onRun, TCallbackStop &&onStop) {
        auto const guard = core::make_scope_exit([this] { m_data.reset(); });
        m_onRun          = std::move(onRun);
        m_onStop         = std::move(onStop);

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
} // namespace NS_OSBASE::application
