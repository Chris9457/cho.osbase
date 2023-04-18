// \brief Declaration of the class TestEventServiceImpl

#pragma once
#include "TestEventSkeleton.h"
#include "TestProcess.h"
#include "osCore/DesignPattern/Singleton.h"

namespace cho::osbase::application::ut {

    class TestEventServiceImpl : public event::TestEventServiceSkeleton, public core::Singleton<TestEventServiceImpl> {
        friend Singleton<TestEventServiceImpl>;

    public:
        void invoke(bool noData) override;
        cho::osbase::data::AsyncData<std::vector<int>> fwdAsyncData() override;

    protected:
        void doConnect(const std::string &url, const unsigned short port) override;
        void doDisconnect() override;

    private:
        TestEventServiceImpl();
        ~TestEventServiceImpl() override = default;

        process::TestProcessServicePtr m_pTestProcessService;
    };
#define TheTestEventServiceImpl TestEventServiceImpl::getInstance()

} // namespace cho::osbase::application::ut
