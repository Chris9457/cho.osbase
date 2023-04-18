// \brief Declaration of the class Service_BMImpl

#pragma once
#include "service_bmSkeleton.h"
#include "osCore/DesignPattern/Singleton.h"

namespace cho::osbase::application::bm {

    class Service_BMImpl : public IService_BMServiceSkeleton, public core::Singleton<Service_BMImpl> {
    public:
        void noRetNoArg() override;
        void noRet1Arg(int arg1) override;
        void noRet2Arg(int arg1, double arg2) override;
        void noRet4Arg(int arg1, double arg2, std::string arg3, unsigned char arg4) override;
        void noRet8Arg(
            int arg1, double arg2, std::string arg3, unsigned char arg4, int arg5, double arg6, std::string arg7, unsigned char arg8)
            override;
        void noRet1ComplexArg(ComplexStruct arg1) override;
        int retNoArg() override;
        ComplexStruct complexRetNoArg() override;
        void noRetBigArg(std::vector<int> arg1) override;
        void noRetAsyncArg(data::AsyncPagedData<std::vector<unsigned char>> arg1) override;
        void noRetAsyncStruct(data::AsyncPagedData<AsyncStruct> arg1) override;
        void noRetSyncStruct(AsyncStruct arg1) override;
        void noRetVolume(data::AsyncPagedData<Volume> volume) override;

        void invokeEvent(DataType dataType);

        bool waitReceivedData(const std::chrono::milliseconds &timeout = std::chrono::milliseconds(100));

    protected:
        void doDisconnect() override;

    private:
        void callBackAsyncBuffer(std::vector<unsigned char> &&value);
        void callBackAsyncStruct(AsyncStruct &&value);

        std::promise<void> m_promiseReceivedData;
        data::AsyncPagedData<std::vector<unsigned char>> m_asyncArg;
        data::AsyncPagedData<AsyncStruct> m_asyncStruct;
    };

#define TheService_BMImpl Service_BMImpl::getInstance()
} // namespace cho::osbase::application::bm
