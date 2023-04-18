// \brief Declaration of the WebSocketDataExchangeImpl concrete methods


#include "WebSocketPPImports.h"

#include "osData/FactoryNames.h"
#include "WebSocketDataExchange.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include "osCore/Misc/TypeCast.h"
#include "osData/Log.h"
#include "osData/INetwork.h"

#include <algorithm>

using namespace std::chrono_literals;

template <>
struct type_converter<std::string, cho::osbase::data::impl::WebSocketDataExchange::Side> {
    static std::string convert(const cho::osbase::data::impl::WebSocketDataExchange::Side side) {
        switch (side) {
        case cho::osbase::data::impl::WebSocketDataExchange::Side::Server:
            return "server";
        case cho::osbase::data::impl::WebSocketDataExchange::Side::Client:
            return "client";
        }
        return "client";
    }
};

namespace cho::osbase::data::impl {
    OS_REGISTER_FACTORY_N(IDataExchange, WebSocketDataExchange, 0, IDATAEXCHANGE_WEBSOCKET_FACTORY_NAME)

    WebSocketDataExchange::WebSocketDataExchange() : m_accessType(AccessType::CreateOpen) {
    }

    WebSocketDataExchange::~WebSocketDataExchange() {
        if (m_pServer != nullptr) {
            destroy();
        } else if (m_pClient != nullptr) {
            close();
        }
    }

    Uri WebSocketDataExchange::getUriOfCreator() const noexcept /* override*/ {
        return m_serverUri;
    }

    void WebSocketDataExchange::open(const Uri &uri) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        checkAccessType(AccessType::CreateOpen);
        try {
            m_pClient = std::make_unique<Client>();
            initializeEndPoint<Side::Client>(*m_pClient);
            installWebsocketHandlers<Side::Client>(*m_pClient);

            websocketpp::lib::error_code ec;
            Client::connection_ptr pConnection = m_pClient->get_connection(type_cast<std::string>(uri), ec);
            if (ec)
                throw DataExchangeException(ec.message());
            m_pClient->connect(pConnection);
            m_hdl             = pConnection->get_handle();
            m_ioServiceStatus = std::async([this] { return m_pClient->run(); });
        } catch (const std::exception &e) {
            throw DataExchangeException(e.what());
        }
    }

    void WebSocketDataExchange::close() {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        if (m_pClient == nullptr) {
            return;
        }
        try {
            websocketpp::lib::error_code ec;
            m_pClient->close(m_hdl, websocketpp::close::status::normal, "", ec);
            if (ec.value() != 0) {
                oslog::error(OS_LOG_CHANNEL_DATA) << "fail to close the connection" << oslog::end();
            }
            waitIoServiceEnding();
            m_pClient.reset();
        } catch (const std::exception &e) {
            throw DataExchangeException(e.what());
        }
    }

    void WebSocketDataExchange::create() {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        checkAccessType(AccessType::CreateOpen);
        try {
            m_pServer = std::make_unique<Server>();
            initializeEndPoint<Side::Server>(*m_pServer);
            installWebsocketHandlers<Side::Server>(*m_pServer);
            m_pServer->listen(0);
            m_pServer->start_accept();
            m_ioServiceStatus = std::async([this] { return m_pServer->run(); });
            websocketpp::lib::asio::error_code ec;
            auto const endPoint = m_pServer->get_local_endpoint(ec);
            if (ec) {
                throw DataExchangeException("fail to create the server endpoint " + ec.message());
            }

            try {
                auto const pNetWork = makeNetwork();
                m_serverUri = Uri({ Uri::schemeWebsocket(), Uri::Authority{ {}, pNetWork->getLocalHost(), endPoint.port() }, {}, {} });
            } catch (const DataExchangeException &e) {
                oslog::error(OS_LOG_CHANNEL_DATA) << "Unable to get hostname or IP address: " << e.what() << oslog::end();
                throw;
            }
        } catch (const std::exception &e) {
            throw DataExchangeException(e.what());
        }
    }

    void WebSocketDataExchange::destroy() {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        if (m_pServer == nullptr) {
            return;
        }
        try {
            m_pServer->stop_listening();
            waitIoServiceEnding();
            m_pServer.reset();
        } catch (const std::exception &e) {
            throw DataExchangeException(e.what());
        }
    }

    void WebSocketDataExchange::push(const ByteBuffer &buffer) const {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        if (m_accessType != AccessType::CreateReadWrite && m_accessType != AccessType::OpenReadWrite) {
            throw DataExchangeException("the endpoint is not on the right state");
        }
        try {
            if (m_pServer != nullptr) {
                m_pServer->send(m_hdl, buffer.data(), buffer.size(), websocketpp::frame::opcode::BINARY);
            } else if (m_pClient != nullptr) {
                m_pClient->send(m_hdl, buffer.data(), buffer.size(), websocketpp::frame::opcode::BINARY);
            }
        } catch (const std::exception &e) {
            throw DataExchangeException(e.what());
        }
    }

    void WebSocketDataExchange::setDelegate(IDelegatePtr pDelegate) noexcept {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_pWDelegate = pDelegate;
    }

    IDataExchange::AccessType WebSocketDataExchange::getAccessType() const noexcept {
        return m_accessType;
    }

    bool WebSocketDataExchange::isWired() const noexcept {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        try {
            if (m_accessType == AccessType::OpenReadWrite && m_pClient != nullptr) {
                if (const auto con = m_pClient->get_con_from_hdl(m_hdl); con != nullptr) {
                    return con->get_socket().is_open();
                }
            } else if (m_accessType == AccessType::CreateReadWrite && m_pServer != nullptr) {
                if (const auto con = m_pServer->get_con_from_hdl(m_hdl); con != nullptr) {
                    return con->get_socket().is_open();
                }
            }
        } catch (const std::exception &) {
            return false;
        }
        return false;
    }

    void WebSocketDataExchange::onOpen(Side side, const websocketpp::connection_hdl &handle) {
        if (side == Side::Server) {
            m_accessType = AccessType::CreateReadWrite;
            m_hdl        = handle;
        } else {
            m_accessType = AccessType::OpenReadWrite;
        }
        if (const auto pDelegate = m_pWDelegate.lock(); pDelegate != nullptr) {
            pDelegate->onConnected(true);
        }
    }

    bool WebSocketDataExchange::onValidate(Side side, const websocketpp::connection_hdl &handle) const noexcept {
        std::ignore = handle;
        if (side == Side::Server && !m_hdl.expired()) {
            return false;
        }
        return true;
    }

    void WebSocketDataExchange::onFail(Side side, const websocketpp::connection_hdl &handle) const {
        std::ignore = handle;
        if (const auto pDelegate = m_pWDelegate.lock(); pDelegate != nullptr) {
            std::string strError;
            if (side == Side::Server) {
                ///! needed to avoid failure when the 1creator ends the connection before the opended endpoint
                if (m_pServer->get_con_from_hdl(handle)->get_local_close_code() == websocketpp::close::status::abnormal_close)
                    return;
                strError = makeFailureMessage(side, m_pServer->get_con_from_hdl(handle));
            } else {
                strError = makeFailureMessage(side, m_pClient->get_con_from_hdl(handle));
            }
            pDelegate->onFailure(std::move(strError));
        }
    }

    void WebSocketDataExchange::onClose(Side side, const websocketpp::connection_hdl &handle) {
        std::ignore  = side;
        std::ignore  = handle;
        m_accessType = AccessType::CreateOpen;
        if (const auto pDelegate = m_pWDelegate.lock(); pDelegate != nullptr) {
            pDelegate->onConnected(false);
        }
    }

    void WebSocketDataExchange::onClientMessage(const websocketpp::connection_hdl &handle, const Client::message_ptr &pMessage) const {
        std::ignore = handle;
        onMessage(pMessage->get_raw_payload());
    }

    void WebSocketDataExchange::onServerMessage(const websocketpp::connection_hdl &handle, const Server::message_ptr &pMessage) const {
        std::ignore = handle;
        onMessage(pMessage->get_raw_payload());
    }

    void WebSocketDataExchange::onMessage(const std::string &payload) const {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        if (const auto pDelegate = m_pWDelegate.lock(); pDelegate != nullptr) {
            const auto pBuffer = reinterpret_cast<const ByteBuffer::value_type *>(payload.c_str());
            ByteBuffer buffer;
            buffer.insert(buffer.end(), pBuffer, pBuffer + payload.size());
            pDelegate->onDataReceived(std::move(buffer));
        }
    }

    void WebSocketDataExchange::checkAccessType(const AccessType type) const {
        if (m_accessType != type) {
            throw DataExchangeException("the endpoint is not on the right state");
        }
    }

    template <WebSocketDataExchange::Side side, typename EndPoint>
    void WebSocketDataExchange::initializeEndPoint(EndPoint &endPoint) {
        endPoint.set_access_channels(websocketpp::log::alevel::none);
        endPoint.clear_access_channels(websocketpp::log::alevel::none);
        endPoint.set_error_channels(websocketpp::log::elevel::none);
        endPoint.init_asio();
        if constexpr (side == Side::Server) {
            m_pServer->set_reuse_addr(true);
        }
    }

    template <WebSocketDataExchange::Side side, typename EndPoint>
    void WebSocketDataExchange::installWebsocketHandlers(EndPoint &endPoint) {
        endPoint.set_open_handler([this](const websocketpp::connection_hdl &hdl) { side, onOpen(side, hdl); });
        endPoint.set_close_handler([this](const websocketpp::connection_hdl &hdl) { side, onClose(side, hdl); });
        endPoint.set_validate_handler([this](const websocketpp::connection_hdl &hdl) { return onValidate(side, hdl); });
        endPoint.set_fail_handler([this](const websocketpp::connection_hdl &hdl) { onFail(side, hdl); });

        if constexpr (side == Side::Server) {
            endPoint.set_message_handler(
                [this](const websocketpp::connection_hdl &hdl, const Server::message_ptr &pMessage) { onServerMessage(hdl, pMessage); });
        } else {
            endPoint.set_message_handler(
                [this](const websocketpp::connection_hdl &hdl, const Client::message_ptr &pMessage) { onClientMessage(hdl, pMessage); });
        }
    }

    template <typename ConnectionPtr>
    std::string WebSocketDataExchange::makeFailureMessage(WebSocketDataExchange::Side side, ConnectionPtr pConnection) const {
        std::stringstream ss;
        ss << "websocketpp handler failure on " << type_cast<std::string>(side) << std::endl;
        ss << pConnection->get_state() << std::endl;
        ss << pConnection->get_local_close_code() << std::endl;
        ss << pConnection->get_local_close_reason() << std::endl;
        ss << pConnection->get_remote_close_code() << std::endl;
        ss << pConnection->get_remote_close_reason() << std::endl;
        ss << pConnection->get_ec() << " - " << pConnection->get_ec().message() << std::endl;
        return ss.str();
    }

    void WebSocketDataExchange::waitIoServiceEnding() const {
        if (const auto status = m_ioServiceStatus.wait_for(100ms); status != std::future_status::ready) {
            oslog::error(OS_LOG_CHANNEL_DATA) << "timeout in stopping io service" << oslog::end();
        }
    }

} // namespace cho::osbase::data::impl
