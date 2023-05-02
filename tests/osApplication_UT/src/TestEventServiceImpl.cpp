// \brief Declaration of the class TestEventServiceImpl

#include "osApplication_UT/TestEventServiceImpl.h"
#include "osApplication/ServiceConfiguration.h"

namespace NS_OSBASE::application::ut {

    void TestEventServiceImpl::invoke(bool noData) {
        if (noData) {
            publishEvtNoDataMsg();
        } else {
            publishEvtDataMsg(42);
        }
    }

    NS_OSBASE::data::AsyncData<std::vector<int>> TestEventServiceImpl::fwdAsyncData() {
        return m_pTestProcessService->fwdAsyncData();
    }

    void TestEventServiceImpl::doConnect() {
        m_pTestProcessService->connect();
    }

    void TestEventServiceImpl::doDisconnect() {
        m_pTestProcessService->disconnect();
    }

    TestEventServiceImpl::TestEventServiceImpl()
        : TestEventServiceSkeleton(TheServiceConfiguration.getBrokerUri(), TheServiceConfiguration.getRealm()),
          m_pTestProcessService(process::makeStub(getBrokerUri(), getRealm(), getTaskLoop())) {
    }
} // namespace NS_OSBASE::application::ut
