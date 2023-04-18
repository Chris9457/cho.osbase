// \brief just header to import the websocketpp used header
#pragma once

/**
 * First import all windows types and defines
 */
#ifndef __wtypes_h__
#include <wtypes.h>
#endif
#ifndef __WINDEF_
#include <windef.h>
#endif

/**
 * Second import the websocketpp librarie needed imports
 * (and disable the warnings for this SOUP)
 */
#pragma warning(push)
#pragma warning(disable : 4127 4267)
#include "websocketpp/config/boost_config.hpp"
#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/config/asio_no_tls_client.hpp"
#include "websocketpp/server.hpp"
#pragma warning(pop)
