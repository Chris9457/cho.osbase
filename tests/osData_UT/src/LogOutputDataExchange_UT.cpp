// \brief Unit test for the logs

#include "osData/ILogOutput.h"
#include "osData/IDataExchange.h"
#include "gtest/gtest.h"
#include <fstream>
#include <future>

using namespace std::chrono_literals;

namespace cho::osbase::data::ut {

    class LogOutputDataExhange_UT : public testing::Test {
        class DataExchangeDelegate : public IDataExchange::IDelegate {
        public:
            DataExchangeDelegate(LogOutputDataExhange_UT &ut) : m_ut(ut) {
            }

            void onConnected(const bool connected) override {
                std::ignore = connected;
            }

            void onFailure(std::string &&failure) override {
                std::ignore = failure;
            }

            void onDataReceived(ByteBuffer &&buffer) override {
                m_ut.onDataReceived(std::forward<ByteBuffer>(buffer));
            }

        private:
            LogOutputDataExhange_UT &m_ut;
        };
        using DataExchangeDelegatePtr = std::shared_ptr<DataExchangeDelegate>;

    protected:
        void SetUp() override {
            Test::SetUp();

            m_pDataExchange         = makeDataExchange();
            m_pDataExchangeDelegate = std::make_shared<DataExchangeDelegate>(*this);
            m_pDataExchange->setDelegate(m_pDataExchangeDelegate);

            m_pDataExchange->create();
        }

        void TearDown() override {
            if (m_pDataExchange != nullptr) {
                m_pDataExchange->destroy();
            }

            Test::TearDown();
        }

        Uri getUri() const {
            return m_pDataExchange->getUriOfCreator();
        }

        IDataExchangePtr getDataExchange() const {
            return m_pDataExchange;
        }

        DataExchangeDelegatePtr getDataExchangeDelegate() const {
            return m_pDataExchangeDelegate;
        }

        void destroyDataExchange() {
            m_pDataExchange->destroy();
            m_pDataExchange.reset();
        }

        std::optional<ByteBuffer> waitData(const std::chrono::milliseconds &timeout = 100ms) {
            auto const guard = core::make_scope_exit([this]() { std::promise<ByteBuffer>().swap(m_promiseData); });
            if (auto futData = m_promiseData.get_future(); futData.wait_for(timeout) == std::future_status::ready) {
                return futData.get();
            }

            return {};
        }

    private:
        void onDataReceived(ByteBuffer &&buffer) {
            m_promiseData.set_value(buffer);
        }

        IDataExchangePtr m_pDataExchange;
        DataExchangeDelegatePtr m_pDataExchangeDelegate;
        std::promise<ByteBuffer> m_promiseData;
    };

    TEST_F(LogOutputDataExhange_UT, create) {
        auto pLog = makeLogOutputDataExchange(getUri());
        ASSERT_NE(nullptr, pLog);

        auto const pDataExchange = makeDataExchange();
        pLog                     = makeLogOutputDataExchange(pDataExchange);
        ASSERT_NE(nullptr, pLog);
    }

    TEST_F(LogOutputDataExhange_UT, log) {
        // log on a server
        auto pLog      = makeLogOutputDataExchange(getUri());
        auto const msg = "test";

        pLog->log(msg);
        auto const data = waitData();
        ASSERT_TRUE(data.has_value());
        ASSERT_EQ(msg, type_cast<std::string>(*data));

        // log on a client
        auto const msg2      = "test2";
        pLog                 = makeLogOutputDataExchange(getDataExchange());
        auto const pListener = makeDataExchange();
        pListener->setDelegate(getDataExchangeDelegate());
        pListener->open(getUri());
        auto const guard = core::make_scope_exit([pListener]() { pListener->close(); });
        pLog->log(msg2);
        auto const data2 = waitData();
        ASSERT_TRUE(data2.has_value());
        ASSERT_EQ(msg2, type_cast<std::string>(*data2));
    }

    TEST_F(LogOutputDataExhange_UT, flush) {
        auto const pLog = makeLogOutputDataExchange(getUri());
        destroyDataExchange(); // cut the channel before logging

        auto const msg = "test";
        pLog->log(msg);

        std::stringstream os;
        pLog->flush(os);

        std::string flushedMsg;
        std::getline(os, flushedMsg);
        ASSERT_EQ(flushedMsg, msg);
    }

} // namespace cho::osbase::data::ut
