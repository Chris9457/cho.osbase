// \brief File containing all the exception that can be thrown by an IMessaging implementation
#pragma once

#include "osCore/Exception/LogicException.h"

/**
 * \defgroup PACKAGE_OSBASE_MESSAGINGEXCEPTION Messaging Exception
 *
 * \brief Definition of the exception that can be thrown by an IMessaging implementation
 *
 * \ingroup PACKAGE_OSBASE_IMESSAGING
 */
namespace cho::osbase::data {

    /**
     * \brief Exception thrown when unable to connect to the messaging server
     */
    class MessagingException : public core::LogicException {
        using core::LogicException::LogicException;
    };

} // namespace cho::osbase::data