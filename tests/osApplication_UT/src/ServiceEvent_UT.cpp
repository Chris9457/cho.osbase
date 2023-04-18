// \brief Test of events

#include "BaseService_UT.h"
#include "TestEvent.h"
#include "osApplication_UT/TestEventServiceImpl.h"
#include "osApplication_UT/TestProcessServiceImpl.h"

namespace nsdata = cho::osbase::data;

namespace cho::osbase::application::ut {

    class ServiceEvent_UT : public TService_UT<event::TestEventService, TestEventServiceImpl> {
    protected:
        class EventObserver : public core::Observer<event::EvtNoDataMsg, event::EvtDataMsg> {
        public:
            void update(const core::Observable &, const event::EvtNoDataMsg &) override {
                m_promiseNoDataReceived.set_value();
            }

            bool waitNoDataEvtRceived(const std::chrono::milliseconds &timeout = getTimeout(100)) {
                auto const guard = core::make_scope_exit([this]() { decltype(m_promiseNoDataReceived)().swap(m_promiseNoDataReceived); });
                auto const futNoDataEvtReceived = m_promiseNoDataReceived.get_future();
                return futNoDataEvtReceived.wait_for(timeout) == std::future_status::ready;
            }

            void update(const core::Observable &, const event::EvtDataMsg &msg) override {
                m_promiseDataReceived.set_value(msg.data);
            }

            std::optional<int> waitDataEvtReceived(const std::chrono::milliseconds &timeout = getTimeout(100)) {
                auto const guard = core::make_scope_exit([this]() { decltype(m_promiseDataReceived)().swap(m_promiseDataReceived); });
                if (auto futDataEvetReceived = m_promiseDataReceived.get_future();
                    futDataEvetReceived.wait_for(timeout) == std::future_status::ready) {
                    return futDataEvetReceived.get();
                }

                return {};
            }

        private:
            std::promise<void> m_promiseNoDataReceived;
            std::promise<int> m_promiseDataReceived;
        };

        ServiceEvent_UT() : TService_UT<event::TestEventService, TestEventServiceImpl>([]() { return event::makeStub(); }) {
        }
    };

    TEST_F(ServiceEvent_UT, invokeNoDataEvt) {
        EventObserver o;
        getStub()->attachAll(o);

        getStub()->invoke(true);
        ASSERT_TRUE(o.waitNoDataEvtRceived());
        ASSERT_FALSE(o.waitDataEvtReceived().has_value());

        getStub()->invoke(false);
        auto const dataReceivedEvt = o.waitDataEvtReceived();
        ASSERT_TRUE(dataReceivedEvt.has_value());
        ASSERT_EQ(42, dataReceivedEvt.value());
        ASSERT_FALSE(o.waitNoDataEvtRceived());
    }

    TEST_F(ServiceEvent_UT, checkForwardAsyncData) {
        auto const guard = core::make_scope_exit([]() {
            try {
                TheTestProcessImpl.disconnect();
            } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
            }
            return;
        });
        TheTestProcessImpl.connect(getBrokerUrl(), getBrokerPort());

        const std::vector<int> expectedData{ 2, 3, 4 };
        TheTestProcessImpl.setFwdData(expectedData);

        auto fwdAsyncData = getStub()->fwdAsyncData();
        std::vector<int> data;
        ASSERT_NO_THROW(data = fwdAsyncData.getFor(getTimeout(100)));
        ASSERT_EQ(expectedData, data);
    }

} // namespace cho::osbase::application::ut
