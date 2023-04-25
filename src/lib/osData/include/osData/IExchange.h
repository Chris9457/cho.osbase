// \brief Base interface to exhange information

#pragma once
#include "Uri.h"
#include "osCore/Misc/NonCopyable.h"

/**
 * \defgroup PACKAGE_OSBASE_EXCHANGE Exchange Interfaces
 *
 * \brief Contains classes that enable the use of the IExchange interface used as a information exchange service
 *
 * The information exchange mechanism is used to push informations through a pipe. The information families are:
 *  - data: array of byte
 *  - files
 *
 * \ingroup PACKAGE_OSDATA
 */

namespace NS_OSBASE::data {

    /**
     * \addtogroup PACKAGE_OSBASE_EXCHANGE
     * \{
     */

    /**
     * \brief this class represent the interface IExchange
     *        intended to offer a point to point communication
     *        between 2 endpoints (a "creator" endpoind and another endpoint)
     *        this interface provide a mean to create/destroy a endpoint, open/close a endpoint, push and receive data
     * \ingroup PACKAGE_OSBASE_EXCHANGE
     */
    class IExchange : core::NonCopyable {

    public:
        /**
         * \brief Define the access type of the exchange
         */
        enum class AccessType {
            Create          = 0x1,               //!< creation allowed
            Open            = 0x1 << 1,          //!< Opening allowed
            ReadWrite       = 0x1 << 2,          //!< Read/write allowed
            CreateOpen      = Create | Open,     //!< creation & opening alloawed
            OpenReadWrite   = Open | ReadWrite,  //!< Resad/Write in open mode
            CreateReadWrite = Create | ReadWrite //!< Read/write in creation mode
        };

        /**
         * \brief Delegate called to notify a connection / disconnection or a failure
         * \ingroup PACKAGE_OSBASE_EXCHANGE
         */
        class IDelegate : NonCopyable {
        public:
            virtual ~IDelegate() = default;

            virtual void onConnected(const bool connected) = 0; //!< Indicate a connection / disconnection
            virtual void onFailure(std::string &&failure)  = 0; //!< Indicate a failure during the exchange
        };

        virtual ~IExchange() = default; // dtor

        virtual Uri getUriOfCreator() const = 0; //!< return the uri creator endpoint

        virtual void open(const Uri &uri) = 0; //!< connect an "open" endpoint to a created endpoint
        virtual void close()              = 0; //!< "close" an opened endpoint

        virtual void create()  = 0; //!< "create" a local endpoint
        virtual void destroy() = 0; //!< destroy a created endpoint

        virtual AccessType getAccessType() const noexcept = 0; //!< indicate the access type
        virtual bool isWired() const noexcept             = 0; //!< indicate if an endpoint is wired with another
    };

    IExchange::AccessType operator&(const IExchange::AccessType &lhs, const IExchange::AccessType &rhs); //!< overload of the operator &
    IExchange::AccessType operator|(const IExchange::AccessType &lhs, const IExchange::AccessType &rhs); //!< overload of the operator |
    /** \} */

} // namespace NS_OSBASE::data
