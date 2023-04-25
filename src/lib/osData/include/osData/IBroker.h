// \brief Header for the IBroker class
#pragma once

#include "osCore/Misc/NonCopyable.h"
#include <memory>

/**
 * \defgroup PACKAGE_OSBASE_IBROKER Messaging broker interface
 *
 * \brief Contains the interface to create a messaging broker
 *
 * \ingroup PACKAGE_OSBASE_IMESSAGING
 */

namespace NS_OSBASE::data {

    /**
     * \addtogroup PACKAGE_OSBASE_IBROKER
     * \{
     */
    class IBroker;
    using IBrokerPtr = std::shared_ptr<IBroker>; //!< alias on shared pointer of IBroker

    /**
     * \brief Interface used to instantiate a Broker
     *
     * \ingroup PACKAGE_OSBASE_IBROKER
     */
    class IBroker : public core::NonCopyable {
    public:
        virtual ~IBroker() = default; //!< Dtor

        /**
         * \brief Method called to start a broker
         * \param	port        Port on which to start the broker
         */
        virtual void start(unsigned short port) = 0;

        /**
         * \brief Method called to stop the broker
         */
        virtual void stop() = 0;
    };

    IBrokerPtr makeBroker(); //!< create a broker

    /** \} */
} // namespace NS_OSBASE::data
