// \brief Declaration of the class Server

#include "Server.h"
#include "Service_BMImpl.h"
#include "osApplication/ServiceConfiguration.h"

namespace NS_OSBASE::application::bm {

    void Server::connect() {
        m_pBroker = data::makeBroker();
        m_pBroker->start(getBrokerPort());

        m_futRun = TheService_BMImpl.runAsync();
        TheServiceConfiguration.setBrokerUri(std::string{ "ws://" + getBrokerUrl() + ":" + std::to_string(getBrokerPort()) });
        TheServiceConfiguration.setRealm("");
        TheService_BMImpl.connect();

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
} // namespace NS_OSBASE::application::bm
