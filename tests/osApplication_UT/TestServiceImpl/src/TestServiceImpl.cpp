/// \brief Implementation of the service ITestService

#include "TestServiceImpl/TestServiceImpl.h"
#include "osApplication/ServiceConfiguration.h"

namespace nsdata = NS_OSBASE::data;

namespace testservice::impl {

    /*
     * \class TestServiceImpl::SagitalViewDelegate
     */
    class TestServiceImpl::SagitalViewDelegate : public nsdata::IDataExchange::IDelegate {
    public:
        void onDataReceived(nsdata::ByteBuffer &&) override {
        }

        void onConnected(const bool connected) override {
            if (connected) {
                TheTestServiceImpl.sendSagitalView();
            }
        }

        void onFailure(std::string &&) override {
        }
    };

    /*
     * \class TestServiceImpl
     */
    TestServiceImpl::TestServiceImpl()
        : ITestServiceSkeleton(
              NS_OSBASE::application::TheServiceConfiguration.getBrokerUri(), NS_OSBASE::application::TheServiceConfiguration.getRealm()) {
    }

    std::string TestServiceImpl::getText() const {
        return "test";
    }

    bool TestServiceImpl::setText(std::string text, int, double) {
        publishTextUpdatedMsg();
        return true;
    }

    api::Position TestServiceImpl::getPosition() {
        if (!m_positions.empty())
            return m_positions.front();

        return {};
    }

    void TestServiceImpl::setPosition(api::Position position, api::EPosition ePos) {
        std::ignore = ePos;
        m_positions.push_back(position);
        publishPositionUpdatedMsg(position);
    }

    void TestServiceImpl::setPositions(std::vector<api::Position> positions, api::EPosition ePos) {
        std::ignore = ePos;
        m_positions = positions;
    }

    api::Mat4 TestServiceImpl::getTransfo(api::Vec4 origin, api::Vec4 dest) {
        std::ignore = origin;
        std::ignore = dest;
        return {};
    }

    void TestServiceImpl::invokeCrash(api::sub2::enum1) {
        *(int *)0 = 0;
    }

    NS_OSBASE::data::AsyncPagedData<std::vector<bool>> TestServiceImpl::setPositionsAsync(
        NS_OSBASE::data::AsyncData<api::Positions> buffer) {
        setPositions(buffer.get(), api::EPosition::Absolute);

        return m_booleansProvider;
    }

    void TestServiceImpl::wait(unsigned long long timeoutMs) {
        m_bWaitStarted = true;
        m_cvWaitStarted.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(timeoutMs));
    }

    void TestServiceImpl::resetWait() {
        m_bWaitStarted = false;
    }

    void TestServiceImpl::waitForStartWait() {
        std::unique_lock lock(m_mutWait);
        m_cvWaitStarted.wait(lock, [this]() { return m_bWaitStarted; });
    }

    void TestServiceImpl::notifyDummyEvent() const {
        getTaskLoop()->push([this]() {
            publishDummyEvent();
            std::this_thread::sleep_for(std::chrono::milliseconds(getAlivePeriod() * 15 / 10));
        });
    }

    std::vector<api::Position> TestServiceImpl::getPositions() {
        return m_positions;
    }

    void TestServiceImpl::doConnect() {
        m_booleansProvider.create();
        m_booleansProvider.set({ true, false });
    }

    void TestServiceImpl::doDisconnect() {
        m_booleansProvider.reset();
    }

    void TestServiceImpl::sendSagitalView() const {
        static auto const buffer = []() -> nsdata::ByteBuffer { return { std::byte{ 0x01 }, std::byte{ 0x02 }, std::byte{ 0x03 } }; }();
    }

    void TestServiceImpl::onDataReceived(api::Positions &&positions) {
        setPositions(positions, api::EPosition::Absolute);
    }

} // namespace testservice::impl
