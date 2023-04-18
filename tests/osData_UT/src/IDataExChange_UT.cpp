// \brief Implementation tests of IDataExchange

#include "osData/ByteBuffer.h"
#include "osData/IDataExchange.h"
#include "osData/Log.h"
#include "osDataImpl/osDataImpl.h"

#include "gtest/gtest.h"

#include <chrono>
#include <future>
#include <optional>
#include <random>

using namespace std::chrono_literals;

namespace cho::osbase::data::ut {

    class DataExchange_UT : public testing::Test {
    protected:
        class DataExchangeDelegate;
        using DataExchangeDelegatePtr = std::shared_ptr<DataExchangeDelegate>;

        class DataExchangeDelegate final : public IDataExchange::IDelegate {
        public:
            void onDataReceived(ByteBuffer &&buffer) override {
                m_data.set_value(std::forward<ByteBuffer>(buffer));
            }

            void onConnected(const bool connected) override {
                m_bConnected.set_value(connected);
            }

            void onFailure(std::string &&strFailure) override {
                m_failureMessage.set_value(std::forward<std::string>(strFailure));
            }

            auto getConnectionStatus() {
                std::optional<bool> connectionStatus;
                try {
                    auto future = m_bConnected.get_future();
                    if (const auto status = future.wait_for(100ms); status == std::future_status::ready) {
                        connectionStatus = future.get();
                    }
                    std::promise<bool>().swap(m_bConnected);
                } catch (const std::exception &e) {
                    oslog::error() << "exception in getConnectionStatus():" << e.what() << oslog::end();
                }
                return connectionStatus;
            }

            auto getData() {
                std::optional<ByteBuffer> optionalBuffer;
                try {
                    auto future = m_data.get_future();
                    if (const auto status = future.wait_for(100ms); status == std::future_status::ready) {
                        optionalBuffer = future.get();
                    }
                    std::promise<ByteBuffer>().swap(m_data);
                } catch (const std::exception &e) {
                    oslog::error() << "exception in getData():" << e.what() << oslog::end();
                }
                return optionalBuffer;
            }

            auto getFailure() {
                std::optional<std::string> optionalFailure;
                try {
                    auto future = m_failureMessage.get_future();
                    if (const auto status = future.wait_for(100ms); status == std::future_status::ready) {
                        optionalFailure = future.get();
                    }
                    std::promise<std::string>().swap(m_failureMessage);
                } catch (const std::exception &e) {
                    oslog::error() << "exception in getFailure():" << e.what() << oslog::end();
                }
                return optionalFailure;
            }

        private:
            std::promise<bool> m_bConnected;
            std::promise<ByteBuffer> m_data;
            std::promise<std::string> m_failureMessage;
        };

        auto getEndPointCreateConnectionStatus() const {
            return getEndPointCreateDelegate()->getConnectionStatus();
        }

        auto getEndPointOpenConnectionStatus() const {
            return getEndPointOpenDelegate()->getConnectionStatus();
        }

        auto getEndPointCreateData() const {
            return getEndPointCreateDelegate()->getData();
        }

        auto getEndPointOpenData() const {
            return getEndPointOpenDelegate()->getData();
        }

        auto getEndPointOpenFailure() const {
            return getEndPointOpenDelegate()->getFailure();
        }

        auto getEndPointCreateFailure() const {
            return getEndPointCreateDelegate()->getFailure();
        }

        static auto generateBuffer(int bufferSize) {
            const ByteBuffer buffer = [&bufferSize]() {
                std::random_device randomDevice;
                std::default_random_engine randomEngine(randomDevice());
                const std::uniform_int<unsigned char> uniformDistribution(0, 255);

                ByteBuffer buffer;
                for (size_t index = 0; index < bufferSize; ++index) {
                    buffer.push_back(static_cast<std::byte>((uniformDistribution(randomEngine))));
                }

                return buffer;
            }();
            return buffer;
        }

        void SetUp() override {
            m_pEndPointCreate         = makeDataExchange();
            m_pEndPointCreateDelegate = std::make_shared<DataExchangeDelegate>();
            ASSERT_NO_THROW(getEndPointCreate()->setDelegate(getEndPointCreateDelegate()));
            m_pEndPointOpen         = makeDataExchange();
            m_pEndPointOpenDelegate = std::make_shared<DataExchangeDelegate>();
            ASSERT_NO_THROW(getEndPointOpen()->setDelegate(getEndPointOpenDelegate()));
            ASSERT_NO_FATAL_FAILURE(
                createOpenWorkflow(getEndPointCreate(), getEndPointCreateDelegate(), getEndPointOpen(), getEndPointOpenDelegate()));
        }

        void TearDown() override {
            ASSERT_NO_THROW(getEndPointOpen()->close());
            ASSERT_NO_THROW(getEndPointCreate()->destroy());
        }

        void createOpenWorkflow(IDataExchangePtr pEndPointCreate,
            DataExchangeDelegatePtr pCreateDelegate,
            IDataExchangePtr pEndPointOpen,
            DataExchangeDelegatePtr pOpenDelegate) {
            ASSERT_NO_THROW(pEndPointCreate->create());
            ASSERT_NO_THROW(pEndPointOpen->open(pEndPointCreate->getUriOfCreator()));
            ASSERT_TRUE(pCreateDelegate->getConnectionStatus().value());
            ASSERT_TRUE(pOpenDelegate->getConnectionStatus().value());
            ASSERT_FALSE(pCreateDelegate->getFailure().has_value());
            ASSERT_FALSE(pOpenDelegate->getFailure().has_value());
            ASSERT_TRUE(pEndPointCreate->isWired());
            ASSERT_TRUE(pEndPointOpen->isWired());
        }

        void closeAndReopenWorkflow(IDataExchangePtr pEndPointCreate,
            DataExchangeDelegatePtr pCreateDelegate,
            IDataExchangePtr pEndPointOpen,
            DataExchangeDelegatePtr pOpenDelegate) {
            ASSERT_NO_THROW(pEndPointOpen->close());
            ASSERT_FALSE(pCreateDelegate->getFailure().has_value());
            ASSERT_FALSE(pOpenDelegate->getFailure().has_value());
            ASSERT_FALSE(pCreateDelegate->getConnectionStatus().value());
            ASSERT_FALSE(pOpenDelegate->getConnectionStatus().value());
            ASSERT_NO_THROW(pEndPointOpen->open(pEndPointCreate->getUriOfCreator()));
            ASSERT_TRUE(pCreateDelegate->getConnectionStatus().value());
            ASSERT_TRUE(pOpenDelegate->getConnectionStatus().value());
            ASSERT_FALSE(pCreateDelegate->getFailure().has_value());
            ASSERT_FALSE(pOpenDelegate->getFailure().has_value());
            ASSERT_TRUE(pEndPointCreate->isWired());
            ASSERT_TRUE(pEndPointOpen->isWired());
        }

        void destroyWithoutCloseRecreateAndReopenWorkflow(IDataExchangePtr pEndPointCreate,
            DataExchangeDelegatePtr pCreateDelegate,
            IDataExchangePtr pEndPointOpen,
            DataExchangeDelegatePtr pOpenDelegate) {
            ASSERT_NO_THROW(pEndPointCreate->destroy());
            ASSERT_FALSE(pCreateDelegate->getFailure().has_value());
            ASSERT_FALSE(pOpenDelegate->getFailure().has_value());
            ASSERT_FALSE(pCreateDelegate->getConnectionStatus().value());
            ASSERT_FALSE(pOpenDelegate->getConnectionStatus().value());
            ASSERT_NO_THROW(pEndPointCreate->create());
            ASSERT_NO_THROW(pEndPointOpen->open(pEndPointCreate->getUriOfCreator()));
            ASSERT_TRUE(pCreateDelegate->getConnectionStatus().value());
            ASSERT_TRUE(pOpenDelegate->getConnectionStatus().value());
            ASSERT_FALSE(pCreateDelegate->getFailure().has_value());
            ASSERT_FALSE(pOpenDelegate->getFailure().has_value());
            ASSERT_TRUE(pEndPointCreate->isWired());
            ASSERT_TRUE(pEndPointOpen->isWired());
        }

        void pushFullDuplexWorkflow(IDataExchangePtr pEndPointCreate,
            DataExchangeDelegatePtr pCreateDelegate,
            IDataExchangePtr pEndPointOpen,
            DataExchangeDelegatePtr pOpenDelegate) {
            auto buffer = generateBuffer(100);
            ASSERT_NO_THROW(pEndPointOpen->push(buffer));
            ASSERT_EQ(buffer, pCreateDelegate->getData().value());
            ASSERT_NO_THROW(pEndPointCreate->push(buffer));
            ASSERT_EQ(buffer, pOpenDelegate->getData().value());
            ASSERT_FALSE(pCreateDelegate->getFailure().has_value());
            ASSERT_FALSE(pOpenDelegate->getFailure().has_value());
        }

        IDataExchangePtr getEndPointCreate() const {
            return m_pEndPointCreate;
        }

        DataExchangeDelegatePtr getEndPointCreateDelegate() const {
            return m_pEndPointCreateDelegate;
        }

        DataExchangeDelegatePtr getEndPointOpenDelegate() const {
            return m_pEndPointOpenDelegate;
        }

        IDataExchangePtr getEndPointOpen() const {
            return m_pEndPointOpen;
        }

        IDataExchangePtr m_pEndPointCreate;
        IDataExchangePtr m_pEndPointOpen;
        DataExchangeDelegatePtr m_pEndPointCreateDelegate;
        DataExchangeDelegatePtr m_pEndPointOpenDelegate;
    };

    static bool isIPv4Address(const std::string &address) {
        static const std::regex regex("[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+");
        return std::regex_match(address, regex);
    }

    TEST_F(DataExchange_UT, instantiateEndPoint) {
        const auto pEndPoint        = makeDataExchange();
        const auto pEndPoindelegate = std::make_shared<DataExchangeDelegate>();
        pEndPoint->setDelegate(pEndPoindelegate);
        ASSERT_NE(pEndPoint, nullptr);
        ASSERT_EQ(pEndPoint->getAccessType(), IDataExchange::AccessType::CreateOpen);
        ASSERT_FALSE(pEndPoindelegate->getFailure().has_value());
    }

    TEST_F(DataExchange_UT, createEndPoint) {
        const auto pEndPoint        = makeDataExchange();
        const auto pEndPoindelegate = std::make_shared<DataExchangeDelegate>();
        pEndPoint->setDelegate(pEndPoindelegate);
        ASSERT_NO_THROW(pEndPoint->create());
        ASSERT_NE(type_cast<std::string>(pEndPoint->getUriOfCreator()).size(), 0);
        ASSERT_TRUE(pEndPoint->getUriOfCreator().authority.has_value());
        EXPECT_TRUE(isIPv4Address(pEndPoint->getUriOfCreator().authority->host));
        EXPECT_NE(std::string{ pEndPoint->getUriOfCreator().authority->host }, "127.0.0.1");
        ASSERT_FALSE(pEndPoindelegate->getFailure().has_value());
    }

    TEST_F(DataExchange_UT, destroyNotCreatedEndPoint) {
        const auto pEndPoint = makeDataExchange();
        ASSERT_NO_THROW(pEndPoint->destroy());
    }

    TEST_F(DataExchange_UT, destroyCreatedEndPoint) {
        const auto pEndPoint        = makeDataExchange();
        const auto pEndPoindelegate = std::make_shared<DataExchangeDelegate>();
        ASSERT_NO_THROW(pEndPoint->destroy());
        ASSERT_FALSE(pEndPoindelegate->getFailure().has_value());
    }

    TEST_F(DataExchange_UT, CreateEndPointAndOpenAllreadyOpenedEndPoint) {

        const auto pEndPointCreate         = makeDataExchange();
        const auto pEndPointOpen           = makeDataExchange();
        const auto pEndPointCreateDelegate = std::make_shared<DataExchangeDelegate>();
        const auto pEndPointOpenDelegate   = std::make_shared<DataExchangeDelegate>();

        pEndPointCreate->setDelegate(pEndPointCreateDelegate);
        pEndPointOpen->setDelegate(pEndPointOpenDelegate);
        ASSERT_FALSE(pEndPointCreateDelegate->getConnectionStatus().has_value());
        ASSERT_FALSE(pEndPointOpenDelegate->getConnectionStatus().has_value());
        ASSERT_FALSE(pEndPointOpenDelegate->getFailure().has_value());
        ASSERT_FALSE(pEndPointCreateDelegate->getFailure().has_value());

        ASSERT_NO_FATAL_FAILURE(createOpenWorkflow(pEndPointCreate, pEndPointCreateDelegate, pEndPointOpen, pEndPointOpenDelegate));
    }

    TEST_F(DataExchange_UT, CloseNotOpenedEndPoint) {
        const auto pEndPointOpen = makeDataExchange();
        ASSERT_NO_THROW(pEndPointOpen->close());
    }

    TEST_F(DataExchange_UT, OpenTwiceTheSameUriFailsAndLastOpenedEndPointIsWired) {
        auto const pEndPointOpen         = makeDataExchange();
        auto const pEndPointOpenDelegate = std::make_shared<DataExchangeDelegate>();
        pEndPointOpen->setDelegate(pEndPointOpenDelegate);
        ASSERT_NO_THROW(pEndPointOpen->open(getEndPointCreate()->getUriOfCreator()));
        ASSERT_TRUE(pEndPointOpenDelegate->getFailure().has_value());
        ASSERT_TRUE(getEndPointOpen()->isWired());
        ASSERT_TRUE(getEndPointCreate()->isWired());
    }

    TEST_F(DataExchange_UT, CallOpenTwiceAndThrow) {
        ASSERT_THROW(getEndPointOpen()->open(getEndPointCreate()->getUriOfCreator()), cho::osbase::data::DataExchangeException);
        ASSERT_TRUE(getEndPointOpen()->isWired());
        ASSERT_TRUE(getEndPointCreate()->isWired());
    }

    TEST_F(DataExchange_UT, CallCreateTwiceAndThrow) {
        const ByteBuffer buffer = generateBuffer(100);
        ASSERT_THROW(getEndPointCreate()->create(), cho::osbase::data::DataExchangeException);
        ASSERT_NO_THROW(getEndPointOpen()->push(buffer));
        ASSERT_EQ(buffer, getEndPointCreateData().value());
    }

    TEST_F(DataExchange_UT, SwapEndPoints) {
        getEndPointOpen()->close();
        getEndPointCreate()->destroy();
        ASSERT_FALSE(getEndPointOpenDelegate()->getConnectionStatus().value());
        ASSERT_FALSE(getEndPointCreateDelegate()->getConnectionStatus().value());
        ASSERT_FALSE(getEndPointCreateDelegate()->getFailure().has_value());
        ASSERT_FALSE(getEndPointOpenDelegate()->getFailure().has_value());

        ASSERT_NO_FATAL_FAILURE(
            createOpenWorkflow(getEndPointOpen(), getEndPointOpenDelegate(), getEndPointCreate(), getEndPointCreateDelegate()));
        ASSERT_NO_FATAL_FAILURE(
            pushFullDuplexWorkflow(getEndPointOpen(), getEndPointOpenDelegate(), getEndPointCreate(), getEndPointCreateDelegate()));
    }

    TEST_F(DataExchange_UT, EndPointClosedAndReOpenedAndPushIsFunctionalAfter) {
        for (auto count = 0; count < 5; ++count) {
            ASSERT_NO_FATAL_FAILURE(
                closeAndReopenWorkflow(getEndPointCreate(), getEndPointCreateDelegate(), getEndPointOpen(), getEndPointOpenDelegate()));
            ASSERT_NO_FATAL_FAILURE(
                pushFullDuplexWorkflow(getEndPointCreate(), getEndPointCreateDelegate(), getEndPointOpen(), getEndPointOpenDelegate()));
        }
    }

    TEST_F(DataExchange_UT, EndPointDestroyeWithoutClosedAndReCreatedPushIsFunctionalAfter) {
        for (auto count = 0; count < 5; ++count) {
            ASSERT_NO_FATAL_FAILURE(destroyWithoutCloseRecreateAndReopenWorkflow(
                getEndPointCreate(), getEndPointCreateDelegate(), getEndPointOpen(), getEndPointOpenDelegate()));
            ASSERT_NO_FATAL_FAILURE(
                pushFullDuplexWorkflow(getEndPointCreate(), getEndPointCreateDelegate(), getEndPointOpen(), getEndPointOpenDelegate()));
        }
    }

    TEST_F(DataExchange_UT, PushOnClosedChannelAndThrow) {
        const ByteBuffer buffer = generateBuffer(1000);
        ASSERT_NO_THROW(getEndPointOpen()->close());
        ASSERT_FALSE(getEndPointCreateConnectionStatus().value());
        ASSERT_FALSE(getEndPointOpenConnectionStatus().value());
        ASSERT_THROW(getEndPointOpen()->push(buffer), cho::osbase::data::DataExchangeException);
    }

    TEST_F(DataExchange_UT, PushOnDestroyedChannelAndThrow) {
        const ByteBuffer buffer = generateBuffer(1000);
        ASSERT_NO_THROW(getEndPointCreate()->destroy());
        ASSERT_FALSE(getEndPointCreateConnectionStatus().value());
        ASSERT_FALSE(getEndPointOpenConnectionStatus().value());
        ASSERT_THROW(getEndPointCreate()->push(buffer), cho::osbase::data::DataExchangeException);
    }

    TEST_F(DataExchange_UT, PushBigChunkOnOspenChannel) {
        const ByteBuffer buffer  = generateBuffer(500000);
        const clock_t begin_time = clock();
        for (auto count = 0; count < 10; ++count) {
            ASSERT_NO_THROW(getEndPointOpen()->push(buffer));
            ASSERT_EQ(buffer, getEndPointCreateData().value());
            ASSERT_NO_THROW(getEndPointCreate()->push(buffer));
            ASSERT_EQ(buffer, getEndPointOpenData().value());
        }
        oslog::trace() << "duration for a 500 000 byte chunk sended 20 times " << static_cast<float>(clock() - begin_time) / CLOCKS_PER_SEC
                       << " seconds" << oslog::end();
    }
} // namespace cho::osbase::data::ut
