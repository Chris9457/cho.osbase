// \brief Declaration of the class Runner

#pragma once
#include "osApplication/ServiceCommandParser.h"
#include "osCore/Exception/RuntimeException.h"
#include "osData/AsyncData.h"
#include <future>
#include <chrono>

namespace NS_OSBASE::application {

    /**
     * \brief Exception thrown by runners
     */
    class RunnerException : public core::RuntimeException {
        using RuntimeException::RuntimeException;
    };

    /**
     * \brief Base class for runners
     */
    class Runner {

    public:
        using TCallbackRun  = std::function<int()>;
        using TCallbackStop = std::function<void()>;

        Runner(int argc, char **argv);
        ~Runner();

        int run(TCallbackRun &&onRun, TCallbackStop &&onStop);

        const ServiceOptions &getOptions() const;

        const std::chrono::milliseconds &getTimeout() const;
        void setTimeout(const std::chrono::milliseconds &timeout);

        template <typename T>
        void sendData(const T &data);

    private:
        void onData(std::string &&data);

        data::AsyncData<std::string> m_data;
        ServiceOptions m_options;
        TCallbackRun m_onRun;
        TCallbackStop m_onStop;
        std::chrono::milliseconds m_timeout;

        static constexpr std::chrono::milliseconds s_defaultTimeout = std::chrono::milliseconds(10);
    };
} // namespace NS_OSBASE::application

#include "Runner.inl"
