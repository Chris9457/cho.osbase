/// \brief Implementation of the service ITestService

#pragma once
#include "testserviceSkeleton.h"
#include "osCore/DesignPattern/Singleton.h"
#include "osData/IDataExchange.h"

namespace testservice::impl {

    class TestServiceImpl final : public testservice::api::ITestServiceSkeleton,
                                  public NS_OSBASE::core::Singleton<TestServiceImpl> {
        friend Singleton<TestServiceImpl>;

    public:
        std::string getText() const override;
        bool setText(std::string text, int n, double d) override;

        api::Position getPosition() override;
        void setPosition(api::Position position, api::EPosition ePos) override;

        std::vector<api::Position> getPositions() override;
        void setPositions(std::vector<api::Position> positions, api::EPosition ePos) override;

        api::Mat4 getTransfo(api::Vec4 origin, api::Vec4 dest) override;
        void invokeCrash(api::sub2::enum1 value) override;

        NS_OSBASE::data::AsyncPagedData<std::vector<bool>> setPositionsAsync(
            NS_OSBASE::data::AsyncData<api::Positions> buffer) override;

        void wait(unsigned long long timeoutMs) override;

        void resetWait();
        void waitForStartWait();
        void notifyDummyEvent() const;

    protected:
        void doConnect(const std::string &url, const unsigned short port) override;
        void doDisconnect() override;

    private:
        class SagitalViewDelegate;

        TestServiceImpl();

        void sendSagitalView() const;
        void onDataReceived(api::Positions &&positions);

        std::vector<api::Position> m_positions;
        NS_OSBASE::data::AsyncPagedData<std::vector<bool>> m_booleansProvider;

        std::mutex m_mutWait;
        bool m_bWaitStarted = false;
        std::condition_variable m_cvWaitStarted;
    };
#define TheTestServiceImpl TestServiceImpl::getInstance()
} // namespace testservice::impl
