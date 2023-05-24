#include "WampccBroker.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include "osCore/Misc/Scope.h"
#include "osData/MessagingException.h"
#include <future>
#include <wampcc/wampcc.h>

namespace NS_OSBASE::data::impl {
    OS_REGISTER_FACTORY_N(IBroker, WampccBroker, 0, WAMPCCBROCKER_FACTORY_NAME);

    void WampccBroker::startWampcc(unsigned short port) {
        {
            std::unique_lock lock(m_mutexStart);
            auto const guard    = core::make_scope_exit([this]() { m_cvStarted.notify_one(); });
            const auto provider = wampcc::auth_provider::no_auth_required();
            auto fut            = m_pRouter->listen(provider, port);

            if (const auto result = fut.get()) {
                throw MessagingException(result.message());
            }
        }

        std::unique_lock lock(m_mutexStop);
        m_cvStopped.wait(lock);
    }

    WampccBroker::WampccBroker() {
        m_pRouter = std::make_shared<wampcc::wamp_router>(&m_kernel);
    }

    unsigned short WampccBroker::start(const unsigned short port) {
        std::unique_lock lock(m_mutexStart);
        m_thread = std::thread([this, port] { startWampcc(port); });
        m_cvStarted.wait(lock);

        auto const addresses = m_pRouter->get_listen_addresses();
        if (addresses.size() == 1) {
            return static_cast<unsigned short>(addresses[0].port());
        }

        return port;
    }

    void WampccBroker::stop() {
        m_cvStopped.notify_one();
        m_thread.join();
    }

} // namespace NS_OSBASE::data::impl
