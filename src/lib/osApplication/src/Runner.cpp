// \brief Helper class to run a Servic// \brief Declaration of the class Runner

#include "osApplication/Runner.h"
#include <iostream>

namespace NS_OSBASE::application {

    /*
     * \class Runner::HandshakeDelegate
     */
    class Runner::HandshakeDelegate : public ServiceHandshake::IDelegate {
    public:
        HandshakeDelegate(Runner &serviceRunner) : m_runner(serviceRunner) {
        }

        void onEngage() override {
        }

        void onDisengage() override {
            m_runner.stop();
        }

    private:
        Runner &m_runner;
    };
    /*
     * \class ServiceRunner<void>
     */
    Runner::Runner(int argc, char **argv)
        : m_options(type_cast<ServiceOptions>(ServiceCommandParser(argc, argv))), m_timeout(s_defaultTimeout) {
    }

    Runner::~Runner() {
        if (m_pHandshake != nullptr) {
            m_pHandshake->disengage();
        }
    }

    const std::chrono::milliseconds &Runner::getTimeout() const {
        return m_timeout;
    }

    void Runner::setTimeout(const std::chrono::milliseconds &timeout) {
        m_timeout = timeout;
    }

    const ServiceOptions &Runner::getOptions() const {
        return m_options;
    }

    void Runner::initHandshake() {
        if (m_options.handshakeScheme) {
            m_pHandshake         = ServiceHandshake::create(data::Uri(
                { *m_options.handshakeScheme, data::Uri::Uri::Authority{ {}, *m_options.handshakeHost, *m_options.handshakePort } }));
            m_pHandshakeDelegate = std::make_shared<HandshakeDelegate>(*this);
            m_pHandshake->setDelegate(m_pHandshakeDelegate);
            m_pHandshake->engage(m_timeout);
        }
    }

    int Runner::run(TCallback &&onRun, TCallback &&onStop) {
        m_onRun  = std::move(onRun);
        m_onStop = std::move(onStop);
        initHandshake();

        if (m_onRun) {
            try {
                m_onRun();
            } catch (const RunnerException &e) {
                std::cout << "Unable to start the runner" << std::endl;
                std::cout << e.what() << std::endl;
                return -1;
            }
        }

        return 0;
    }

    void Runner::stop() {
        if (m_onStop) {
            m_onStop();
        }
    }

    std::string Runner::getBrokerUrl() const {
        return getOptions().brokerUrl ? *getOptions().brokerUrl : "127.0.0.1";
    }

    unsigned short Runner::getBrokerPort() const {
        return getOptions().brokerPort ? *getOptions().brokerPort : 8080;
    }

} // namespace NS_OSBASE::application
