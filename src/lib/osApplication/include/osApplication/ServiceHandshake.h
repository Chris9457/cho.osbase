// \brief This class implement a handshake when launching a service

#pragma once
#include "osData/IDataExchange.h"
#include <future>

namespace NS_OSBASE::application {

    class ServiceHandshake;
    using ServiceHandshakePtr = std::shared_ptr<ServiceHandshake>; //!< alias for shared pointer on ServiceHandshake

    /**
     * \brief Realize a handshake
     * \ingroup PACKAGE_SERVICE
     */
    class ServiceHandshake final {
    public:
        /**
         * \brief Delegate called on handshake notifications
         * \ingroup PACKAGE_SERVICE
         */
        class IDelegate {
        public:
            virtual ~IDelegate() = default; //!< dtor

            virtual void onEngage()    = 0; //!< called when engaged
            virtual void onDisengage() = 0; //!< called when disengaged
        };
        using IDelegatePtr  = std::shared_ptr<IDelegate>; //!< alias for shared pointer on IDelegate
        using IDelegateWPtr = std::weak_ptr<IDelegate>;   //!< alias for weak pointer on IDelegate

        static ServiceHandshakePtr create();                     //!< create an handshake - initilize a pipe (server side)
        static ServiceHandshakePtr create(const data::Uri &uri); //!< create an handshake - connect to the pipe linked to the uri (client
                                                                 //!< side)

        ~ServiceHandshake(); //!< dtor

        data::Uri getUri() const; //!< return the uri of the pipe initialized (server side)

        bool engage(const std::chrono::milliseconds &timeout = std::chrono::milliseconds::max());    //!< engage an handshake
        bool disengage(const std::chrono::milliseconds &timeout = std::chrono::milliseconds::max()); //!< disengage an handshake

        void setDelegate(IDelegatePtr pDelegate); //!< assign a delegate

    private:
        class ExchangeDelegate;
        using ExchangeDelegatePtr = std::shared_ptr<ExchangeDelegate>;

        ServiceHandshake();
        ServiceHandshake(const data::Uri &uri);
        ServiceHandshake(const bool bCreate);

        void onDataReceived(data::ByteBuffer &&buffer);
        void onConnected(const bool connected);
        void onFailure(std::string &&failure);

        IDelegateWPtr m_pDelegate;
        bool m_isCreator = false;
        data::ByteBuffer m_uuid;
        data::Uri m_uri;
        bool m_bEngaged = false;
        data::IDataExchangePtr m_pDataExchange;
        ExchangeDelegatePtr m_pExchangeDelegate;
        std::mutex m_mutex;
        std::condition_variable m_cvHandshake;
        std::condition_variable m_cvConnected;
        std::condition_variable m_cvDisconnected;
    };
} // namespace NS_OSBASE::application
