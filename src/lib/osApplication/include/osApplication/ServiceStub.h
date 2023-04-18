/// \brief Common interface for services

#pragma once
#include "ServiceBase.h"
#include "osApplication/TaskLoop.h"
#include "osData/IMessaging.h"
#include <future>
#include <memory>
#include <optional>
#include <set>
#include <type_traits>
#include <vector>
#include <optional>

namespace cho::osbase::application {

    /**
     * \brief Base implementation of the "stub" of the service
     * \tparam TService Interface of the service to implement
     * \ingroup PACKAGE_SERVICE
     */
    template <class TService>
    class ServiceStub : public ServiceBase<TService> {
    public:
        void connect(const std::string &url, const unsigned short port) final; //!< Connect the service to the broker
        void disconnect() final;                                               //!< Disconnect the service from the broker
        unsigned long long getAlivePeriod() const override final;              //!< period of alive message in ms

    protected:
        /**
         * \brief Encapsulate the return result and the error
         * \tparam T   Type of the return code
         */
        template <typename T, bool = std::is_void_v<T>>
        struct Result {
            std::optional<std::string> strError; //!< Human readable error
        };

        /**
         * \copydoc ServiceStub::Result
         */
        template <typename T>
        struct Result<T, false> : Result<void> {
            T result = {}; //!< Return value
        };

        ServiceStub(const std::string &serviceName, TaskLoopPtr pTaskLoop); //!< Ctor

        /**
         * \brief Invoke synchroneously the service process related to the URI
         * \tparam TRet     Type of the return value
         * \tparam TArgs    Types of the arguments
         * \param uri       URI of the call
         * \param args      Arguments of the invocation
         * \return          Result of the invocation
         *
         * \throws          ServiceException in case of timeout or messaging error
         */
        template <typename TRet, typename... TArgs>
        TRet invoke(const std::string &uri, TArgs &&...args);

        template <typename TRet, typename... TArgs>
        TRet invoke(const std::string &uri, TArgs &&...args) const; //!< \copydoc ServiceStub::invoke()

        /**
         * \brief Invoke synchroneously the service process related to the URI
         * \tparam TRet     Type of the return value
         * \tparam TArgs    Types of the arguments
         * \param uri       URI of the call
         * \param args      Arguments of the invocation
         * \return          The future result of the invocation
         */
        template <typename TRet, typename... TArgs>
        std::future<Result<TRet>> invokeAsync(const std::string &uri, TArgs &&...args);

        template <typename TRet, typename... TArgs>
        std::future<Result<TRet>> invokeAsync(const std::string &uri, TArgs &&...args) const; //!< \copydoc ServiceStub::invokeAsync()

        /**
         * \brief Subscribe the service stub to the topic (event) published by the ServiceImpl side
         * \tparam TMessage Type of the expected message
         * \param topic Name (URI) of the event
         */
        template <typename TMessage>
        void subscribe(const std::string &topic);

        /**
         * \brief Unsubscribe the service stub to the topic (event) published by the ServiceImpl side
         * \tparam TMessage Type of the expected message
         * \param topic Name (URI) of the event
         */
        template <typename TMessage>
        void unsubscribe(const std::string &topic);

        void doRegister() override;
        void doUnregister() override;

        void onMessagingConnection(const bool bConnected) override;

    private:
        template <typename TRet>
        class ClientDelegate;

        template <class TMessage>
        class EventDelegate;

        template <typename TRet>
        auto makeClientDelegate();

        template <typename TRet>
        auto makeClientDelegate() const;

        void removeClientDelegate(data::IMessaging::IClientDelegatePtr pClientDelegate);

        void listenAliveMessage(const std::chrono::milliseconds &timeout);
        void stopListenAliveMessage();
        void resetListenAliveMessage(const std::chrono::milliseconds &timeout);
        bool isListeningAliveMessage() const;
        void onConnected(const bool bConnected, const bool bQueued);

        mutable std::set<data::IMessaging::IClientDelegatePtr> m_pClientDelegates;
        mutable std::unordered_map<std::string, data::IMessaging::IEventDelegatePtr> m_pEventDelegates;

        mutable std::mutex m_mutex;

        bool m_hasTaskLoopOwnerShip = false;
        std::shared_future<void> m_futTaskLoop;
        ITaskPtr m_pTaskAlive;
        size_t m_refCall = 0;
        std::chrono::milliseconds m_lastAliveTimeout;

        static constexpr int s_factorAlivePeriod = 2;
    };
} // namespace cho::osbase::application

#include "ServiceStub.inl"
