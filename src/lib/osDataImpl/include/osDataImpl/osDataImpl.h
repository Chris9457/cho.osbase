// \brief Declare link tools

#pragma once

#include "osCore/DesignPattern/AbstractFactory.h"

/**
 * \defgroup PACKAGE_OSDATAIMPL Data Implementation
 *
 * \brief This package gathers classes and functions of the DataImpl package
 *
 * \ingroup PACKAGE_OSBASE
 */

/**
 * \addtogroup PACKAGE_OSDATAIMPL
 * \{
 */

/**
 * \def OS_DATA_IMPL_LINK()
 * \brief Macro mandatory to link the concrete implementation with the current binary
 */
#define OS_DATA_IMPL_LINK()                                                                                                                \
    OS_DATA_LINK_MESSAGING()                                                                                                               \
    OS_DATA_LINK_EXCHANGE()                                                                                                                \
    OS_DATA_LINK_LOGOUTPUT()                                                                                                               \
    OS_DATA_LINK_NETWORK()

/** \} */

/** \cond */
#define OS_DATA_LINK_MESSAGING()                                                                                                           \
    namespace NS_OSBASE::data::impl {                                                                                                      \
        OS_LINK_FACTORY_N(IMessaging, WampccMessaging, 0);                                                                                 \
        OS_LINK_FACTORY_N(IBroker, WampccBroker, 0);                                                                                       \
    }

#define OS_DATA_LINK_EXCHANGE()                                                                                                            \
    namespace NS_OSBASE::data::impl {                                                                                                      \
        OS_LINK_FACTORY_N(IDataExchange, WebSocketDataExchange, 0);                                                                        \
        OS_LINK_FACTORY_N(IFileExchange, LocalFileExchange, 0);                                                                            \
    }

#define OS_DATA_LINK_LOGOUTPUT()                                                                                                           \
    namespace NS_OSBASE::data::impl {                                                                                                      \
        OS_LINK_FACTORY_N(ILogOutput, LogOutputNull, 0);                                                                                   \
        OS_LINK_FACTORY_N(ILogOutput, LogOutputFile, 0);                                                                                   \
        OS_LINK_FACTORY_N(ILogOutput, LogOutputDataExchange, 0);                                                                           \
        OS_LINK_FACTORY_N(ILogOutput, LogOutputDataExchange, 1);                                                                           \
        OS_LINK_FACTORY_N(ILogOutput, LogOutputConsole, 0);                                                                                \
        OS_LINK_FACTORY_N(ILogOutput, LogOutputDebug, 0);                                                                                  \
        OS_LINK_FACTORY_N(ILogOutput, LogOutputAsyncData, 0);                                                                              \
    }

#define OS_DATA_LINK_NETWORK()                                                                                                             \
    namespace NS_OSBASE::data::impl {                                                                                                      \
        OS_LINK_FACTORY_N(INetwork, Network, 0);                                                                                           \
    }

/** \endcond */
