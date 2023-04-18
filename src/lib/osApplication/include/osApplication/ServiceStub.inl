/// \brief	Implementation of the service stub

#pragma once
#include "osCore/Serialization/KeyStream.h"
#include "osData/IMessaging.h"
#ifdef CHO_OSBASE_APPLICATION_SERVICE_TRACE
#include "osData/Log.h"
#endif
#include <sstream>

namespace cho::osbase::application {

    /*
     * \class ServiceStub::ClientDelegate
     */
    template <class TService>
    template <typename TRet>
    class ServiceStub<TService>::ClientDelegate : public data::IMessaging::IClientDelegate,
                                                  public data::IMessaging::IErrorDelegate,
                                                  public std::enable_shared_from_this<ClientDelegate<TRet>> {
    public:
        ClientDelegate(ServiceStub<TService> &serviceStub) : m_serviceStub(serviceStub) {
        }

        void onResult(const std::string &json) override {
#ifdef CHO_OSBASE_APPLICATION_SERVICE_TRACE
            oslog::trace(data::OS_LOG_CHANNEL_APPLICATION) << "Result json : '" << json << "'" << oslog::end();
#endif

            if constexpr (std::is_void_v<TRet>) {
                m_result.set_value(Result<void>{});
            } else {
                auto const pKeyStream = core::makeJsonStream(std::stringstream(json));
                auto value            = pKeyStream->getValue(TRet{});
                m_result.set_value(Result<TRet>{ {}, std::move(value) });
            }
            m_serviceStub.removeClientDelegate(shared_from_this());
        }

        void onError(const std::string &error) override {
            if constexpr (std::is_void_v<TRet>) {
                m_result.set_value(Result<void>{ error });
            } else {
                m_result.set_value(Result<TRet>{ error, TRet{} });
            }
            m_serviceStub.removeClientDelegate(shared_from_this());
        }

        auto getFutureResult() {
            return m_result.get_future();
        }

    private:
        ServiceStub<TService> &m_serviceStub;
        std::promise<Result<TRet>> m_result;
    };

    /*
     * \class ServiceStub::EventDelegate
     */
    template <class TService>
    template <typename TMessage>
    class ServiceStub<TService>::EventDelegate : public data::IMessaging::IEventDelegate,
                                                 public data::IMessaging::IErrorDelegate,
                                                 public std::enable_shared_from_this<EventDelegate<TMessage>> {
    public:
        EventDelegate(ServiceStub<TService> &serviceStub) : m_serviceStub(serviceStub) {
        }

        void onEvent(const std::string &json) override {
#ifdef CHO_OSBASE_APPLICATION_SERVICE_TRACE
            oslog::trace(data::OS_LOG_CHANNEL_APPLICATION) << "Event json: '" << json << "'" << oslog::end();
#endif
            auto const pKeyStream = core::makeJsonStream(std::stringstream(json));
            auto const message    = pKeyStream->getValue(TMessage::type{});

            if constexpr (std::is_same_v<TMessage, AliveMsg>) {
                if (message == 0) {
                    m_serviceStub.stopListenAliveMessage();
                } else if (m_serviceStub.isListeningAliveMessage()) {
                    m_serviceStub.resetListenAliveMessage(std::chrono::milliseconds(message));
                } else {
                    m_serviceStub.listenAliveMessage(std::chrono::milliseconds(message));
                }
            } else if constexpr (std::is_same_v<TMessage, ReadyMsg>) {
                if (m_serviceStub.isListeningAliveMessage()) {
                    m_serviceStub.stopListenAliveMessage();
                    m_serviceStub.onConnected(false, true);
                }

                m_serviceStub.onConnected(true, true);
                m_serviceStub.listenAliveMessage(std::chrono::milliseconds(message));
            } else {
                if (m_serviceStub.isListeningAliveMessage()) {
                    m_serviceStub.resetListenAliveMessage(m_serviceStub.m_lastAliveTimeout);
                }
                m_serviceStub.getTaskLoop()->push([this, message]() { m_serviceStub.notify(TMessage{ message }); });
            }
        }

        void onError(const std::string &errorMsg) override {
            m_serviceStub.getTaskLoop()->push([this, errorMsg]() { m_serviceStub.notify(RuntimeErrorMsg{ errorMsg }); });
        }

        auto getFutureResult() {
            return m_result.get_future();
        }

    private:
        ServiceStub<TService> &m_serviceStub;
    };

    /*
     * \class ServiceStub
     */
    template <class TService>
    ServiceStub<TService>::ServiceStub(const std::string &serviceName, TaskLoopPtr pTaskLoop)
        : ServiceBase<TService>(serviceName, pTaskLoop != nullptr ? pTaskLoop : std::make_shared<TaskLoop>()),
          m_hasTaskLoopOwnerShip(pTaskLoop == nullptr) {
    }

    template <class TService>
    void ServiceStub<TService>::connect(const std::string &url, const unsigned short port) {
        if (m_hasTaskLoopOwnerShip) {
            m_futTaskLoop = getTaskLoop()->runAsync();
        }

        try {
            ServiceBase<TService>::connect(url, port);
        } catch (const ServiceException &) {
            return; // Failed connection is not an error, the method onMessagingConnection will be callesd on (re-)connection
        }

        auto timeoutAliveMsg = std::chrono::milliseconds(0);

        try {
            timeoutAliveMsg = std::chrono::milliseconds(getAlivePeriod());
#ifdef CHO_OSBASE_APPLICATION_SERVICE_TRACE
        } catch (const ServiceException &e) {
            oslog::warning() << "ServiceStub::connect: " << e.what() << oslog::end();
            return;
        }
#else
        } catch (const ServiceException &) {
            return;
        }
#endif
        onConnected(true, false);
        listenAliveMessage(timeoutAliveMsg);
    }

    template <class TService>
    void ServiceStub<TService>::disconnect() {
        auto const guard = core::make_scope_exit([this]() {
            if (m_hasTaskLoopOwnerShip) {
                getTaskLoop()->stop();
                m_futTaskLoop.wait();
            }
        });

        stopListenAliveMessage();
        ServiceBase<TService>::disconnect();
    }

    template <class TService>
    unsigned long long ServiceStub<TService>::getAlivePeriod() const {
        return invoke<unsigned long long>(makeFullUri(s_serviceGetAlivePeriodUri));
    }

    template <class TService>
    void ServiceStub<TService>::doRegister() {
        subscribe<RuntimeErrorMsg>(makeFullUri(s_serviceRuntimeErrorTopic));
        subscribe<ReadyMsg>(makeFullUri(s_serviceNotifyReadyTopic));
        subscribe<AliveMsg>(makeFullUri(s_serviceNotifyAliveTopic));
    }

    template <class TService>
    void ServiceStub<TService>::doUnregister() {
        unsubscribe<AliveMsg>(makeFullUri(s_serviceNotifyAliveTopic));
        unsubscribe<ReadyMsg>(makeFullUri(s_serviceNotifyReadyTopic));
        unsubscribe<RuntimeErrorMsg>(makeFullUri(s_serviceRuntimeErrorTopic));
    }

    template <class TService>
    template <typename TRet>
    auto ServiceStub<TService>::makeClientDelegate() {
        auto pClientDelegate = std::make_shared<ClientDelegate<TRet>>(*this);
        m_pClientDelegates.insert(pClientDelegate);
        return pClientDelegate;
    }

    template <class TService>
    template <typename TRet>
    auto ServiceStub<TService>::makeClientDelegate() const {
        auto pClientDelegate = std::make_shared<ClientDelegate<TRet>>(*this);
        m_pClientDelegates.insert(pClientDelegate);
        return pClientDelegate;
    }

    template <class TService>
    template <typename TRet, typename... TArgs>
    TRet ServiceStub<TService>::invoke(const std::string &uri, TArgs &&...args) {
        auto asyncResult = invokeAsync<TRet>(uri, std::forward<TArgs>(args)...);

        if (asyncResult.wait_for(this->getCallTimeout()) == std::future_status::timeout) {
            throw ServiceException("invoke timeout!");
        }

        auto result = asyncResult.get();
        if (result.strError) {
            throw ServiceException(*result.strError);
        }

        if constexpr (!std::is_void_v<TRet>) {
            return std::move(result.result);
        }
    }

    template <class TService>
    template <typename TRet, typename... TArgs>
    TRet ServiceStub<TService>::invoke(const std::string &uri, TArgs &&...args) const {
        auto &self = const_cast<ServiceStub &>(*this);
        return self.invoke<TRet>(uri, std::forward<TArgs>(args)...);
    }

    template <class TService>
    template <typename TRet, typename... TArgs>
    std::future<typename ServiceStub<TService>::Result<TRet>> ServiceStub<TService>::invokeAsync(const std::string &uri, TArgs &&...args) {

        // serialize parameters
        auto const serializedParams = [&args...]() {
            auto const pKeyStream = core::makeJsonStream();
            pKeyStream->setValue(std::make_tuple(std::forward<TArgs>(args)...));
            std::ostringstream oss;
            oss << *pKeyStream;
            return oss.str();
        }();

        // invoke the call
#ifdef CHO_OSBASE_APPLICATION_SERVICE_TRACE
        oslog::trace(data::OS_LOG_CHANNEL_APPLICATION) << "Call json : '" << serializedParams << "'" << oslog::end();
#endif
        auto const pClientDelegate = makeClientDelegate<TRet>();
        getMessaging()->invoke(uri, serializedParams, pClientDelegate, pClientDelegate);
        return pClientDelegate->getFutureResult();
    }

    template <class TService>
    template <typename TRet, typename... TArgs>
    std::future<typename ServiceStub<TService>::Result<TRet>> ServiceStub<TService>::invokeAsync(
        const std::string &uri, TArgs &&...args) const {
        auto &self = const_cast<ServiceStub &>(*this);
        return self.invokeAsync<TRet>(uri, std::forward<TArgs>(args)...);
    }

    template <class TService>
    template <typename TMessage>
    void ServiceStub<TService>::subscribe(const std::string &topic) {
        auto pDelegate = std::make_shared<EventDelegate<TMessage>>(*this);
        m_pEventDelegates.insert(std::make_pair(topic, pDelegate));

        getMessaging()->subscribe(topic, pDelegate, pDelegate);
    }

    template <class TService>
    template <typename TMessage>
    void ServiceStub<TService>::unsubscribe(const std::string &topic) {
        auto const itEventDelegate = m_pEventDelegates.find(topic);
        if (itEventDelegate == m_pEventDelegates.cend()) {
            throw ServiceException("the topic '" + topic + "' has not been subscribed!");
        }

        auto pEventDelegate = std::dynamic_pointer_cast<EventDelegate<TMessage>>(itEventDelegate->second);
        if (pEventDelegate == nullptr) {
            throw ServiceException("bad message type!");
        }

        getMessaging()->unsubscribe(topic, pEventDelegate);
        m_pEventDelegates.erase(itEventDelegate);
    }

    template <class TService>
    void ServiceStub<TService>::removeClientDelegate(data::IMessaging::IClientDelegatePtr pClientDelegate) {
        m_pClientDelegates.erase(pClientDelegate);
    }

    template <class TService>
    void ServiceStub<TService>::listenAliveMessage(const std::chrono::milliseconds &timeout) {
        std::lock_guard lock(m_mutex);

        if (m_refCall == 0) {
            m_lastAliveTimeout = timeout;
            m_pTaskAlive       = getTaskLoop()->pushSingleShot(timeout * s_factorAlivePeriod, [this]() {
                try {
                    // last chance: check if an rpc is reachable
                    auto const alivePeriod = std::chrono::milliseconds(getAlivePeriod());
                    if (!isListeningAliveMessage()) {
                        listenAliveMessage(alivePeriod);
                    }
                } catch (const ServiceException &) {
                    onConnected(false, true);
                    m_pTaskAlive.reset();
                }
            });
        }
        ++m_refCall;
    }

    template <class TService>
    void ServiceStub<TService>::stopListenAliveMessage() {
        std::lock_guard lock(m_mutex);

        if (m_refCall != 0) {
            --m_refCall;
        }

        if (m_refCall == 0 && m_pTaskAlive != nullptr) {
            m_pTaskAlive->setEnabled(false);
            m_pTaskAlive.reset();
        }
    }

    template <class TService>
    bool ServiceStub<TService>::isListeningAliveMessage() const {
        std::lock_guard lock(m_mutex);
        return m_pTaskAlive != nullptr;
    }

    template <class TService>
    void ServiceStub<TService>::resetListenAliveMessage(const std::chrono::milliseconds &timeout) {
        stopListenAliveMessage();
        listenAliveMessage(timeout);
    }

    template <class TService>
    void ServiceStub<TService>::onConnected(const bool bConnected, const bool bQueued) {
        if (bQueued) {
            getTaskLoop()->push([this, bConnected]() { notify(ServiceConnectionMsg{ bConnected }); });
        } else {
            notify(ServiceConnectionMsg{ bConnected });
        }
    }

    template <class TService>
    void ServiceStub<TService>::onMessagingConnection(const bool bConnected) {
        if (bConnected) {
            m_pEventDelegates.clear();
        }

        ServiceBase<TService>::onMessagingConnection(bConnected);

        {
            std::lock_guard lock(m_mutex);
            m_refCall = 0;
        }

        if (bConnected) {
            listenAliveMessage(m_lastAliveTimeout);
        } else {
            stopListenAliveMessage();
        }
    }
} // namespace cho::osbase::application
