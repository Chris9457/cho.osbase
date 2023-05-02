/// \Brief Realization of the interface IServiceBase

#pragma once
#include "ServiceException.h"
#include "osData/MessagingException.h"

namespace NS_OSBASE::application {

    /*
     * \class ServiceBase::MessagingConnectionObserver
     */
    template <class TService>
    class ServiceBase<TService>::MessagingConnectionObserver : public core::Observer<data::IMessaging::MessagingConnectionMsg> {
    public:
        MessagingConnectionObserver(ServiceBase<TService> &serviceBase) : m_serviceBase(serviceBase) {
        }

        void update(const core::Observable &, const data::IMessaging::MessagingConnectionMsg &msg) override {
            m_serviceBase.onMessagingConnection(msg.isConnected());
        }

    private:
        ServiceBase<TService> &m_serviceBase;
    };

    /*
     * \class ServiceBase
     */
    template <class TService>
    ServiceBase<TService>::ServiceBase(
        const std::string &serviceName, const data::Uri &uri, const std::string &realm, TaskLoopPtr pTaskLoop)
        : m_pTaskLoop(pTaskLoop),
          m_pMessagingConnectionObserver(std::make_shared<MessagingConnectionObserver>(*this)),
          m_serviceName(serviceName),
          m_brokerUri(uri),
          m_realm(realm) {
    }

    template <class TService>
    void ServiceBase<TService>::connect() {
        auto const guard = core::make_scope_exit([this]() { m_pMessaging->attachAll(*m_pMessagingConnectionObserver); });

        m_pMessaging = data::makeWampMessaging(m_brokerUri, m_realm);
        try {
            m_pMessaging->connect();
            doRegister();
            doConnect();
        } catch (const data::MessagingException &e) {
            throw ServiceException(e.what());
        }
    }

    template <class TService>
    void ServiceBase<TService>::disconnect() {
        auto const guard = core::make_scope_exit([this]() {
            m_pMessaging->detachAll(*m_pMessagingConnectionObserver);
            m_pMessaging->disconnect();
            m_pMessaging.reset();
        });

        try {
            doDisconnect();
            doUnregister();
        } catch (const data::MessagingException &e) {
            throw ServiceException(e.what());
        }
    }

    template <class TService>
    void ServiceBase<TService>::setCallTimeout(const std::chrono::milliseconds &timeout) {
        m_callTimeout = timeout;
    }

    template <class TService>
    TaskLoopPtr ServiceBase<TService>::getTaskLoop() const {
        return m_pTaskLoop;
    }

    template <class TService>
    data::IMessagingPtr ServiceBase<TService>::getMessaging() const {
        return m_pMessaging;
    }

    template <class TService>
    const std::chrono::milliseconds &ServiceBase<TService>::getCallTimeout() const {
        return m_callTimeout;
    }

    template <class TService>
    std::string ServiceBase<TService>::makeFullUri(const std::string &uri) const {
        return m_serviceName + "." + uri;
    }

    template <class TService>
    const data::Uri &ServiceBase<TService>::getBrokerUri() const {
        return m_brokerUri;
    }

    template <class TService>
    const std::string &ServiceBase<TService>::getRealm() const {
        return m_realm;
    }

    template <class TService>
    void ServiceBase<TService>::doConnect() {
    }

    template <class TService>
    void ServiceBase<TService>::doDisconnect() {
    }

    template <class TService>
    void ServiceBase<TService>::onMessagingConnection(const bool bConnected) {
        if (bConnected) {
            doRegister();
            doConnect();
        } else {
            doDisconnect();
        }

        notify(ServiceConnectionMsg{ bConnected });
    }

} // namespace NS_OSBASE::application
