#include "WampccBroker.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include "osCore/Misc/Scope.h"
#include "osData/MessagingException.h"
#include <future>
#include <wampcc/wampcc.h>

namespace cho::osbase::data::impl {
    OS_REGISTER_FACTORY_N(IBroker, WampccBroker, 0, WAMPCCBROCKER_FACTORY_NAME);

    void WampccBroker::startWampcc(unsigned short port) {
        {
            std::unique_lock lock(m_mutexStart);
            auto const guard    = core::make_scope_exit([this]() { m_cvStarted.notify_one(); });
            const auto provider = wampcc::auth_provider::no_auth_required();
            auto fut            = m_router.listen(provider, port);

            if (const auto result = fut.get())
                throw MessagingException(result.message());
        }

        std::unique_lock lock(m_mutexStop);
        m_cvStopped.wait(lock);
    }

    void WampccBroker::start(unsigned short port) {
        std::unique_lock lock(m_mutexStart);
        m_thread = std::thread([this](unsigned short p) { this->startWampcc(p); }, port);
        m_cvStarted.wait(lock);
    }

    void WampccBroker::stop() {
        m_cvStopped.notify_one();
        m_thread.join();
    }

} // namespace cho::osbase::data::impl