/// \Brief Realization of the interface IServiceBase

#pragma once
#include "ServiceException.h"
#include "osData/MessagingException.h"

namespace cho::osbase::application {

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
    ServiceBase<TService>::ServiceBase(const std::string &serviceName, TaskLoopPtr pTaskLoop)
        : m_pTaskLoop(pTaskLoop),
          m_serviceName(serviceName),
          m_pMessagingConnectionObserver(std::make_shared<MessagingConnectionObserver>(*this)) {
    }

    template <class TService>
    void ServiceBase<TService>::connect(const std::string &url, const unsigned short port) {
        auto const guard = core::make_scope_exit([this]() { m_pMessaging->attachAll(*m_pMessagingConnectionObserver); });
        m_url            = url;
        m_port           = port;

        m_pMessaging = data::makeMessaging();
        try {
            m_pMessaging->connect(url, port);
            doRegister();
            doConnect(url, port);
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
    void ServiceBase<TService>::doConnect([[maybe_unused]] const std::string &url, [[maybe_unused]] const unsigned short port) {
    }

    template <class TService>
    void ServiceBase<TService>::doDisconnect() {
    }

    template <class TService>
    void ServiceBase<TService>::onMessagingConnection(const bool bConnected) {
        if (bConnected) {
            doRegister();
            doConnect(m_url, m_port);
        } else {
            doDisconnect();
        }

        notify(ServiceConnectionMsg{ bConnected });
    }

} // namespace cho::osbase::application
