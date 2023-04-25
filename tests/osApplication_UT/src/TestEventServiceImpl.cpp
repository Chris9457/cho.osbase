// \brief Declaration of the class TestEventServiceImpl

#include "osApplication_UT/TestEventServiceImpl.h"

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

    void TestEventServiceImpl::doConnect(const std::string &url, const unsigned short port) {
        m_pTestProcessService->connect(url, port);
    }

    void TestEventServiceImpl::doDisconnect() {
        m_pTestProcessService->disconnect();
    }

    TestEventServiceImpl::TestEventServiceImpl() : m_pTestProcessService(process::makeStub(getTaskLoop())) {
    }
} // namespace NS_OSBASE::application::ut
