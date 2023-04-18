// \brief Implementation tests of IMessaging

#include "osData/IBroker.h"
#include "osData/IMessaging.h"
#include "osData/MessagingException.h"
#include "osData/Log.h"
#include "gtest/gtest.h"

#include <chrono>
#include <future>
#include <thread>
#include <queue>

using namespace std::chrono_literals;

namespace cho::osbase::data::ut {

    IMessagingPtr connectToWamp() {
        const std::string url = "127.0.0.1";
        const int port        = 8080;

        auto wampccMessaging = makeMessaging();
        if (wampccMessaging == nullptr)
            return nullptr;

        wampccMessaging->connect(url, port);
        return wampccMessaging;
    }

    class TestErrorDelegate : public IMessaging::IErrorDelegate {
    public:
        void onError(const std::string &error) override {
            m_error.set_value(error);
        }

        std::promise<std::string> m_error;
    };

    class TestClientDelegate : public IMessaging::IClientDelegate {
    public:
        void onResult(const IMessaging::JsonText &json) override {
            oslog::trace() << json << oslog::end();
            auto const pStream = core::makeJsonStream(std::stringstream(json));
            auto const args    = pStream->getValue(std::tuple<std::string>());
            m_received.set_value(std::get<0>(args));
        }

        std::promise<std::string> m_received;
    };

    class TestEventDelegate : public IMessaging::IEventDelegate {
    public:
        void onEvent(const IMessaging::JsonText &json) override {
            oslog::trace() << json << oslog::end();
            m_received.set_value(json);
        }

        std::promise<std::string> m_received;
    };

    class TestSupplierDelegate : public IMessaging::ISupplierDelegate {
    public:
        std::string onCall(const IMessaging::JsonText &json) override {
            oslog::trace() << json << oslog::end();
            std::string result;
            {
                auto const pStream = core::makeJsonStream(std::stringstream(json));
                auto const args    = pStream->getValue(std::tuple<std::string>());
                result             = std::get<0>(args);
            }

            std::string jsonResult;
            {
                auto const pStream = core::makeJsonStream();
                auto const args    = std::make_tuple(result);
                pStream->setValue(args);
                std::ostringstream oss;
                oss << *pStream;
                jsonResult = oss.str();
            }

            try {
                auto const timeout = std::stoi(result);
                if (timeout != 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
                }
            } catch (const std::invalid_argument &) {
            }

            m_called.set_value();
            return jsonResult;
        }

        std::promise<void> m_called;
    };

    class IMessaging_UT : public testing::Test {
    protected:
        class MessagingConnectionObserver : public core::Observer<IMessaging::MessagingConnectionMsg> {
        public:
            void update(const core::Observable &, const IMessaging::MessagingConnectionMsg &msg) override {
                std::lock_guard lock(m_mut);
                m_connections.push(msg.isConnected());
                m_cv.notify_one();
            }

            std::optional<bool> wait(const std::chrono::milliseconds &timeout = 100ms) {
                std::unique_lock lock(m_mut);
                if (m_cv.wait_for(lock, timeout, [this]() { return !m_connections.empty(); })) {
                    auto const guard = core::make_scope_exit([this]() { m_connections.pop(); });
                    return m_connections.front();
                }
                return {};
            }

        private:
            std::mutex m_mut;
            std::condition_variable m_cv;
            std::queue<bool> m_connections;
        };
        void SetUp() override {
            startBroker();
        }

        void TearDown() override {
            stopBroker();
        }

        void startBroker() {
            if (m_refCount == 0) {
                m_broker = makeBroker();
                m_broker->start(8080);
            }
            ++m_refCount;
        }

        void stopBroker() {
            if (m_refCount == 0) {
                return;
            }
            --m_refCount;
            if (m_refCount == 0) {
                m_broker->stop();
                m_broker.reset();
            }
        }

    private:
        std::shared_ptr<IBroker> m_broker;
        size_t m_refCount = 0;
    };

    TEST_F(IMessaging_UT, Create_Broker) {
        auto wampccBroker = makeBroker();

        ASSERT_NE(wampccBroker, nullptr);
    }

    TEST_F(IMessaging_UT, New_Delete_Right_Away) {
        auto wampccMessaging = makeMessaging();

        ASSERT_NE(wampccMessaging, nullptr);

        wampccMessaging.reset();

        ASSERT_EQ(wampccMessaging, nullptr);
    }

    TEST_F(IMessaging_UT, Register_And_Call_Should_Call_Delegate_And_Send_Response) {
        const std::string uri = "com.test.echo";

        const auto wampccMessaging = connectToWamp();
        ASSERT_NE(wampccMessaging, nullptr);

        const auto pClientDelegate   = std::make_shared<TestClientDelegate>();
        const auto pSupplierDelegate = std::make_shared<TestSupplierDelegate>();
        const auto pErrorDelegate    = std::make_shared<TestErrorDelegate>();
        auto fClient                 = pClientDelegate->m_received.get_future();
        const auto fSupplier         = pSupplierDelegate->m_called.get_future();

        wampccMessaging->registerCall(uri, pSupplierDelegate, pErrorDelegate);

        auto const argTest = std::string("Test");
        auto const args    = std::make_tuple(argTest);
        auto const pStream = core::makeJsonStream();
        pStream->setValue(args);

        std::ostringstream oss;
        oss << *pStream;
        wampccMessaging->invoke(uri, oss.str(), pClientDelegate, pErrorDelegate);

        // The invoke should trigger the provider delegate
        auto status = fSupplier.wait_for(std::chrono::seconds(5));
        ASSERT_NE(status, std::future_status::timeout);
        // It should then trigger the client delegate with the response
        status = fClient.wait_for(std::chrono::seconds(5));
        ASSERT_NE(status, std::future_status::timeout);
        ASSERT_EQ(fClient.get(), argTest);

        wampccMessaging->disconnect();
    }

    TEST_F(IMessaging_UT, Double_Register_Should_Throw_An_Error) {
        const std::string uri = "com.test.echo";

        auto wampccMessaging = connectToWamp();
        ASSERT_NE(wampccMessaging, nullptr);

        auto pClientDelegate   = std::make_shared<TestClientDelegate>();
        auto pSupplierDelegate = std::make_shared<TestSupplierDelegate>();
        auto pErrorDelegate    = std::make_shared<TestErrorDelegate>();
        auto fClient           = pClientDelegate->m_received.get_future();
        auto fSupplier         = pSupplierDelegate->m_called.get_future();

        wampccMessaging->registerCall(uri, pSupplierDelegate, pErrorDelegate);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        ASSERT_THROW({ wampccMessaging->registerCall(uri, pSupplierDelegate, pErrorDelegate); }, MessagingException);

        wampccMessaging->disconnect();
    }

    TEST_F(IMessaging_UT, Double_Register_Without_Timing_Should_Create_An_Error_In_The_Broker) {
        const std::string uri = "com.test.echo";

        auto wampccMessaging = connectToWamp();
        ASSERT_NE(wampccMessaging, nullptr);

        auto pSupplierDelegate = std::make_shared<TestSupplierDelegate>();
        auto pErrorDelegate    = std::make_shared<TestErrorDelegate>();
        auto fError            = pErrorDelegate->m_error.get_future();

        wampccMessaging->registerCall(uri, pSupplierDelegate, pErrorDelegate);

        wampccMessaging->registerCall(uri, pSupplierDelegate, pErrorDelegate);

        fError.wait_for(std::chrono::seconds(5));
        ASSERT_EQ(fError.get(), "Procedure registration failed, error wamp.error.procedure_already_exists");

        wampccMessaging->disconnect();
    }

    TEST_F(IMessaging_UT, Register_Unregister_And_Call_Should_Fail) {
        const std::string uri = "com.test.toUnregister";

        auto wampccMessaging = connectToWamp();
        ASSERT_NE(wampccMessaging, nullptr);

        auto pClientDelegate   = std::make_shared<TestClientDelegate>();
        auto pSupplierDelegate = std::make_shared<TestSupplierDelegate>();
        auto pErrorDelegate    = std::make_shared<TestErrorDelegate>();
        auto fError            = pErrorDelegate->m_error.get_future();

        wampccMessaging->registerCall(uri, pSupplierDelegate, pErrorDelegate);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        wampccMessaging->unregisterCall(uri, pErrorDelegate);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        const std::string notImplementedStr = "There was an error when invoking: wamp.error.no_such_procedure";
        wampccMessaging->invoke(uri, "", pClientDelegate, pErrorDelegate);

        // The invoke should trigger the error delegate
        auto status = fError.wait_for(std::chrono::seconds(5));
        ASSERT_NE(status, std::future_status::timeout);
        ASSERT_EQ(fError.get(), notImplementedStr);

        wampccMessaging->disconnect();
    }

    TEST_F(IMessaging_UT, Subscription_Should_Call_Delegate_When_Event_Happens_And_Not_After_Unsub) {
        const std::string topic = "com.test.topic";
        const std::string args  = "args";

        auto wampcc1 = connectToWamp();
        auto wampcc2 = connectToWamp();

        auto pEventDelegate = std::make_shared<TestEventDelegate>();
        auto pErrorDelegate = std::make_shared<TestErrorDelegate>();
        auto fEvent         = pEventDelegate->m_received.get_future();

        wampcc1->subscribe(topic, pEventDelegate, pErrorDelegate);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        wampcc2->publish(topic, args, pErrorDelegate);

        auto status = fEvent.wait_for(std::chrono::seconds(5));
        ASSERT_NE(status, std::future_status::timeout);
        ASSERT_EQ(fEvent.get(), args);

        wampcc1->unsubscribe(topic, pErrorDelegate);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        wampcc2->publish(topic, args, pErrorDelegate);

        pEventDelegate->m_received = std::promise<std::string>();
        fEvent                     = pEventDelegate->m_received.get_future();
        status                     = fEvent.wait_for(std::chrono::milliseconds(500));
        ASSERT_EQ(status, std::future_status::timeout);

        wampcc1->disconnect();
        wampcc2->disconnect();
    }

    TEST_F(IMessaging_UT, Double_Subscribe_Should_Fail) {
        const std::string topic = "com.test.veryInterestingTopic";

        auto wampccMessaging = connectToWamp();
        ASSERT_NE(wampccMessaging, nullptr);

        auto pEventDelegate = std::make_shared<TestEventDelegate>();
        auto pErrorDelegate = std::make_shared<TestErrorDelegate>();
        auto fError         = pErrorDelegate->m_error.get_future();

        wampccMessaging->subscribe(topic, pEventDelegate, pErrorDelegate);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        ASSERT_THROW({ wampccMessaging->subscribe(topic, pEventDelegate, pErrorDelegate); }, MessagingException);

        wampccMessaging->disconnect();
    }

    TEST_F(IMessaging_UT, Calling_Register_Without_Connect_Should_Throw) {
        const std::string uri = "com.test.fail";

        auto wampccMessaging = makeMessaging();

        ASSERT_NE(wampccMessaging, nullptr);

        EXPECT_THROW({ wampccMessaging->registerCall(uri, std::make_shared<TestSupplierDelegate>(), nullptr); }, MessagingException);

        EXPECT_THROW({ wampccMessaging->unregisterCall(uri, nullptr); }, MessagingException);

        EXPECT_THROW({ wampccMessaging->invoke(uri, "", std::make_shared<TestClientDelegate>(), nullptr); }, MessagingException);

        EXPECT_THROW({ wampccMessaging->subscribe(uri, std::make_shared<TestEventDelegate>(), nullptr); }, MessagingException);

        EXPECT_THROW({ wampccMessaging->unsubscribe(uri, nullptr); }, MessagingException);

        EXPECT_THROW({ wampccMessaging->publish(uri, "", nullptr); }, MessagingException);
    }

    TEST_F(IMessaging_UT, check_connection_broker_started) {
        auto pMessaging = makeMessaging();
        MessagingConnectionObserver o;
        pMessaging->attachAll(o);

        ASSERT_NO_THROW(pMessaging->connect("127.0.0.1", 8080));
        auto const connect = o.wait(10s);
        ASSERT_TRUE(connect.has_value());
        ASSERT_TRUE(connect.value());
    }

    TEST_F(IMessaging_UT, check_connection_broker_not_started) {
        stopBroker();

        auto pMessaging = makeMessaging();
        MessagingConnectionObserver o;
        pMessaging->attachAll(o);

        ASSERT_THROW(pMessaging->connect("127.0.0.1", 8080), MessagingException);
        auto connect = o.wait(10s);
        ASSERT_TRUE(connect.has_value());
        ASSERT_FALSE(connect.value());

        startBroker();
        connect = o.wait(10s);
        ASSERT_TRUE(connect.has_value());
        ASSERT_TRUE(connect.value());
    }

    TEST_F(IMessaging_UT, check_deconnection) {
        auto pMessaging = connectToWamp();
        MessagingConnectionObserver o;
        pMessaging->attachAll(o);

        stopBroker();
        auto connect = o.wait(1s);
        ASSERT_TRUE(connect.has_value());
        ASSERT_FALSE(connect.value());

        pMessaging->disconnect();
    }

    TEST_F(IMessaging_UT, check_reconnection) {
        static constexpr auto timeout = 2s;
        auto pMessaging               = connectToWamp();
        MessagingConnectionObserver o;
        pMessaging->attachAll(o);

        // check with several deconnection / reconnection of the broker
        for (size_t nbTry = 0; nbTry < 2; ++nbTry) {
            stopBroker();
            auto connect = o.wait(1s);
            ASSERT_TRUE(connect.has_value());
            ASSERT_FALSE(connect.value());

            std::this_thread::sleep_for(timeout);

            startBroker();
            connect = o.wait(timeout);
            ASSERT_TRUE(connect.has_value());
            ASSERT_TRUE(connect.value());
        }

        pMessaging->disconnect();
    }

    TEST_F(IMessaging_UT, loose_connection_while_calling) {
        const std::string uri = "com.test.echo";

        const auto wampccMessaging = connectToWamp();
        ASSERT_NE(wampccMessaging, nullptr);

        const auto pClientDelegate   = std::make_shared<TestClientDelegate>();
        const auto pSupplierDelegate = std::make_shared<TestSupplierDelegate>();
        const auto pErrorDelegate    = std::make_shared<TestErrorDelegate>();
        auto fClient                 = pClientDelegate->m_received.get_future();
        const auto fSupplier         = pSupplierDelegate->m_called.get_future();

        wampccMessaging->registerCall(uri, pSupplierDelegate, pErrorDelegate);

        constexpr int timeout = 1000; // ms
        auto const argTimout  = std::to_string(timeout);
        auto const args       = std::make_tuple(argTimout);
        auto const pStream    = core::makeJsonStream();
        pStream->setValue(args);

        std::ostringstream oss;
        oss << *pStream;
        wampccMessaging->invoke(uri, oss.str(), pClientDelegate, pErrorDelegate);

        std::this_thread::sleep_for(std::chrono::milliseconds(timeout / 3));
        stopBroker();

        auto status = fSupplier.wait_for(std::chrono::seconds(timeout));
        ASSERT_NE(status, std::future_status::timeout);

        status = fClient.wait_for(std::chrono::milliseconds(timeout));
        ASSERT_EQ(status, std::future_status::timeout);
    }

} // namespace cho::osbase::data::ut
