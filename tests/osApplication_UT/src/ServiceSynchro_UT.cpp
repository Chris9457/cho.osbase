// \brief Tests for service's synchro

#include "BaseService_UT.h"
#include "testservice.h"
#include "TestServiceImpl/TestServiceImpl.h"
#include <queue>

using namespace std::chrono_literals;
namespace nscore = NS_OSBASE::core;

namespace NS_OSBASE::application::ut {

    class ServiceSynchro_UT : public BaseService_UT {
    protected:
        class TestServiceObserver : public nscore::Observer<ServiceConnectionMsg, testservice::api::DummyEvent> {
        public:
            TestServiceObserver(bool bClient = false) : m_bClient(bClient) {
            }

            void update(const core::Observable &, const ServiceConnectionMsg &msg) override {
                pushConnection(msg.isConnected());
            }

            std::optional<bool> waitConnectionMsg(const std::chrono::milliseconds &timeout = 100ms) {
                std::unique_lock lock(m_mut);
                if (m_cv.wait_for(lock, timeout, [this]() { return !m_connections.empty(); })) {
                    lock.unlock();
                    return pullConnection();
                }

                return {};
            }

            void update(const core::Observable &, const testservice::api::DummyEvent &) override {
                m_promiseDummy.set_value();
            }

            bool waitDummy(const std::chrono::milliseconds &timeout) {
                auto const guard    = core::make_scope_exit([this]() { std::promise<void>().swap(m_promiseDummy); });
                auto const futDummy = m_promiseDummy.get_future();
                return futDummy.wait_for(timeout) == std::future_status::ready;
            }

        private:
            void pushConnection(const bool bConnected) {
                std::lock_guard lock(m_mut);

                {
                    std::lock_guard lockLog(m_mutLog);
                    std::cout << (m_bClient ? "Client: " : "Server: ") << "push connection: " << std::boolalpha << bConnected;
                    auto const now = std::chrono::steady_clock::now();
                    if (m_lastConnectionMsg.has_value()) {
                        auto const duration = now - m_lastConnectionMsg.value();
                        std::cout << ", duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " ms";
                    }
                    std::cout << std::endl;
                    m_lastConnectionMsg = now;
                }
                m_connections.push(bConnected);
                m_cv.notify_one();
            }

            bool pullConnection() {
                std::lock_guard lock(m_mut);
                auto const bConnected = m_connections.front();
                m_connections.pop();
                if (!m_connections.empty()) {
                    m_cv.notify_one();
                }
                return bConnected;
            }

            std::queue<bool> m_connections;
            std::mutex m_mut;
            std::condition_variable m_cv;
            std::optional<std::chrono::steady_clock::time_point> m_lastConnectionMsg;
            std::promise<void> m_promiseDummy;
            bool m_bClient = false;
            static inline std::mutex m_mutLog;
        };

        class TestServiceClient {
        public:
            TestServiceClient()
                : m_pStub(testservice::api::makeStub(std::string{ "ws://" + getBrokerUrl() + ":" + std::to_string(getBrokerPort()) }, "")) {
                m_pStub->connect();
            }

            TestServiceClient(TestServiceObserver &o)
                : m_pStub(testservice::api::makeStub(std::string{ "ws://" + getBrokerUrl() + ":" + std::to_string(getBrokerPort()) }, "")) {
                m_pStub->attachAll(o);
                m_pStub->connect();
            }

            ~TestServiceClient() {
                try {
                    m_pStub->disconnect();
                } catch (const ServiceException &e) {
                    std::cout << "client: " << e.what() << std::endl;
                }
            }

            testservice::api::ITestServicePtr operator->() const {
                return m_pStub;
            }

        private:
            testservice::api::ITestServicePtr m_pStub;
        };

        void SetUp() override {
            BaseService_UT::SetUp();
            startBroker();
        }

        void TearDown() override {
            stopBroker();
            BaseService_UT::TearDown();
        }

        void startBroker() {
            m_pBroker = data::makeBroker();
            m_pBroker->start(getBrokerPort());
        }

        void stopBroker() {
            if (m_pBroker != nullptr) {
                m_pBroker->stop();
                m_pBroker.reset();
            }
        }

        void startService() {
            testservice::impl::TheTestServiceImpl.runAsync();
            testservice::impl::TheTestServiceImpl.connect();
            // std::this_thread::sleep_for(100ms);
        }

        void stopService() const {
            try {
                // std::this_thread::sleep_for(100ms);
                testservice::impl::TheTestServiceImpl.disconnect();
            } catch (const ServiceException &e) {
                std::cout << "server: " << e.what() << std::endl;
            }
            testservice::impl::TheTestServiceImpl.stop();
        }

        static std::string getBrokerUrl() {
            return "127.0.0.1";
        }

        static constexpr unsigned short getBrokerPort() {
            return 8080;
        }

    private:
        data::IBrokerPtr m_pBroker;
        std::shared_future<void> m_futRun;
    };

    TEST_F(ServiceSynchro_UT, getAlivePeriod) {
        auto const guard = nscore::make_scope_exit([this]() { stopService(); });
        startService();

        const TestServiceClient client;

        auto const expectedAlivePeriod = testservice::impl::TheTestServiceImpl.getAlivePeriod();
        ASSERT_EQ(expectedAlivePeriod, client->getAlivePeriod());
    }

    TEST_F(ServiceSynchro_UT, service_checkStart) {
        const TestServiceClient client1, client2;
        TestServiceObserver o1, o2;
        auto const guard = core::make_scope_exit([&]() {
            stopService();
            client1->detachAll(o1);
            client2->detachAll(o2);
        });
        client1->attachAll(o1);
        client1->attachAll(o2);
        startService();

        auto const alivePeriod = testservice::impl::TheTestServiceImpl.getAlivePeriod();

        // check the alive msg through the clients
        auto const connect1 = o1.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_TRUE(connect1.has_value() && connect1.value());

        auto const connect2 = o2.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_TRUE(connect2.has_value() && connect2.value());
    }

    TEST_F(ServiceSynchro_UT, service_checkRPC) {
        const TestServiceClient client1, client2;
        TestServiceObserver o1, o2;
        auto const guard = core::make_scope_exit([&]() {
            stopService();
            client1->detachAll(o1);
            client2->detachAll(o2);
        });
        client1->attachAll(o1);
        client1->attachAll(o2);
        startService();
        auto const alivePeriod = testservice::impl::TheTestServiceImpl.getAlivePeriod();

        // check both clients are connected
        auto connect1 = o1.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_TRUE(connect1.has_value() && connect1.value());
        connect1.reset();

        auto connect2 = o2.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_TRUE(connect2.has_value() && connect2.value());
        connect2.reset();

        // invoke the RPC
        testservice::impl::TheTestServiceImpl.resetWait();
        std::thread th([&]() { client1->wait(alivePeriod * 3); });
        testservice::impl::TheTestServiceImpl.waitForStartWait();

        // check the alive msg is suspended during the RPC call
        connect2 = o2.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_FALSE(connect2.has_value());

        th.join();

        // check no deconnection after the end of the RPC
        connect2 = o2.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_FALSE(connect2.has_value());

        connect1 = o1.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_FALSE(connect1.has_value());
    }

    TEST_F(ServiceSynchro_UT, service_publishEvent) {
        startService();

        TestServiceObserver o1, o2;
        const TestServiceClient client1(o1), client2(o2);
        auto const guard       = core::make_scope_exit([&]() {
            stopService();
            client1->detachAll(o1);
            client2->detachAll(o2);
        });
        auto const alivePeriod = std::chrono::milliseconds(testservice::impl::TheTestServiceImpl.getAlivePeriod());

        auto connect1 = o1.waitConnectionMsg(0s);
        ASSERT_TRUE(connect1.has_value() && connect1.value());

        auto connect2 = o2.waitConnectionMsg(0s);
        ASSERT_TRUE(connect2.has_value() && connect2.value());

        testservice::impl::TheTestServiceImpl.notifyDummyEvent(); // perform a sleep around 1.1 * alive period in the main thread
                                                                  // verify there is no disconnection

        connect1 = o1.waitConnectionMsg(alivePeriod * 2);
        ASSERT_FALSE(connect1.has_value());
        connect2 = o2.waitConnectionMsg(alivePeriod * 2);
        ASSERT_FALSE(connect2.has_value());
    }

    TEST_F(ServiceSynchro_UT, client_Connection_Service_Started) {
        startService();

        TestServiceObserver o1, o2;
        const TestServiceClient client1(o1), client2(o2);
        auto const guard = core::make_scope_exit([&]() {
            stopService();
            client1->detachAll(o1);
            client2->detachAll(o2);
        });

        // check both clients are connected
        auto const connect1 = o1.waitConnectionMsg(0s);
        ASSERT_TRUE(connect1.has_value() && connect1.value());

        auto const connect2 = o2.waitConnectionMsg(0s);
        ASSERT_TRUE(connect2.has_value() && connect2.value());
    }

    TEST_F(ServiceSynchro_UT, client_Connection_Service_Not_Started) {
        TestServiceObserver o1, o2;
        const TestServiceClient client1(o1), client2(o2);
        auto const guard = core::make_scope_exit([&]() {
            client1->detachAll(o1);
            client2->detachAll(o2);
        });

        // check both clients are connected
        auto const connect1 = o1.waitConnectionMsg(0s);
        ASSERT_FALSE(connect1.has_value());

        auto const connect2 = o2.waitConnectionMsg(0s);
        ASSERT_FALSE(connect2.has_value());
    }

    TEST_F(ServiceSynchro_UT, client_Connected_Service_Connection) {
        TestServiceObserver o1, o2;
        const TestServiceClient client1(o1), client2(o2);
        auto const guard = core::make_scope_exit([&]() {
            stopService();
            client1->detachAll(o1);
            client2->detachAll(o2);
        });

        startService();

        auto const alivePeriod = testservice::impl::TheTestServiceImpl.getAlivePeriod();

        // check both clients are connected
        auto const connect1 = o1.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_TRUE(connect1.has_value() && connect1.value());

        auto const connect2 = o2.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_TRUE(connect2.has_value() && connect2.value());
    }

    TEST_F(ServiceSynchro_UT, client_Connected_Service_Fall) {
        TestServiceObserver o1, o2;
        const TestServiceClient client1(o1), client2(o2);
        auto const guard = core::make_scope_exit([&]() {
            client1->detachAll(o1);
            client2->detachAll(o2);
        });

        startService();

        auto const alivePeriod = testservice::impl::TheTestServiceImpl.getAlivePeriod();

        // check both clients are connected
        auto connect1 = o1.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_TRUE(connect1.has_value() && connect1.value());

        auto connect2 = o2.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_TRUE(connect2.has_value() && connect2.value());

        stopService();
        connect1 = o1.waitConnectionMsg(std::chrono::milliseconds(3 * alivePeriod));
        ASSERT_TRUE(connect1.has_value() && !connect1.value());

        connect2 = o2.waitConnectionMsg(std::chrono::milliseconds(3 * alivePeriod));
        ASSERT_TRUE(connect2.has_value() && !connect2.value());
    }

    TEST_F(ServiceSynchro_UT, client_Connected_Service_Restart_after_timeout_alive) {
        startService();

        TestServiceObserver o1, o2;
        const TestServiceClient client1(o1), client2(o2);
        auto const guard = core::make_scope_exit([&]() {
            stopService();
            client1->detachAll(o1);
            client2->detachAll(o2);
        });

        auto const alivePeriod = testservice::impl::TheTestServiceImpl.getAlivePeriod();

        auto connect1 = o1.waitConnectionMsg(0s);
        auto connect2 = o2.waitConnectionMsg(0s);

        stopService();

        // Wait for disconnection
        connect1 = o1.waitConnectionMsg(std::chrono::milliseconds(3 * alivePeriod));
        connect2 = o2.waitConnectionMsg(std::chrono::milliseconds(3 * alivePeriod));

        startService();

        // check re-connection
        connect1 = o1.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_TRUE(connect1.has_value() && connect1.value());

        connect2 = o2.waitConnectionMsg(std::chrono::milliseconds(2 * alivePeriod));
        ASSERT_TRUE(connect2.has_value() && connect2.value());
    }

    TEST_F(ServiceSynchro_UT, client_Connected_Service_Restart_before_timeout_alive) {
        startService();

        TestServiceObserver o1, o2;
        const TestServiceClient client1(o1), client2(o2);
        auto const guard = core::make_scope_exit([&]() {
            stopService();
            client1->detachAll(o1);
            client2->detachAll(o2);
        });

        auto const alivePeriod = std::chrono::milliseconds(testservice::impl::TheTestServiceImpl.getAlivePeriod());

        auto connect1 = o1.waitConnectionMsg(0s);
        ASSERT_TRUE(connect1.has_value() && connect1.value());

        auto connect2 = o2.waitConnectionMsg(0s);
        ASSERT_TRUE(connect2.has_value() && connect2.value());

        // check no deconnection
        connect1 = o1.waitConnectionMsg(alivePeriod);
        ASSERT_FALSE(connect1.has_value());

        connect2 = o2.waitConnectionMsg(alivePeriod);
        ASSERT_FALSE(connect2.has_value());

        stopService();
        startService();

        // check disconnection
        connect1 = o1.waitConnectionMsg(2 * alivePeriod);
        ASSERT_TRUE(connect1.has_value());
        ASSERT_FALSE(connect1.value());

        connect2 = o2.waitConnectionMsg(2 * alivePeriod);
        ASSERT_TRUE(connect2.has_value() && !connect2.value());

        // check re-connection
        connect1 = o1.waitConnectionMsg(2 * alivePeriod);
        ASSERT_TRUE(connect1.has_value() && connect1.value());

        connect2 = o2.waitConnectionMsg(2 * alivePeriod);
        ASSERT_TRUE(connect2.has_value() && connect2.value());
    }

    TEST_F(ServiceSynchro_UT, broker_disconnection_after_service_start) {
        auto const guard       = core::make_scope_exit([&]() {
            stopService();
            return;
        });
        auto const alivePeriod = std::chrono::milliseconds(testservice::impl::TheTestServiceImpl.getAlivePeriod());

        TestServiceObserver oServer;
        testservice::impl::TheTestServiceImpl.attachAll(oServer);

        startService();

        auto connectServer = oServer.waitConnectionMsg(3 * alivePeriod);
        ASSERT_TRUE(connectServer.has_value());
        ASSERT_TRUE(connectServer.value());

        stopBroker();

        connectServer = oServer.waitConnectionMsg(3 * alivePeriod);
        ASSERT_TRUE(connectServer.has_value());
        ASSERT_FALSE(connectServer.value());
    }

    TEST_F(ServiceSynchro_UT, broker_connection_after_service_start) {
        auto const guard       = core::make_scope_exit([&]() { stopService(); });
        auto const alivePeriod = std::chrono::milliseconds(testservice::impl::TheTestServiceImpl.getAlivePeriod());

        stopBroker();

        TestServiceObserver oServer;
        testservice::impl::TheTestServiceImpl.attachAll(oServer);

        startService();

        auto connectServer = oServer.waitConnectionMsg(3 * alivePeriod);
        ASSERT_FALSE(connectServer.has_value());

        startBroker();
        connectServer = oServer.waitConnectionMsg(3 * alivePeriod);
        ASSERT_TRUE(connectServer.has_value());
        ASSERT_TRUE(connectServer.value());

        // check rpc available
        TestServiceClient client;
        auto clientAlivePeriod = std::chrono::milliseconds(client->getAlivePeriod());
        ASSERT_EQ(alivePeriod, clientAlivePeriod);

        stopBroker();
        connectServer = oServer.waitConnectionMsg(3 * alivePeriod);
        ASSERT_TRUE(connectServer.has_value());
        ASSERT_FALSE(connectServer.value());

        startBroker();
        connectServer = oServer.waitConnectionMsg(3 * alivePeriod);
        ASSERT_TRUE(connectServer.has_value());
        ASSERT_TRUE(connectServer.value());

        // check rpc available
        clientAlivePeriod = std::chrono::milliseconds(client->getAlivePeriod());
        ASSERT_EQ(alivePeriod, clientAlivePeriod);
    }

    TEST_F(ServiceSynchro_UT, broker_disconnection_after_client_start) {
        startService();

        TestServiceObserver o1, o2;
        const TestServiceClient client1(o1), client2(o2);
        auto const alivePeriod = std::chrono::milliseconds(testservice::impl::TheTestServiceImpl.getAlivePeriod());
        auto const guard       = core::make_scope_exit([&]() {
            stopService();
            client1->detachAll(o1);
            client2->detachAll(o2);
        });

        // Wait connection
        auto connect1 = o1.waitConnectionMsg(100ms);
        ASSERT_TRUE(connect1.has_value() && connect1.value());

        stopBroker();

        // Wait disconnection
        connect1 = o1.waitConnectionMsg(alivePeriod * 3);
        ASSERT_TRUE(connect1.has_value() && !connect1.value());
    }

    TEST_F(ServiceSynchro_UT, broker_connection_after_client_start) {
        stopBroker();
        startService();

        TestServiceObserver o1(true), o2(true);
        TestServiceObserver oServer;
        testservice::impl::TheTestServiceImpl.attachAll(oServer);

        const TestServiceClient client1(o1), client2(o2);
        auto const alivePeriod = std::chrono::milliseconds(testservice::impl::TheTestServiceImpl.getAlivePeriod());
        auto const guard       = core::make_scope_exit([&]() {
            stopService();
            client1->detachAll(o1);
            client2->detachAll(o2);
        });

        // check no connection
        auto connectServer = oServer.waitConnectionMsg(3 * alivePeriod);
        ASSERT_FALSE(connectServer.has_value());

        auto connect1 = o1.waitConnectionMsg(alivePeriod * 3);
        ASSERT_FALSE(connect1.has_value());

        auto connect2 = o2.waitConnectionMsg(alivePeriod * 3);
        ASSERT_FALSE(connect2.has_value());

        startBroker();

        // Wait connection
        connectServer = oServer.waitConnectionMsg(3 * alivePeriod);
        ASSERT_TRUE(connectServer.has_value());
        ASSERT_TRUE(connectServer.value());

        connect1 = o1.waitConnectionMsg(alivePeriod * 3);
        ASSERT_TRUE(connect1.has_value() && connect1.value());

        connect2 = o2.waitConnectionMsg(alivePeriod * 3);
        ASSERT_TRUE(connect2.has_value() && connect1.value());

        // check receive event
        testservice::impl::TheTestServiceImpl.notifyDummyEvent();
        ASSERT_TRUE(o1.waitDummy(alivePeriod * 3));
        ASSERT_TRUE(o2.waitDummy(alivePeriod * 3));

        for (size_t index = 0; index < 2; ++index) {
            std::cout << std::endl;
            std::cout << "iteration #" << index + 1 << std::endl;
            std::cout << std::endl;

            stopBroker();
            std::this_thread::sleep_for(100ms);
            std::cout << "broker stopped" << std::endl;

            connectServer = oServer.waitConnectionMsg(3 * alivePeriod);
            ASSERT_TRUE(connectServer.has_value());
            ASSERT_FALSE(connectServer.value());

            connect1 = o1.waitConnectionMsg(alivePeriod * 3);
            ASSERT_TRUE(connect1.has_value());
            ASSERT_FALSE(connect1.value());

            connect2 = o2.waitConnectionMsg(alivePeriod * 3);
            ASSERT_TRUE(connect2.has_value());
            ASSERT_FALSE(connect2.value());

            startBroker();
            std::cout << "broker started" << std::endl;

            connectServer = oServer.waitConnectionMsg(3 * alivePeriod);
            ASSERT_TRUE(connectServer.has_value());
            ASSERT_TRUE(connectServer.value());

            connect1 = o1.waitConnectionMsg(alivePeriod * 3);
            ASSERT_TRUE(connect1.has_value());
            ASSERT_TRUE(connect1.value());

            connect2 = o2.waitConnectionMsg(alivePeriod * 3);
            ASSERT_TRUE(connect2.has_value());
            ASSERT_TRUE(connect2.value());

            // check events available
            testservice::impl::TheTestServiceImpl.notifyDummyEvent();
            std::this_thread::sleep_for(1s);

            ASSERT_TRUE(o1.waitDummy(alivePeriod * 3));
            ASSERT_TRUE(o2.waitDummy(alivePeriod * 3));
            std::cout << "evt dummy!" << std::endl;

            connect1 = o1.waitConnectionMsg(alivePeriod * 3);
            ASSERT_FALSE(connect1.has_value());
        }
    }

} // namespace NS_OSBASE::application::ut
