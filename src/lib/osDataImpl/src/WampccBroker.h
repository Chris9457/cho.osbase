#pragma once

#include "osData/IBroker.h"

#include <future>
#include <mutex>
#include <thread>
#include <wampcc/wampcc.h>

namespace NS_OSBASE::data::impl {
    /**
     * Implementation of the Wampcc broker
     */
    class WampccBroker : public IBroker {
    public:
        WampccBroker() : m_kernel(), m_router(&m_kernel), m_runPromise(){};
        ~WampccBroker() override = default;

        void start(unsigned short port) override;
        void stop() override;

    private:
        void startWampcc(unsigned short port);

        std::mutex m_mutexStart;
        std::mutex m_mutexStop;
        std::thread m_thread;
        wampcc::kernel m_kernel;
        wampcc::wamp_router m_router;
        std::promise<void> m_runPromise;
        std::condition_variable m_cvStarted;
        std::condition_variable m_cvStopped;
    };
} // namespace NS_OSBASE::data::impl