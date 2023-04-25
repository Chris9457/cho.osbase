// \brief Implementation of a WAMP client using wampcc SOUP

#include "WampccClient.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include "osCore/Exception/RuntimeException.h"
#include "osData/MessagingException.h"
#include "osData/Log.h"

#include <chrono>
#include <string>

using namespace std::chrono_literals;
using namespace wampcc;

#define LOGIMPL(X, LEVEL)                                                                                                                  \
    do {                                                                                                                                   \
        if (__logger.wants_level && __logger.write && __logger.wants_level(LEVEL)) {                                                       \
            std::ostringstream __xx_oss;                                                                                                   \
            __xx_oss << X;                                                                                                                 \
            __logger.write(LEVEL, __xx_oss.str(), __FILE__, __LINE__);                                                                     \
        }                                                                                                                                  \
    } while (false)

#define LOG_INFO(X) LOGIMPL(X, wampcc::logger::eInfo)

namespace NS_OSBASE::data::impl {
    OS_REGISTER_FACTORY_N(IMessaging, MessagingWampcc, 0, MESSAGINGWAMPCC_FACTORY_NAME);

    namespace {
        wampcc::logger osLogger() {
            logger logger_p;

            const auto levelMask = logger::levels_upto(logger::eInfo);
            logger_p.wants_level = [levelMask](logger::Level l) { return (l & levelMask) != 0; };

            logger_p.write = [](logger::Level level, const std::string &msg, const char *, int) {
                if ((TheLogger.getChannelMask() & OS_LOG_CHANNEL_DATA) == 0) {
                    return;
                }

                if ((level & logger::eError) != 0) {
                    ::oslog::error(OS_LOG_CHANNEL_DATA) << msg << ::oslog::end();
                } else if ((level & logger::eWarn) != 0) {
                    ::oslog::warning(OS_LOG_CHANNEL_DATA) << msg << ::oslog::end();
                } else if ((level & logger::eInfo) != 0) {
                    ::oslog::info(OS_LOG_CHANNEL_DATA) << msg << ::oslog::end();
                }
            };

            return logger_p;
        }
    } // namespace

    static auto const __logger = osLogger();

    MessagingWampcc::MessagingWampcc() : m_kernel(m_wampccConf, __logger) {
    }

    MessagingWampcc::~MessagingWampcc() /*override*/ {
        if (!m_bStopRetryConnection) {
            std::lock_guard lock(m_mutConnection);
            m_bStopRetryConnection = true;
            m_cvConnection.notify_one();
        }

        if (m_futConnection.valid()) {
            if (m_futConnection.wait_for(s_timeoutRetryConnection * 2) != std::future_status::ready) {
                std::cerr << "MessagingWampcc::~MessagingWampcc: fail to stop the reconnection thread!" << std::endl;
            }
        }

        doDisconnect();
    }

    void MessagingWampcc::connect(const std::string &brokerUrl, int brokerPort) {
        connect(brokerUrl, brokerPort, m_default_realm);
    }

    void MessagingWampcc::connect(const std::string &brokerUrl, int brokerPort, const std::string &realm) {
        using namespace std::chrono_literals;
        static constexpr auto helloTimeout = 4s;

        m_session.reset();
        m_brokerUrl  = brokerUrl;
        m_brokerPort = brokerPort;
        m_realm      = realm;

        auto sock                  = std::make_unique<wampcc::tcp_socket>(&m_kernel);
        const auto connectionError = sock->connect(brokerUrl, brokerPort).get();
        if (connectionError != 0) {
            if (isStateConnected() || isStateIdle()) {
                setStateDisconnected();
                m_futConnection = std::async([this]() { retryConnection(); });
            }
            throw MessagingException("Could not connect to socket " + std::string(connectionError.message()));
        }

        m_session = wamp_session::create<websocket_protocol>(&m_kernel, std::move(sock), [this](wamp_session &, bool is_open) {
            if (!is_open && isStateConnected()) {
                setStateDisconnected();
                m_futConnection = std::async([this]() { retryConnection(); });
            }
        });

        if (m_session->hello(realm).wait_for(helloTimeout) != std::future_status::ready) {
            throw MessagingException("Realm logon failed");
        }
        if (!m_session->is_open()) {
            throw MessagingException("Realm logon failed");
        }

        setStateConnected();
    }

    void MessagingWampcc::disconnect() {
        doDisconnect();
    }

    void MessagingWampcc::registerCall(const std::string &uri, ISupplierDelegatePtr pDelegate, IErrorDelegatePtr pError) /*override*/ {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);

        if (m_registeredCalls.find(uri) != m_registeredCalls.end())
            throw MessagingException("The uri " + uri + " has already been registered");
        auto &session = ensureValidSession();
        session.provide(
            uri,
            {},
            [this, pDelegate, uri, pwErrorDelegate = IErrorDelegateWPtr(pError)](wamp_session &, const registered_info &info) {
                std::lock_guard<std::recursive_mutex> guard(m_mutex);

                if (info.was_error) {
                    auto const pErrorDelegate = pwErrorDelegate.lock();
                    if (pErrorDelegate != nullptr)
                        pErrorDelegate->onError("Procedure registration failed, error " + info.error_uri);
                    return;
                }

                LOG_INFO("Procedure registered with id: " << info.registration_id);
                m_callDelegates[info.registration_id] = pDelegate;
                m_registeredCalls[uri]                = info.registration_id;
            },
            [this](wamp_session &ws, invocation_info info) {
                std::lock_guard<std::recursive_mutex> guard(m_mutex);

                if (const auto itDelegates = m_callDelegates.find(info.registration_id); itDelegates != m_callDelegates.cend()) {
                    const ISupplierDelegatePtr pDelegate = itDelegates->second.lock();
                    if (pDelegate != nullptr) {
                        if (info.args.args_list.size() <= 1) {
                            try {
                                m_state = States::ConnectedCalling;
                                auto const result =
                                    pDelegate->onCall(info.args.args_list.empty() ? "" : info.args.args_list[0].as_string());
                                if (m_state == States::ConnectedCalling) {
                                    ws.yield(info.request_id, { result });
                                    m_state = States::Connected;
                                }
                            } catch (const core::RuntimeException &e) {
                                ws.invocation_error(info.request_id, e.what());
                                oslog::error(OS_LOG_CHANNEL_DATA) << e.what() << oslog::end();
                            }
                            return;
                        }
                    }
                }
                ws.invocation_error(info.request_id, "Error while calling procedure.");
            });
    }

    void MessagingWampcc::unregisterCall(const std::string &uri, IErrorDelegatePtr pError) {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);

        auto &session = ensureValidSession();
        if (m_registeredCalls.find(uri) == m_registeredCalls.end()) {
            throw MessagingException("No procedure registered with name " + uri);
        }
        auto registration_id = m_registeredCalls[uri];
        session.unprovide(registration_id,
            [this, &registration_id, uri, pwErrorDelegate = IErrorDelegateWPtr(pError)](wamp_session &, unregistered_info info) {
                std::lock_guard<std::recursive_mutex> guard(m_mutex);

                if (info.was_error) {
                    auto const pErrorDelegate = pwErrorDelegate.lock();
                    if (pErrorDelegate != nullptr)
                        pErrorDelegate->onError("Unregister error for " + uri);
                    return;
                }
                auto const itCallDelegates = m_callDelegates.find(registration_id);
                if (itCallDelegates != m_callDelegates.end()) {
                    m_callDelegates.erase(itCallDelegates);
                }
                auto const itRegisteredCalls = m_registeredCalls.find(uri);
                if (itRegisteredCalls != m_registeredCalls.end()) {
                    m_registeredCalls.erase(itRegisteredCalls);
                }
            });
    }

    void MessagingWampcc::invoke(
        const std::string &uri, const std::string &argsSerialized, IClientDelegatePtr pDelegate, IErrorDelegatePtr pError) const {
        auto &session = ensureValidSession();
        wamp_args wampArgs;
        wampArgs.args_list.push_back(argsSerialized);

        session.call(uri,
            {},
            wampArgs,
            [this, pWDelegate = IClientDelegateWPtr(pDelegate), pWErrorDelegate = IErrorDelegateWPtr(pError)](
                wamp_session &, wampcc::result_info info) {
                std::lock_guard<std::recursive_mutex> guard(m_mutex);

                if ((info.was_error || info.args.args_list.empty())) {
                    std::string strError;
                    if (info.was_error) {
                        strError = "There was an error when invoking: " + info.error_uri;
                    } else if (info.args.args_list.empty()) {
                        strError = "No result received from remote call.";
                    }

                    if (auto const pErrorDelegate = pWErrorDelegate.lock(); pErrorDelegate != nullptr) {
                        pErrorDelegate->onError(strError);
                    }
                    return;
                }

                auto const pClientDelegate = pWDelegate.lock();
                if (pClientDelegate != nullptr) {
                    pClientDelegate->onResult(info.args.args_list[0].as_string());
                }
            });
    }

    void MessagingWampcc::subscribe(const std::string &topic, IEventDelegatePtr pDelegate, IErrorDelegatePtr pError) /*override*/ {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);

        auto &session = ensureValidSession();
        if (m_subscribedTopics.find(topic) != m_subscribedTopics.end())
            throw MessagingException(topic + " already subscribed");

        session.subscribe(
            topic,
            {},
            [topic, this, pWDelegate = IEventDelegateWPtr(pDelegate), pWErrorDelegate = IErrorDelegateWPtr(pError)](
                wamp_session &, const subscribed_info &info) {
                std::lock_guard<std::recursive_mutex> guard(m_mutex);

                if (info.was_error) {
                    auto const pErrorDelegate = pWErrorDelegate.lock();
                    if (pErrorDelegate != nullptr)
                        pErrorDelegate->onError("There was an issue subscribing the topic: " + topic);
                }
                m_eventDelegates[info.subscription_id] = pWDelegate;
                m_subscribedTopics[topic]              = info.subscription_id;
            },
            [this](wamp_session &, event_info info) {
                std::lock_guard<std::recursive_mutex> guard(m_mutex);

                const IEventDelegatePtr pDelegate = m_eventDelegates[info.subscription_id].lock();
                if (pDelegate != nullptr) {
                    if (!info.args.args_list.empty()) {
                        pDelegate->onEvent(info.args.args_list[0].as_string());
                    }
                }
            });
    }

    void MessagingWampcc::unsubscribe(const std::string &topic, IErrorDelegatePtr pError) /*override*/ {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);

        auto &session       = ensureValidSession();
        auto subscriptionId = m_subscribedTopics[topic];
        session.unsubscribe(subscriptionId,
            [this, subscriptionId, topic, pWErrorDelegate = IErrorDelegateWPtr(pError)](wamp_session &, const unsubscribed_info &info) {
                std::lock_guard<std::recursive_mutex> guard(m_mutex);

                if (info.was_error) {
                    auto const pErrorDelegate = pWErrorDelegate.lock();
                    if (pErrorDelegate != nullptr)
                        pErrorDelegate->onError("Unsubscription failed to: " + topic);
                    return;
                }
                auto const itEventDelegate = m_eventDelegates.find(subscriptionId);
                if (itEventDelegate != m_eventDelegates.cend())
                    m_eventDelegates.erase(itEventDelegate);
                auto const itSubscribedTopics = m_subscribedTopics.find(topic);
                if (itSubscribedTopics != m_subscribedTopics.cend())
                    m_subscribedTopics.erase(itSubscribedTopics);
            });
    }

    void MessagingWampcc::publish(const std::string &topic, const std::string &argsSerialized, IErrorDelegatePtr pError) const
    /*override*/ {
        auto &session = ensureValidSession();
        wamp_args wampArgs;
        wampArgs.args_list.push_back(argsSerialized);

        session.publish(topic, {}, wampArgs, [topic, pWErrorDelegate = IErrorDelegateWPtr(pError)](wamp_session &, published_info &info) {
            if (info.was_error) {
                auto const pErrorDelegate = pWErrorDelegate.lock();
                if (pErrorDelegate != nullptr)
                    pErrorDelegate->onError("Publishing failed for topic: " + topic);
            }
        });
    }

    wampcc::wamp_session &MessagingWampcc::ensureValidSession() const {
        if (m_session && m_session->is_open()) {
            return *m_session;
        }
        throw MessagingException("You have to connect to use the client");
    }

    bool MessagingWampcc::tryConnect(const std::string &brokerUrl, int brokerPort, const std::string &realm) {
        try {
            connect(brokerUrl, brokerPort, realm);
        } catch (const MessagingException &) {
            return false;
        }

        return true;
    }

    void MessagingWampcc::doDisconnect() {
        if (m_session != nullptr && m_session->is_open()) {
            m_bStopRetryConnection = true;
            m_session->close().wait();
        }

        if (!m_bStopRetryConnection) {
            {
                std::lock_guard lock(m_mutConnection);
                m_bStopRetryConnection = true;
                m_cvConnection.notify_one();
            }
            m_futConnection.wait();
        }

        setStateDisconnected();
    }

    void MessagingWampcc::retryConnection() {
        auto const guard = core::make_scope_exit([this]() { m_bStopRetryConnection = false; });
        while (!m_bStopRetryConnection) {
            std::unique_lock lock(m_mutConnection);
            if (!m_cvConnection.wait_for(lock, s_timeoutRetryConnection, [this]() { return m_bStopRetryConnection; })) {
                m_bStopRetryConnection = tryConnect(m_brokerUrl, m_brokerPort, m_realm);
            }
        }
    }

    bool MessagingWampcc::isStateConnected() const {
        switch (m_state) {
        case States::Idle:
        case States::Disconnected:
        case States::DisconnectedCalling:
            return false;
        case States::Connected:
        case States::ConnectedCalling:
        case States::ConnectedCallingAbort:
            return true;
        }

        return false;
    }

    void MessagingWampcc::setStateConnected() {
        switch (m_state) {
        case States::Idle:
        case States::Disconnected:
            m_state = States::Connected;
            break;
        case States::DisconnectedCalling:
            m_state = States::ConnectedCallingAbort;
            break;
        case States::Connected:
        case States::ConnectedCalling:
        case States::ConnectedCallingAbort:
            break; // no state change
        }
        notify(MessagingConnectionMsg{ true });
    }

    bool MessagingWampcc::isStateDisconnected() const {
        return !isStateConnected() && m_state != States::Idle;
    }

    void MessagingWampcc::setStateDisconnected() {
        switch (m_state) {
        case States::Disconnected:
        case States::DisconnectedCalling:
            break; // no state change
        case States::Idle:
        case States::Connected:
            m_state = States::Disconnected;
            break;
        case States::ConnectedCalling:
        case States::ConnectedCallingAbort:
            m_state = States::DisconnectedCalling;
            break;
        }

        m_registeredCalls.clear();
        m_callDelegates.clear();
        m_subscribedTopics.clear();
        m_eventDelegates.clear();
        notify(MessagingConnectionMsg{ false });
    }

    bool MessagingWampcc::isStateIdle() const {
        return m_state == States::Idle;
    }

} // namespace NS_OSBASE::data::impl
