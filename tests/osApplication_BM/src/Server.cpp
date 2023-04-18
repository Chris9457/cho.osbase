// \brief Declaration of the class Server

#include "Server.h"
#include "Service_BMImpl.h"

namespace cho::osbase::application::bm {

    void Server::connect() {
        m_pBroker = data::makeBroker();
        m_pBroker->start(getBrokerPort());

        m_futRun = TheService_BMImpl.runAsync();
        TheService_BMImpl.connect(getBrokerUrl(), getBrokerPort());

        m_asyncBuffer.create();
        m_asyncStruct.create();
    }

    void Server::disconnect() {
        m_asyncBuffer.reset();
        m_asyncStruct.reset();
        TheService_BMImpl.disconnect();
        TheService_BMImpl.stop();
        m_futRun.wait();

        m_pBroker->stop();
    }

    std::string Server::getBrokerUrl() const {
        return "127.0.0.1";
    }

    unsigned short Server::getBrokerPort() const {
        return 8080;
    }

    data::AsyncPagedData<std::vector<unsigned char>> &Server::getAsyncBuffer() {
        return m_asyncBuffer;
    }

    data::AsyncPagedData<AsyncStruct> &Server::getAsyncStruct() {
        return m_asyncStruct;
    }
} // namespace cho::osbase::application::bm
