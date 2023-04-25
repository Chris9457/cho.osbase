// \brief Unit test of PagedDataExchange

#include "osData/PagedDataExchange.h"
#include "gtest/gtest.h"
#include "osData/IDataExchange.h"
#include <future>

using namespace std::chrono_literals;

namespace NS_OSBASE::data::ut {

    class PagedDataExchange_UT : public testing::Test {
    protected:
        class DataExchangeDelegate : public IDataExchange::IDelegate {
        public:
            void onConnected(const bool connected) override {
                m_promiseConnected.set_value(connected);
            }

            auto waitConnected(const std::chrono::milliseconds &timeout = 100ms) {
                return waitExchange(m_promiseConnected, timeout);
            }

            void onFailure(std::string &&failure) override {
                std::ignore = failure;
            }

            void onDataReceived(ByteBuffer &&buffer) override {
                m_promiseDataReceived.set_value(std::move(buffer));
            }

            auto waitDataReceived(const std::chrono::milliseconds &timeout = 100ms) {
                return waitExchange(m_promiseDataReceived, timeout);
            }

        private:
            template <typename T>
            std::optional<T> waitExchange(std::promise<T> &promiseExchange, const std::chrono::milliseconds &timeout) {
                auto const guard = core::make_scope_exit([&promiseExchange]() { std::promise<T>().swap(promiseExchange); });
                if (auto futExchange = promiseExchange.get_future(); futExchange.wait_for(timeout) == std::future_status::ready) {
                    return futExchange.get();
                }

                return {};
            }

            std::promise<bool> m_promiseConnected;
            std::promise<ByteBuffer> m_promiseDataReceived;
        };

        static auto makeDataExchangeDelegate() {
            return std::make_shared<DataExchangeDelegate>();
        }
    };

    TEST_F(PagedDataExchange_UT, makers) {
        auto constexpr size = PagedDataExchange::s_headerSize + 1;
        {
            auto const pPagedDataExchanged = makePagedDataExchange(makeDataExchange(), size);
            ASSERT_NE(nullptr, pPagedDataExchanged);
            ASSERT_EQ(size, pPagedDataExchanged->getPageSize());
        }
        {
            auto const pPagedDataExchanged = makePagedDataExchange(size);
            ASSERT_NE(nullptr, pPagedDataExchanged);
            ASSERT_EQ(size, pPagedDataExchanged->getPageSize());
        }
        {
            auto const pPagedDataExchanged = makePagedDataExchange();
            ASSERT_NE(nullptr, pPagedDataExchanged);
            ASSERT_EQ(PagedDataExchange::s_websocketPageSize, pPagedDataExchanged->getPageSize());
        }
    }

    TEST_F(PagedDataExchange_UT, push) {
        auto constexpr maxSizeInBytes = PagedDataExchange::s_headerSize + 1;
        auto const pCreator           = makePagedDataExchange(maxSizeInBytes);
        pCreator->create();

        auto const pEndpoint = makePagedDataExchange();
        auto const pDelegate = makeDataExchangeDelegate();
        pEndpoint->setDelegate(pDelegate);
        pEndpoint->open(pCreator->getUriOfCreator());

        auto const isConnected = pDelegate->waitConnected();
        ASSERT_TRUE(isConnected.has_value());
        ASSERT_TRUE(isConnected.value());

        const std::string expectedText = "test";
        pCreator->push(type_cast<ByteBuffer>(expectedText));
        auto receivedData = pDelegate->waitDataReceived();
        ASSERT_TRUE(receivedData.has_value());
        ASSERT_EQ(expectedText, type_cast<std::string>(receivedData.value()));

        pCreator->push(type_cast<ByteBuffer>(expectedText));
        receivedData = pDelegate->waitDataReceived();
        ASSERT_TRUE(receivedData.has_value());
        ASSERT_EQ(expectedText, type_cast<std::string>(receivedData.value()));
    }

    TEST_F(PagedDataExchange_UT, push_big_buffer_size) {
        auto constexpr bufferSize = 2 * PagedDataExchange::s_websocketPageSize;
        constexpr size_t pageSize = PagedDataExchange::s_websocketPageSize;

        ByteBuffer buffer(bufferSize, std::byte{ 0x0A });

        // Check ko without pages - for too large buffer, the connection is closed
        {
            auto const pCreator = makeDataExchange();
            pCreator->create();

            auto const pEndpoint = makeDataExchange();
            auto const pDelegate = makeDataExchangeDelegate();
            pEndpoint->setDelegate(pDelegate);
            pEndpoint->open(pCreator->getUriOfCreator());

            auto isConnected = pDelegate->waitConnected();
            ASSERT_TRUE(isConnected.has_value());
            ASSERT_TRUE(isConnected.value());

            pCreator->push(buffer);
            isConnected = pDelegate->waitConnected();
            ASSERT_TRUE(isConnected.has_value());
            ASSERT_FALSE(isConnected.value());
        }

        // Check ok with pages
        {
            auto const pCreator = makePagedDataExchange(pageSize);
            pCreator->create();

            auto const pEndpoint = makePagedDataExchange();
            auto const pDelegate = makeDataExchangeDelegate();
            pEndpoint->setDelegate(pDelegate);
            pEndpoint->open(pCreator->getUriOfCreator());

            auto isConnected = pDelegate->waitConnected();
            ASSERT_TRUE(isConnected.has_value());
            ASSERT_TRUE(isConnected.value());

            pCreator->push(buffer);
            isConnected = pDelegate->waitConnected(1s);
            ASSERT_FALSE(isConnected.has_value()); // no connection lost, the message has been succesfuly pushed

            auto const receivedData = pDelegate->waitDataReceived();
            ASSERT_TRUE(receivedData.has_value());
            ASSERT_EQ(buffer, receivedData.value());
        }
    }
} // namespace NS_OSBASE::data::ut
