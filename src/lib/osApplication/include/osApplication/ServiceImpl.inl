/// \brief Implementation of a service

#pragma once
#include "osData/ExchangeMaker.h"
#include "osCore/Exception/Check.h"

namespace cho::osbase::application {

    namespace internal {
        template <typename T>
        struct deduce_tuple_from_callback;

        template <typename TRet, typename TClass, typename... TArgs>
        struct deduce_tuple_from_callback<TRet (TClass::*)(TArgs...)> {
            using arg_type    = std::tuple<TArgs...>;
            using return_type = TRet;
        };

        template <typename TRet, typename TClass, typename... TArgs>
        struct deduce_tuple_from_callback<TRet (TClass::*)(TArgs...) const> {
            using arg_type    = std::tuple<TArgs...>;
            using return_type = TRet;
        };

        template <typename TRet, typename TClass>
        struct deduce_tuple_from_callback<TRet (TClass::*)()> {
            using arg_type    = void;
            using return_type = TRet;
        };

        template <typename TRet, typename TClass>
        struct deduce_tuple_from_callback<TRet (TClass::*)() const> {
            using arg_type    = void;
            using return_type = TRet;
        };
    } // namespace internal

    /*
     * \class ServiceImpl::SupplierDelegate
     */
    template <typename TService>
    class ServiceImpl<TService>::SupplierDelegate : public data::IMessaging::ISupplierDelegate, public data::IMessaging::IErrorDelegate {};

    /*
     * \class ServiceImpl::SupplierDelegate
     */
    template <typename TService>
    template <typename TClass, typename TMethodCallback>
    class ServiceImpl<TService>::TSupplierDelegate : public SupplierDelegate {

        using arg_type    = typename internal::deduce_tuple_from_callback<TMethodCallback>::arg_type;
        using return_type = typename internal::deduce_tuple_from_callback<TMethodCallback>::return_type;

    public:
        TSupplierDelegate(ServiceImpl &serviceImpl, TClass *pInstance, TMethodCallback mth)
            : m_service(serviceImpl), m_pInstance(pInstance), m_mth(mth) {
        }

        std::string onCall(const data::IMessaging::JsonText &jsonArgs) override {
            try {
                return oscheck::throwIfCrashOrReturn<std::string>([&jsonArgs, mth = m_mth, pInstance = m_pInstance, this]() {
                    auto const guard = core::make_scope_exit([this]() { m_service.resumeAliveNotification(); });
                    m_service.suspendAliveNotification();
                    m_service.publishAliveMessage(std::chrono::milliseconds(0));
                    auto const pResultKeyStream = core::makeJsonStream();
                    std::ostringstream oss;

                    if constexpr (std::is_void_v<arg_type>) {
                        if constexpr (std::is_void_v<return_type>) {
                            std::apply(mth, std::make_tuple(pInstance));
                        } else {
                            return_type result = std::apply(mth, std::make_tuple(pInstance));
                            pResultKeyStream->setValue(result);
                        }
                    } else {
                        auto const pKeyStream = core::makeJsonStream(std::istringstream(jsonArgs));
                        auto value            = pKeyStream->getValue(arg_type{});

                        if constexpr (std::is_void_v<return_type>) {
                            std::apply(mth, std::tuple_cat(std::make_tuple(pInstance), std::move(value)));
                        } else {
                            return_type result = std::apply(mth, std::tuple_cat(std::make_tuple(pInstance), std::move(value)));
                            pResultKeyStream->setValue(result);
                        }
                    }

                    oss << *pResultKeyStream;
                    return oss.str();
                });
            } catch (const core::RuntimeException &e) {
                m_service.publish(m_service.makeFullUri(m_service.s_serviceRuntimeErrorTopic), RuntimeErrorData{ e.what() });
                throw e;
            }
            return {};
        }

        void onError(const std::string &error) override {
            m_service.getTaskLoop()->pushImmediate([error]() { throw ServiceException(error); });
        }

    private:
        ServiceImpl &m_service;
        TClass *m_pInstance;
        TMethodCallback m_mth;
    };

    /*
     * \class ServiceImpl::PublishError
     */
    template <typename TService>
    class ServiceImpl<TService>::PublishError : public data::IMessaging::IErrorDelegate {
    public:
        PublishError(ServiceImpl &serviceImpl) : m_serviceImpl(serviceImpl) {
        }

        void onError(const std::string &error) override {
            m_serviceImpl.getTaskLoop()->pushImmediate([error]() { throw ServiceException(error); });
        }

    private:
        ServiceImpl &m_serviceImpl;
    };

    /*
     * \class ServiceImpl
     */
    template <typename TService>
    ServiceImpl<TService>::ServiceImpl(const std::string &serviceName) : ServiceBase<TService>(serviceName, std::make_shared<TaskLoop>()) {
        m_pPublishErrorDelegate = std::make_shared<PublishError>(*this);
    }

    template <typename TService>
    void ServiceImpl<TService>::connect(const std::string &url, const unsigned short port) {
        try {
            ServiceBase<TService>::connect(url, port);
        } catch (const ServiceException &) {
            return; // Failed connection is not an error, the method onMessagingConnection will be callesd on (re-)connection
        }

        publishReadyMessage();
        resumeAliveNotification();
        notify(ServiceConnectionMsg{ true });
    }

    template <typename TService>
    void ServiceImpl<TService>::disconnect() {
        auto const guard = core::make_scope_exit([this]() { m_pSupplierDelegates.clear(); });

        suspendAliveNotification();
        if (m_pTaskAlive != nullptr) {
            m_pTaskAlive->setEnabled(false);
            m_pTaskAlive.reset();
        }
        ServiceBase<TService>::disconnect();
    }

    template <typename TService>
    void ServiceImpl<TService>::run() {
        getTaskLoop()->run();
    }

    template <typename TService>
    std::shared_future<void> ServiceImpl<TService>::runAsync() {
        return getTaskLoop()->runAsync();
    }

    template <typename TService>
    void ServiceImpl<TService>::stop() {
        getTaskLoop()->stop();
    }

    template <typename TService>
    unsigned long long ServiceImpl<TService>::getAlivePeriod() const {
        return m_alivePeriod.count();
    }

    template <typename TService>
    void ServiceImpl<TService>::setAlivePeriod(const unsigned long long period) {
        m_alivePeriod = std::chrono::milliseconds(period);
    }

    template <typename TService>
    void ServiceImpl<TService>::doRegister() {
        registerCall(makeFullUri(s_serviceGetAlivePeriodUri), this, &ServiceImpl<TService>::getAlivePeriod);
    }

    template <typename TService>
    void ServiceImpl<TService>::doUnregister() {
        unregisterCall(makeFullUri(s_serviceGetAlivePeriodUri));
    }

    template <typename TService>
    template <typename TClass, typename TMethodCallback>
    void ServiceImpl<TService>::registerCall(const std::string &uri, TClass *pInstance, TMethodCallback &&methodCallback) {
        auto pDelegate = std::make_shared<TSupplierDelegate<TClass, TMethodCallback>>(*this, pInstance, methodCallback);
        m_pSupplierDelegates.insert(std::make_pair(uri, pDelegate));
        getMessaging()->registerCall(uri, pDelegate, pDelegate);
    }

    template <typename TService>
    void ServiceImpl<TService>::unregisterCall(const std::string &uri) {
        auto const itDelegate = m_pSupplierDelegates.find(uri);
        if (itDelegate == m_pSupplierDelegates.cend()) {
            throw ServiceException("'" + uri + "' has not been registered!");
        }

        auto const guard = core::make_scope_exit([this, &itDelegate]() { m_pSupplierDelegates.erase(itDelegate); });
        auto pDelegate   = std::dynamic_pointer_cast<SupplierDelegate>(itDelegate->second);
        getMessaging()->unregisterCall(uri, pDelegate);
    }

    template <typename TService>
    template <typename TMessage>
    void ServiceImpl<TService>::publish(const std::string &topic, const TMessage &message) const {
        publishMessage(topic, message);
        resetAliveNotification();
    }

    template <typename TService>
    void ServiceImpl<TService>::onMessagingConnection(const bool bConnected) {
        if (bConnected) {
            m_pSupplierDelegates.clear();
        }

        ServiceBase<TService>::onMessagingConnection(bConnected);

        {
            std::lock_guard lock(m_mutexAlive);
            m_refCall = 0;
        }

        if (bConnected) {
            resumeAliveNotification();
        } else {
            suspendAliveNotification();
        }
    }

    template <typename TService>
    void ServiceImpl<TService>::publishReadyMessage() const {
        publishMessage(makeFullUri(s_serviceNotifyReadyTopic), m_alivePeriod.count());
    }

    template <typename TService>
    void ServiceImpl<TService>::publishAliveMessage(const std::chrono::milliseconds &alivePeriod) const {
        try {
            publishMessage(makeFullUri(s_serviceNotifyAliveTopic), alivePeriod.count());
        } catch (const data::MessagingException &) {
        } // doesn't propagate any exception, broker deconnection handled by another mehcanism
    }

    template <typename TService>
    template <typename TMessage>
    void ServiceImpl<TService>::publishMessage(const std::string &topic, const TMessage &message) const {
        const std::string serializedTopic = [&message]() {
            auto const pKeyStream = core::makeJsonStream();
            pKeyStream->setValue(message);

            std::ostringstream oss;
            oss << *pKeyStream;
            return oss.str();
        }();

        getMessaging()->publish(topic, serializedTopic, m_pPublishErrorDelegate);
    }

    template <typename TService>
    void ServiceImpl<TService>::suspendAliveNotification() const {
        std::lock_guard lock(m_mutexAlive);

        if (m_refCall != 0) {
            --m_refCall;
        }

        if (m_refCall == 0 && m_pTaskAlive != nullptr) {
            m_pTaskAlive->setEnabled(false);
            m_pTaskAlive.reset();
        }
    }

    template <typename TService>
    void ServiceImpl<TService>::resumeAliveNotification() const {
        std::lock_guard lock(m_mutexAlive);

        if (m_refCall == 0) {
            m_pTaskAlive = getTaskLoop()->pushRepeated(m_alivePeriod, [this]() { publishAliveMessage(m_alivePeriod); });
        }
        ++m_refCall;
    }

    template <typename TService>
    void ServiceImpl<TService>::resetAliveNotification() const {
        suspendAliveNotification();
        resumeAliveNotification();
    }
} // namespace cho::osbase::application
