// \brief Base classes for tests

#include "osData/Log.h"
#include "osData/IBroker.h"
#include "gtest/gtest.h"
#include <future>

namespace NS_OSBASE::application::ut {

    /*
     * \class BaseService_UT
     */
    class BaseService_UT : public testing::Test {
    public:
        static constexpr std::chrono::milliseconds getTimeout(unsigned int timeout) {
            return std::chrono::milliseconds(timeout * TIMEOUT_FACTOR);
        }

    protected:
        void SetUp() override {
            Test::SetUp();

            data::TheLogger.setLogOutput(data::makeLogOutputConsole());
            data::TheLogger.setChannelMask(data::OS_LOG_CHANNEL_DATA);
            data::TheLogger.setSeverityMask(static_cast<data::Severity_t>(data::Severity::error));
        }
    };

    /*
     * \class TService_UT
     */
    template <class TService, class TServiceImpl>
    class TService_UT : public BaseService_UT {
    protected:
        using TServicePtr = std::shared_ptr<TService>;
        using TMakeStub   = std::function<TServicePtr()>;

        TService_UT(const TMakeStub &makeStub) : m_makeStub(makeStub) {
        }

        void SetUp() override {
            BaseService_UT::SetUp();
            startBroker();
            startService();
            m_pStub = m_makeStub();
            m_pStub->connect();
        }

        void TearDown() override {
            m_pStub->disconnect();
            stopService();
            stopBtoker();
            BaseService_UT::TearDown();
        }

        static std::string getBrokerUrl() {
            return "127.0.0.1";
        }

        static constexpr unsigned short getBrokerPort() {
            return 8080;
        }

        auto getStub() const {
            return m_pStub;
        }

        void startBroker() {
            if (m_refCountBroker == 0) {
                m_pBroker = data::makeBroker();
                m_pBroker->start(getBrokerPort());
            }
            ++m_refCountBroker;
        }

        void stopBtoker() {
            if (m_refCountBroker == 0) {
                return;
            }
            --m_refCountBroker;
            if (m_refCountBroker == 0) {
                m_pBroker->stop();
                m_pBroker.reset();
            }
        }

        void startService() {
            if (m_refCountService == 0) {
                m_futRun = TServiceImpl::getInstance().runAsync();
                TServiceImpl::getInstance().connect();
            }
            ++m_refCountService;
        }

        void stopService() {
            if (m_refCountService == 0) {
                return;
            }
            --m_refCountService;
            if (m_refCountService == 0) {
                TServiceImpl::getInstance().disconnect();
                TServiceImpl::getInstance().stop();
                m_futRun.wait();
            }
        }

    private:
        data::IBrokerPtr m_pBroker;
        std::shared_ptr<TService> m_pStub;
        TMakeStub m_makeStub;
        std::shared_future<void> m_futRun;
        size_t m_refCountBroker  = 0;
        size_t m_refCountService = 0;
    };

} // namespace NS_OSBASE::application::ut
