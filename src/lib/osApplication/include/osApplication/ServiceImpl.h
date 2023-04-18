/// \brief Implementation of a service

#pragma once
#include "ServiceBase.h"

namespace cho::osbase::application {

    /**
     * \brief Base implementation of the "impl" of the service
     * \tparam TService Interface of the service to implement
     * \ingroup PACKAGE_SERVICE
     */
    template <typename TService>
    class ServiceImpl : public ServiceBase<TService> {
    public:
        void connect(const std::string &url, const unsigned short port) final;
        void disconnect() final;

        void run();                          //!< \copydoc TaskLoop::run()
        std::shared_future<void> runAsync(); //!< \copydoc TaskLoop::runAsync()
        void stop();                         //!< \copydoc TaskLoop::stop()

        unsigned long long getAlivePeriod() const override final; //!< period of alive message in ms
        void setAlivePeriod(const unsigned long long period);

    protected:
        ServiceImpl(const std::string &serviceName); //!< Ctor

        void doRegister() override;
        void doUnregister() override;

        /**
         * \brief Register a method
         * \tparam TClass           Type of the class
         * \tparam TMethodCallback  Type of the method to call
         * \param uri               URI associated to the method
         * \param pInstance         Instance of the class implementing the method
         * \param methodCallback    Address of the method to call
         */
        template <typename TClass, typename TMethodCallback>
        void registerCall(const std::string &uri, TClass *pInstance, TMethodCallback &&methodCallback);

        /**
         * \brief Unregister a method
         * \param uri   URI associated to the method
         */
        void unregisterCall(const std::string &uri);

        /**
         * \brief Publish a message
         * \tparam TMessage Type of the message
         * \param topic     Name / URI of the event
         * \param message   Message to send
         */
        template <typename TMessage>
        void publish(const std::string &topic, const TMessage &message) const;

        void onMessagingConnection(const bool bConnected) override;

    private:
        class SupplierDelegate;

        template <typename TClass, typename TMethodCallback>
        class TSupplierDelegate;

        class PublishError;

        template <typename TMessage>
        void publishMessage(const std::string &topic, const TMessage &message) const;

        void publishReadyMessage() const;
        void publishAliveMessage(const std::chrono::milliseconds &alivePeriod) const;

        void suspendAliveNotification() const;
        void resumeAliveNotification() const;
        void resetAliveNotification() const;

        std::unordered_map<std::string, data::IMessaging::ISupplierDelegatePtr> m_pSupplierDelegates;
        data::IMessaging::IErrorDelegatePtr m_pPublishErrorDelegate;
        std::chrono::milliseconds m_alivePeriod = std::chrono::milliseconds(1000);
        mutable ITaskPtr m_pTaskAlive;
        mutable std::mutex m_mutexAlive;
        mutable size_t m_refCall = 0;
    };
} // namespace cho::osbase::application

#include "ServiceImpl.inl"
