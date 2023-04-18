// \brief Declaration of the class Service_BMImpl

#include "Service_BMImpl.h"

namespace cho::osbase::application::bm {
    void Service_BMImpl::noRetNoArg() {
    }

    void Service_BMImpl::noRet1Arg(int arg1) {
        std::ignore = arg1;
    }

    void Service_BMImpl::noRet2Arg(int arg1, double arg2) {
        std::ignore = arg1;
        std::ignore = arg2;
    }

    void Service_BMImpl::noRet4Arg(int arg1, double arg2, std::string arg3, unsigned char arg4) {
        std::ignore = arg1;
        std::ignore = arg2;
        std::ignore = arg3;
        std::ignore = arg4;
    }

    void Service_BMImpl::noRet8Arg(
        int arg1, double arg2, std::string arg3, unsigned char arg4, int arg5, double arg6, std::string arg7, unsigned char arg8) {
        std::ignore = arg1;
        std::ignore = arg2;
        std::ignore = arg3;
        std::ignore = arg4;
        std::ignore = arg5;
        std::ignore = arg6;
        std::ignore = arg7;
        std::ignore = arg8;
    }

    void Service_BMImpl::noRet1ComplexArg(ComplexStruct arg1) {
        std::ignore = arg1;
    }

    int Service_BMImpl::retNoArg() {
        return 3;
    }

    ComplexStruct Service_BMImpl::complexRetNoArg() {
        static ComplexStruct data = { { 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3 },
            { { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 } },
            "tutu" };
        return data;
    }

    void Service_BMImpl::invokeEvent(DataType dataType) {
        static ComplexStruct data = { { 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3 },
            { { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 },
                { "titi", 4.2 },
                { "tata", 52.3 } },
            "tutu" };

        switch (dataType) {
        case DataType::noData:
            publishEvtNoData();
            break;
        case DataType::simple:
            publishEvtData(4);
            break;
        case DataType::complex:
            publishEvtComplexData(data);
        }
    }

    void Service_BMImpl::noRetBigArg(std::vector<int> arg1) {
        std::ignore = arg1;
    }

    void Service_BMImpl::noRetAsyncArg(data::AsyncPagedData<std::vector<unsigned char>> arg1) {
        if (m_asyncArg.isValid()) {
            return;
        }

        m_asyncArg = std::move(arg1);
        m_asyncArg.get([this](std::vector<unsigned char> &&value) { callBackAsyncBuffer(std::move(value)); });
    }

    void Service_BMImpl::noRetAsyncStruct(cho::osbase::data::AsyncPagedData<AsyncStruct> arg1) {
        if (m_asyncStruct.isValid()) {
            return;
        }

        m_asyncStruct = std::move(arg1);
        m_asyncStruct.get([this](AsyncStruct &&value) { callBackAsyncStruct(std::move(value)); });
    }

    void Service_BMImpl::noRetSyncStruct(AsyncStruct arg1) {
        std::ignore = arg1;
    }

    void Service_BMImpl::noRetVolume(data::AsyncPagedData<Volume> volume) {
        auto const vol = volume.get();
    }

    bool Service_BMImpl::waitReceivedData(const std::chrono::milliseconds &timeout) {
        auto const guard           = core::make_scope_exit([this]() { decltype(m_promiseReceivedData)().swap(m_promiseReceivedData); });
        auto const futReceivedData = m_promiseReceivedData.get_future();
        return futReceivedData.wait_for(timeout) == std::future_status::ready;
    }

    void Service_BMImpl::doDisconnect() {
        if (m_asyncArg.isConnected()) {
            m_asyncArg.reset();
        }
        if (m_asyncStruct.isConnected()) {
            m_asyncStruct.reset();
        }

        IService_BMServiceSkeleton::doDisconnect();
    }

    void Service_BMImpl::callBackAsyncBuffer(std::vector<unsigned char> &&) {
        m_promiseReceivedData.set_value();
    }

    void Service_BMImpl::callBackAsyncStruct(AsyncStruct &&) {
        m_promiseReceivedData.set_value();
    }

} // namespace cho::osbase::application::bm
