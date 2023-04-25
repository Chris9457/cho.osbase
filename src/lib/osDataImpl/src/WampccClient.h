// \brief Declaration of a WAMP client using wampcc SOUP
#pragma once

#include "osData/IMessaging.h"
#include "wampcc/wampcc.h"

#include <mutex>
#include <unordered_map>

namespace NS_OSBASE::data::impl {
    class MessagingWampcc : public IMessaging {
    public:
        MessagingWampcc();
        ~MessagingWampcc() override;

        void connect(const std::string &brokerUrl, int brokerPort) override;
        void disconnect() override;
        void registerCall(const std::string &uri, ISupplierDelegatePtr pDelegate, IErrorDelegatePtr pError) override;
        void unregisterCall(const std::string &uri, IErrorDelegatePtr pError) override;
        void invoke(const std::string &uri,
            const std::string &argsSerialized,
            IClientDelegatePtr pDelegate,
            IErrorDelegatePtr pError) const override;
        void subscribe(const std::string &topic, IEventDelegatePtr pDelegate, IErrorDelegatePtr pError) override;
        void unsubscribe(const std::string &topic, IErrorDelegatePtr pError) override;
        void publish(const std::string &topic, const std::string &argsSerialized, IErrorDelegatePtr pError) const override;

    private:
        enum class States { Idle, Disconnected, DisconnectedCalling, Connected, ConnectedCalling, ConnectedCallingAbort };

        void connect(const std::string &brokerUrl, int brokerPort, const std::string &realm);
        bool tryConnect(const std::string &brokerUrl, int brokerPort, const std::string &realm);
        void doDisconnect();
        wampcc::wamp_session &ensureValidSession() const;

        void retryConnection();

        bool isStateConnected() const;
        void setStateConnected();

        bool isStateDisconnected() const;
        void setStateDisconnected();

        bool isStateIdle() const;

        inline static const std::string m_default_realm = "osbase";
        static constexpr auto s_timeoutRetryConnection  = std::chrono::seconds(1);

        wampcc::config m_wampccConf;
        wampcc::kernel m_kernel;
        std::unique_ptr<wampcc::tcp_socket> m_sock;
        std::shared_ptr<wampcc::wamp_session> m_session;
        mutable std::recursive_mutex m_mutex;
        std::unordered_map<wampcc::t_subscription_id, IEventDelegateWPtr> m_eventDelegates;
        std::unordered_map<std::string, wampcc::t_subscription_id> m_subscribedTopics;
        std::unordered_map<wampcc::t_registration_id, ISupplierDelegateWPtr> m_callDelegates;
        std::unordered_map<std::string, wampcc::t_registration_id> m_registeredCalls;
        std::string m_brokerUrl;
        int m_brokerPort = 8080;
        std::string m_realm;

        std::future<void> m_futConnection;
        std::mutex m_mutConnection;
        std::condition_variable m_cvConnection;
        bool m_bStopRetryConnection = false;
        States m_state              = States::Idle;
    };
} // namespace NS_OSBASE::data::impl
