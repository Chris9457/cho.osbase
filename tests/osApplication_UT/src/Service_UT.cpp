
#include "BaseService_UT.h"
#include "testservice.h"
#include "TestServiceImpl/TestServiceImpl.h"

using namespace std::chrono_literals;

namespace nscore = cho::osbase::core;

namespace cho::osbase::application::ut {

    class Service_UT : public TService_UT<testservice::api::ITestService, testservice::impl::TestServiceImpl> {
    protected:
        class TestServiceObserver
            : public nscore::Observer<testservice::api::TextUpdatedMsg, testservice::api::PositionUpdatedMsg, RuntimeErrorMsg> {
        public:
            void update(const nscore::Observable &, const testservice::api::TextUpdatedMsg &msg) override {
                if (m_bTextUpdatedMsg)
                    return;

                m_bTextUpdatedMsg = true;
                m_promiseTextUpdatedMsg.set_value(msg);
            }

            void update(const core::Observable &, const testservice::api::PositionUpdatedMsg &msg) override {
                if (m_bPositionUpdatedMsg)
                    return;

                m_bPositionUpdatedMsg = true;
                m_promisePositionUpdatedMsg.set_value(msg);
            }

            void update(const core::Observable &, const RuntimeErrorMsg &msg) override {
                m_promiseRuntimeErrorData.set_value(msg.data);
            }

            auto waitTextUpdatedMsg(const std::chrono::milliseconds &timeout = BaseService_UT::getTimeout(100)) {
                return waitData<testservice::api::TextUpdatedMsg>(m_promiseTextUpdatedMsg, timeout);
            }

            auto waitPositionUpdatedMsg(const std::chrono::milliseconds &timeout = BaseService_UT::getTimeout(100)) {
                return waitData<testservice::api::PositionUpdatedMsg>(m_promisePositionUpdatedMsg, timeout);
            }

            auto waitRuntimeErrorData(const std::chrono::milliseconds &timeout = BaseService_UT::getTimeout(100)) {
                return waitData<RuntimeErrorData>(m_promiseRuntimeErrorData, timeout);
            }

            bool waitAlive(const std::chrono::milliseconds &timeout = 100ms) {
                std::unique_lock lock(m_mutMsg);
                m_bAlive = m_cvAlive.wait_for(lock, timeout) == std::cv_status::no_timeout;
                lock.unlock();
                return m_bAlive;
            }

        private:
            template <typename T>
            std::optional<T> waitData(std::promise<T> &promise, const std::chrono::milliseconds &timeout) {
                auto const guard = nscore::make_scope_exit([&promise]() { std::promise<T>().swap(promise); });
                if (auto futData = promise.get_future(); futData.wait_for(timeout) == std::future_status::ready) {
                    return futData.get();
                }

                return {};
            }

            bool m_bTextUpdatedMsg = false;
            std::promise<testservice::api::TextUpdatedMsg> m_promiseTextUpdatedMsg;
            bool m_bPositionUpdatedMsg = false;
            std::promise<testservice::api::PositionUpdatedMsg> m_promisePositionUpdatedMsg;
            std::promise<RuntimeErrorData> m_promiseRuntimeErrorData;
            std::promise<void> m_promiseAlive;

            std::mutex m_mutMsg;
            std::condition_variable m_cvAlive;
            bool m_bAlive = false;
        };

        Service_UT() : TService_UT([]() { return testservice::api::makeStub(); }) {
        }
    };

    TEST_F(Service_UT, checkMessaging) {
        TestServiceObserver o;
        getStub()->attachAll(o);

        auto const txt = getStub()->getText();
        ASSERT_TRUE(getStub()->setText("myText2", 45, 2.36));
        auto textUpdatedMsg = o.waitTextUpdatedMsg();
        ASSERT_TRUE(textUpdatedMsg.has_value());

        getStub()->setPosition({ 1.6, 2.4, 3.5 }, testservice::api::EPosition::Absolute);
        getStub()->setPosition({ 1.7, 2.6, 13.5 }, testservice::api::EPosition::Relative);

        auto const position = getStub()->getPosition();
        auto positions      = getStub()->getPositions();

        positions.push_back({ 44.33, 55.6, 99.9 });
        getStub()->setPositions(positions, testservice::api::EPosition::Absolute);
        auto msgPositionUpdatedMsg = o.waitPositionUpdatedMsg();
        ASSERT_TRUE(msgPositionUpdatedMsg.has_value());

        auto positions2 = getStub()->getPositions();

        auto buffer = data::makeAsyncData<testservice::api::Positions>();
        buffer.set(positions);
        auto booleansEndpoint       = getStub()->setPositionsAsync(buffer);
        auto const booleans         = booleansEndpoint.get();
        auto const expectedBooleans = std::vector{ true, false };
        ASSERT_EQ(expectedBooleans, booleans);
    }

    TEST_F(Service_UT, checkCrash) {
        TestServiceObserver o;
        getStub()->attachAll(o);

        ASSERT_THROW(getStub()->invokeCrash(testservice::api::sub2::enum1::e1), ServiceException);
        auto const runtimeError = o.waitRuntimeErrorData();
        ASSERT_TRUE(runtimeError.has_value());
    }

} // namespace cho::osbase::application::ut
