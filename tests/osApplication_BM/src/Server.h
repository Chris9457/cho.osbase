// \brief Declaration of the class Server

#pragma once
#include "service_bm.h"
#include "osCore/DesignPattern/Singleton.h"
#include "osData/AsyncData.h"
#include "osData/IBroker.h"

namespace cho::osbase::application::bm {
    class Server : public core::Singleton<Server> {
        friend core::Singleton<Server>;

    public:
        void connect();
        void disconnect();

        std::string getBrokerUrl() const;
        unsigned short getBrokerPort() const;
        data::AsyncPagedData<std::vector<unsigned char>> &getAsyncBuffer();
        data::AsyncPagedData<AsyncStruct> &getAsyncStruct();

    private:
        data::IBrokerPtr m_pBroker;
        std::shared_future<void> m_futRun;
        data::AsyncPagedData<std::vector<unsigned char>> m_asyncBuffer;
        data::AsyncPagedData<AsyncStruct> m_asyncStruct;
    };
#define TheServer Server::getInstance()
} // namespace cho::osbase::application::bm
