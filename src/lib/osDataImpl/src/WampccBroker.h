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
        WampccBroker();
        ;
        ~WampccBroker() override = default;

        unsigned short start(const unsigned short port) override;
        void stop() override;

    private:
        void startWampcc(unsigned short port);

        std::mutex m_mutexStart;
        std::mutex m_mutexStop;
        std::thread m_thread;
        wampcc::kernel m_kernel;
        std::shared_ptr<wampcc::wamp_router> m_pRouter;
        std::condition_variable m_cvStarted;
        std::condition_variable m_cvStopped;
    };
} // namespace NS_OSBASE::data::impl
