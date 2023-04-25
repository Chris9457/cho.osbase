/// \brief Common implementation of the ServiceStub and ServiceImp classes

#pragma once
//#include "IService.h"
#include "TaskLoop.h"
#include "osData/IMessaging.h"
#include <type_traits>

namespace NS_OSBASE::application {

    /**
     * \brief Common implementation of the ServiceStub and ServiceImpl classes
     * \tparam TService Interface of the service to implement
     * \ingroup PACKAGE_SERVICE
     */
    template <class TService>
    class ServiceBase : public TService {
    public:
        virtual void connect(const std::string &url, const unsigned short port) override; //!< Connect the service to the broker
        virtual void disconnect() override;                                               //!< Disconnect the service from the broker
        void setCallTimeout(const std::chrono::milliseconds &timeout) override final;     //!< Set the timeout of a RPC invoke

        TaskLoopPtr getTaskLoop() const; //!< Return the associated task loop

    protected:
        struct ReadyMsg : ServiceMsg<unsigned int> {}; //!< Message sent when the service finish its connection
        struct AliveMsg : ServiceMsg<unsigned int> {}; //!< Message priodically sent when the service is alive

        explicit ServiceBase(const std::string &serviceName, TaskLoopPtr pTaskLoop); //!< Ctor

        data::IMessagingPtr getMessaging() const;                //!< Return the Messaging class
        const std::chrono::milliseconds &getCallTimeout() const; //!< Return the timeout of any RPC invoke
        std::string makeFullUri(const std::string &uri) const;   //!< Return the full uri (used fro RPC & PubSub uris)

        virtual void doConnect(const std::string &url, const unsigned short port); //!< Method template called by connect() and performs
                                                                                   //!< extra works needed after the connection
        virtual void doDisconnect(); //!< Method template called by disconnect() and performs extra works needed after the disconnection

        virtual void doRegister() = 0;   //!< Methode template called by connect() to register the client (event subscription) or the server
                                         //!< (RPC registering)
        virtual void doUnregister() = 0; //!< Methode template called by disconnect() to unregister the client (event unsubscription) or the
                                         //!< server (RPC unregistering)

        virtual void onMessagingConnection(const bool bConnected);

        inline static const std::string s_serviceRuntimeErrorTopic = "error.runtime";    //!< \private
        inline static const std::string s_serviceNotifyReadyTopic  = "notify.ready";     //!< \private
        inline static const std::string s_serviceNotifyAliveTopic  = "notify.alive";     //!< \private
        inline static const std::string s_serviceGetAlivePeriodUri = "get.alive.period"; //!< \private

    private:
        class MessagingConnectionObserver;
        using MessagingConnectionObserverPtr = std::shared_ptr<MessagingConnectionObserver>;

        data::IMessagingPtr m_pMessaging;
        MessagingConnectionObserverPtr m_pMessagingConnectionObserver;
        TaskLoopPtr m_pTaskLoop;
        std::chrono::milliseconds m_callTimeout = std::chrono::milliseconds::max();
        std::string m_serviceName;
        std::string m_url;
        unsigned short m_port = 8080;
    };
} // namespace NS_OSBASE::application

#include "ServiceBase.inl"
