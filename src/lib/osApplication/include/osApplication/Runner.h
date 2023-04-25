// \brief Declaration of the class Runner

#pragma once
#include "osApplication/ServiceCommandParser.h"
#include "osApplication/ServiceHandshake.h"
#include "osCore/Exception/RuntimeException.h"
#include <memory>
#include <future>
#include <chrono>

namespace NS_OSBASE::application {

    /**
     * çbrief Exception thrown by runners
     */
    class RunnerException : public core::RuntimeException {
        using RuntimeException::RuntimeException;
    };

    /**
     * \brief Base class for runners
     */
    class Runner {

    public:
        using TCallback = std::function<void()>;

        Runner(int argc, char **argv);
        ~Runner();

        int run(TCallback &&onRun, TCallback &&onStop);
        void stop();

        std::string getBrokerUrl() const;
        unsigned short getBrokerPort() const;

        const std::chrono::milliseconds &getTimeout() const;
        void setTimeout(const std::chrono::milliseconds &timeout);

    protected:
        const ServiceOptions &getOptions() const;

    private:
        class HandshakeDelegate;
        using HandshakeDelegatePtr = std::shared_ptr<HandshakeDelegate>;

        void initHandshake();

        ServiceOptions m_options;
        ServiceHandshakePtr m_pHandshake;
        HandshakeDelegatePtr m_pHandshakeDelegate;
        TCallback m_onRun;
        TCallback m_onStop;
        std::chrono::milliseconds m_timeout;

        static constexpr std::chrono::milliseconds s_defaultTimeout = std::chrono::milliseconds(10);
    };
} // namespace NS_OSBASE::application
