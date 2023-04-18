// \brief Declaration of the WebSocketDataExchange class
#pragma once

#include "osData/IDataExchange.h"

#include <atomic>
#include <future>
#include <memory>
#include <shared_mutex>

namespace cho::osbase::data::impl {

    /*
     *  \\! DataTypes alias declaration
     */

    class WebSocketDataExchange final : public IDataExchange {

        using Server    = websocketpp::server<websocketpp::config::asio>;
        using ServerPtr = std::unique_ptr<Server>;
        using Client    = websocketpp::client<websocketpp::config::asio>;
        using ClientPtr = std::unique_ptr<Client>;

    public:
        enum class Side { Client, Server };

        WebSocketDataExchange();
        ~WebSocketDataExchange() override;

        Uri getUriOfCreator() const noexcept override;
        void open(const Uri &uri) override;
        void close() override;
        void create() override;
        void destroy() override;
        void push(const ByteBuffer &buffer) const override;
        void setDelegate(IDelegatePtr pDelegate) noexcept override;
        AccessType getAccessType() const noexcept override;
        bool isWired() const noexcept override;

    private:
        template <Side side, typename EndPoint>
        void initializeEndPoint(EndPoint &endPoint);

        template <Side side, typename EndPoint>
        void installWebsocketHandlers(EndPoint &endPoint);

        template <typename ConnectionPtr>
        std::string makeFailureMessage(Side side, ConnectionPtr pConnection) const;

        void onOpen(Side side, const websocketpp::connection_hdl &handle);

        bool onValidate(Side side, const websocketpp::connection_hdl &handle) const noexcept;

        void onFail(Side side, const websocketpp::connection_hdl &handle) const;

        void onClose(Side side, const websocketpp::connection_hdl &handle);

        void onClientMessage(const websocketpp::connection_hdl &handle, const Client::message_ptr &pMessage) const;

        void onServerMessage(const websocketpp::connection_hdl &handle, const Server::message_ptr &pMessage) const;

        void onMessage(const std::string &payload) const;

        void checkAccessType(const AccessType type) const;

        void waitIoServiceEnding() const;

        mutable std::shared_mutex m_mutex;
        std::atomic<AccessType> m_accessType;
        ServerPtr m_pServer;
        ClientPtr m_pClient;
        IDelegateWPtr m_pWDelegate;
        websocketpp::connection_hdl m_hdl;
        std::future<size_t> m_ioServiceStatus;
        Uri m_serverUri;
    };

} // namespace cho::osbase::data::impl