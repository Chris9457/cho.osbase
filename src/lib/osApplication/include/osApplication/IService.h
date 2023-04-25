// \brief Base interface of services

#pragma once
#include "osCore/DesignPattern/Observer.h"
#include "osData/IDataExchange.h"
#include <chrono>
#include <string>

/**
 * \defgroup PACKAGE_SERVICE Services
 * \brief   Set of classes and functions that manages service
 *
 * \ingroup PACKAGE_APPLICATION
 */

namespace NS_OSBASE::application {

    /**
     * \addtogroup PACKAGE_SERVICE
     * \{
     */

    /**
     * \brief Base class of all services
     */
    template <typename TService>
    class IService : public TService, public core::Observable {
    public:
        ~IService() override = default; //!< dtor

        /**
         * \brief Connect the service to the broker
         * \param url   url of the broker
         * \param port  port of the broker
         *
         * \throws  MessagingException Exception thrown when unable to connect to the messaging server
         */
        virtual void connect(const std::string &url, const unsigned short port) = 0;

        virtual void disconnect() = 0; //!< Disconnect the service from the broker

        virtual unsigned long long getAlivePeriod() const = 0; //!< return the period of the alive notifucation in ms

        /**
         * \brief Set the timeout of a RPC invoke
         * \param timeout   Timeout of any RPC invoke
         *
         * \remark In case of timeout, the invoke method throw a ServiceException
         */
        virtual void setCallTimeout(const std::chrono::milliseconds &timeout) = 0;
    };

    /**
     * \brief Structure used to store the data of a service event / topic
     */
    template <typename T>
    struct ServiceMsg {
        using type = T; //!< alias for the template type
        T data;         //!< data (payload) of the message
    };

    /**
     * \brief Structure use for a service message with no data
     */
    struct NullMsg {};

    /**
     * \brief Structure used to store the human readable SEH exception
     */
    struct RuntimeErrorData {
        std::string strError; //!< label of the runtime exception
    };
    using RuntimeErrorMsg = ServiceMsg<RuntimeErrorData>; //!< alias of the message published in case of runtime exception during an RPC

    struct ServiceConnectionMsg {
        ServiceConnectionMsg(const bool bConnected);
        bool isConnected() const;

    private:
        bool m_bConnected = false;
    };
    /** \} */

} // namespace NS_OSBASE::application
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::application::NullMsg);
OS_KEY_SERIALIZE_STRUCT(NS_OSBASE::application::RuntimeErrorData, strError);
