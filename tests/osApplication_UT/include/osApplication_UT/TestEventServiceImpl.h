// \brief Declaration of the class TestEventServiceImpl

#pragma once
#include "TestEventSkeleton.h"
#include "TestProcess.h"
#include "osCore/DesignPattern/Singleton.h"

namespace NS_OSBASE::application::ut {

    class TestEventServiceImpl : public event::TestEventServiceSkeleton, public core::Singleton<TestEventServiceImpl> {
        friend Singleton<TestEventServiceImpl>;

    public:
        void invoke(bool noData) override;
        NS_OSBASE::data::AsyncData<std::vector<int>> fwdAsyncData() override;

    protected:
        void doConnect() override;
        void doDisconnect() override;

    private:
        TestEventServiceImpl();
        ~TestEventServiceImpl() override = default;

        process::TestProcessServicePtr m_pTestProcessService;
    };
#define TheTestEventServiceImpl TestEventServiceImpl::getInstance()

} // namespace NS_OSBASE::application::ut
