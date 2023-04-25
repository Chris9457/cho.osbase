// \brief Unit test for the logs

#include "osData/Log.h"
#include "gtest/gtest.h"
#include <fstream>
#include <future>

using namespace std::chrono_literals;

namespace NS_OSBASE::data::ut {

    class Logger_UT : public testing::Test {
        class LogOutputString : public ILogOutput {
        public:
            LogOutputString(std::string &str, size_t &count) : m_str(str), m_count(count) {
            }

            void log(std::string &&msg) override {
                m_str = msg;
                ++m_count;
            }

            void flush(std::ostream &os) override {
                os << m_str;
            };

        private:
            std::string &m_str;
            size_t &m_count;
        };

    protected:
        void SetUp() override {
            Test::SetUp();

            auto const pLogOutputGroup = makeLogOutputGroup();
            pLogOutputGroup->add(makeLogOutputConsole());
            pLogOutputGroup->add(makeLogOutputDebug());
            pLogOutputGroup->add(std::make_shared<LogOutputString>(m_strLog, m_count));
            TheLogger.setLogOutput(pLogOutputGroup);
            TheLogger.setChannelMask(static_cast<unsigned long long>(-1));
            TheLogger.setSeverityMask(-1);
        }

        void TearDown() override {
            TheLogger.setLogOutput(nullptr);
            Test::TearDown();
        }

        const std::string &getStrLog() {
            return m_strLog;
        }

        size_t getLogCount() const {
            return m_count;
        }

        template <typename T>
        T getKeyMessage(const std::string &key) {
            auto const pStream = core::makeJsonStream(std::stringstream(m_strLog));
            return pStream->getKeyValue(key, T{});
        }

        bool isKeyExist(const std::string &key) const {
            auto const pStream = core::makeJsonStream(std::stringstream(m_strLog));
            return pStream->isKeyExist(key);
        }

        unsigned long long getChannel() {
            return LoggerChannel::fromString(getKeyMessage<std::string>(LOGGER_KEY_CHANNEL));
        }

    private:
        std::string m_strLog;
        size_t m_count = 0;
    };

    struct TestObject {
        int value = 0;
        std::string strValue;

        bool operator==(const TestObject &other) const {
            return value == other.value && strValue == other.strValue;
        }
    };

    TEST_F(Logger_UT, outputNullptr) {
        const std::string key      = "my_key";
        const std::string strValue = "value";

        TheLogger.setLogOutput(nullptr);
        TheLogger << LoggerMessage<std::string>(key, strValue) << LoggerEnd();

        ASSERT_TRUE(getKeyMessage<std::string>(key).empty());
    }

    TEST_F(Logger_UT, addMessage_Generic) {
        const std::string key      = "my_key";
        const std::string strValue = "value";

        TheLogger << LoggerMessage<std::string>(key, strValue) << TheLogger << LoggerEnd();
        ASSERT_EQ(strValue, getKeyMessage<std::string>(key));
    }

    TEST_F(Logger_UT, addMessageSeverity) {
        TheLogger << LoggerSeverity(Severity::error) << LoggerEnd();
        ASSERT_EQ(Severity::error, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
    }

    TEST_F(Logger_UT, addMessageChannel) {
        TheLogger << LoggerChannel(5) << LoggerEnd();
        ASSERT_EQ(5, getChannel());
    }

    TEST_F(Logger_UT, addMessageContent) {
        const std::string content = "test content";

        TheLogger << LoggerContent(content) << LoggerEnd();
        ASSERT_EQ(content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << "test content" << LoggerEnd();
        ASSERT_EQ("test content", getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << content << LoggerEnd();
        ASSERT_EQ(content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
    }

    TEST_F(Logger_UT, addMessagechainedContents) {
        const std::string content = "test content";
        TheLogger << LoggerContent(content) << LoggerContent(content) << LoggerEnd();
        ASSERT_EQ(content + content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << "test content"
                  << "test content" << LoggerEnd();
        ASSERT_EQ("test contenttest content", getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << content << content << LoggerEnd();
        ASSERT_EQ(content + content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
    }

    TEST_F(Logger_UT, addMessagechainedArithmeticContents) {
        const std::string content = " test content ";

        TheLogger << content << (char)2 << content << LoggerEnd();
        ASSERT_EQ(content + std::to_string((char)2) + content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << content << (unsigned char)2 << content << LoggerEnd();
        ASSERT_EQ(content + std::to_string((unsigned char)2) + content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << content << (int)2 << content << LoggerEnd();
        ASSERT_EQ(content + std::to_string((int)2) + content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << content << (unsigned int)2 << content << LoggerEnd();
        ASSERT_EQ(content + std::to_string((unsigned int)2) + content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << content << (long)2 << content << LoggerEnd();
        ASSERT_EQ(content + std::to_string((long)2) + content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << content << (unsigned long)2 << content << LoggerEnd();
        ASSERT_EQ(content + std::to_string((unsigned long)2) + content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << content << (float)2 << content << LoggerEnd();
        ASSERT_EQ(content + std::to_string((float)2) + content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << content << (double)2 << content << LoggerEnd();
        ASSERT_EQ(content + std::to_string((double)2) + content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
    }

    TEST_F(Logger_UT, addMessageObject) {
        const TestObject o = { 1, "toto" };
        TheLogger << o << LoggerEnd();
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));

        TheLogger << TestObject{ 1, "toto" } << LoggerEnd();
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));
    }

    TEST_F(Logger_UT, addMessageSeverityChannelContentEnd) {
        const std::string content = "test content";

        TheLogger << LoggerSeverity(Severity::error) << LoggerChannel(5) << content << LoggerEnd();
        ASSERT_EQ(Severity::error, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ(5, getChannel());
        ASSERT_EQ(content, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        TheLogger << LoggerSeverity(Severity::error) << LoggerEnd();
        ASSERT_EQ(Severity::error, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ(0, getChannel());

        TheLogger << LoggerChannel(5) << LoggerEnd();
        ASSERT_EQ(static_cast<Severity>(0), getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ(5, getChannel());
    }

    TEST_F(Logger_UT, checkConcurrency) {
        auto checkLog = []() {
            TheLogger << LoggerSeverity(Severity::error);
            std::this_thread::sleep_for(100ms);
            TheLogger << LoggerEnd();
        };

        std::thread th1(checkLog);
        std::thread th2(checkLog);
        th1.join();
        th2.join();

        ASSERT_EQ(2, getLogCount());
    }

    TEST_F(Logger_UT, checkSeverityMask) {
        const std::string testContent = "test content";

        TheLogger.setSeverityMask(Severity::debug | Severity::trace);

        oslog::warning() << testContent << oslog::end();
        ASSERT_TRUE(getKeyMessage<std::string>(LOGGER_KEY_CONTENT).empty());

        oslog::debug() << testContent << oslog::end();
        ASSERT_EQ(testContent, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        oslog::trace() << testContent << oslog::end();
        ASSERT_EQ(testContent, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
    }

    TEST_F(Logger_UT, checkChannelMask) {
        const std::string testContent = "test content";

        auto constexpr f1 = 1ull << 5;
        auto constexpr f2 = 1ull << 7;

        TheLogger.setChannelMask(f1 | f2);

        oslog::warning(~(f1 | f2)) << testContent << oslog::end();
        ASSERT_TRUE(getKeyMessage<std::string>(LOGGER_KEY_CONTENT).empty());

        oslog::debug(f1) << testContent << oslog::end();
        ASSERT_EQ(testContent, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        oslog::trace(f2) << testContent << oslog::end();
        ASSERT_EQ(testContent, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));

        oslog::error() << testContent << oslog::end();
        ASSERT_EQ(testContent, getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
    }

    TEST_F(Logger_UT, checkLogApi) {
        constexpr int value    = 3;
        const TestObject o     = { 2, "4" };
        auto constexpr channel = 1ull << 57; // 0x12ffaa668d667b44;
        oslog::info() << "test content: " << 3 << o << oslog::end();
        ASSERT_EQ(Severity::info, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ("test content: " + std::to_string(3), getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
        ASSERT_EQ(0, getChannel());
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));

        oslog::info(channel) << "test content: " << 3 << o << oslog::end();
        ASSERT_EQ(Severity::info, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ("test content: " + std::to_string(3), getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
        ASSERT_EQ(channel, getChannel());
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));

        oslog::warning() << "test content: " << 3 << o << oslog::end();
        ASSERT_EQ(Severity::warning, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ("test content: " + std::to_string(3), getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
        ASSERT_EQ(0, getChannel());
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));

        oslog::warning(channel) << "test content: " << value << o << oslog::end();
        ASSERT_EQ(Severity::warning, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ(channel, getChannel());
        ASSERT_EQ("test content: " + std::to_string(value), getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));

        oslog::error() << "test content: " << 3 << o << oslog::end();
        ASSERT_EQ(Severity::error, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ("test content: " + std::to_string(3), getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
        ASSERT_EQ(0, getChannel());
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));

        oslog::error(channel) << "test content: " << value << o << oslog::end();
        ASSERT_EQ(Severity::error, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ(channel, getChannel());
        ASSERT_EQ("test content: " + std::to_string(value), getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));

        oslog::debug() << "test content: " << 3 << o << oslog::end();
        ASSERT_EQ(Severity::debug, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ("test content: " + std::to_string(3), getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
        ASSERT_EQ(0, getChannel());
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));

        oslog::debug(channel) << "test content: " << value << o << oslog::end();
        ASSERT_EQ(Severity::debug, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ(channel, getChannel());
        ASSERT_EQ("test content: " + std::to_string(value), getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));

        oslog::trace() << "test content: " << 3 << o << oslog::end();
        ASSERT_EQ(Severity::trace, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ("test content: " + std::to_string(3), getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
        ASSERT_EQ(0, getChannel());
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));

        oslog::trace(channel) << "test content: " << value << o << oslog::end();
        ASSERT_EQ(Severity::trace, getKeyMessage<Severity>(LOGGER_KEY_SEVERITY));
        ASSERT_EQ(channel, getChannel());
        ASSERT_EQ("test content: " + std::to_string(value), getKeyMessage<std::string>(LOGGER_KEY_CONTENT));
        ASSERT_EQ(o, getKeyMessage<TestObject>(LOGGER_KEY_OBJECT));
    }
} // namespace NS_OSBASE::data::ut
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::data::ut::TestObject, value, strValue);
