// \file  CoreImpl.h
// \brief Declare link tools

#pragma once
#include "osCore/DesignPattern/AbstractFactory.h"
#include "osCore/Serialization/KeyStream.h"

/**
 * \defgroup PACKAGE_OSCOREIMPL Core implementation
 *
 * \brief This package gathers classes and functions of the CoreImpl package
 *
 * \ingroup PACKAGE_OSBASE
 */

/**
 * \brief Macro mandatory to link with the current binary the concrete realizations (Xml & Json)
 * \ingroup PACKAGE_OSCOREIMPL
 */
#define OS_CORE_IMPL_LINK()                                                                                                                \
    OS_CORE_LINK_KEYSTREAM_XML()                                                                                                           \
    OS_CORE_LINK_KEYSTREAM_JSON()

/** \cond */
#define OS_CORE_LINK_KEYSTREAM_XML()                                                                                                       \
    namespace cho::osbase::core::impl {                                                                                             \
        OS_LINK_FACTORY_N(StringKeyStream, RapidXmlStream, 0);                                                                             \
        OS_LINK_FACTORY_N(StringKeyStream, RapidXmlStream, 1);                                                                             \
    }

#define OS_CORE_LINK_KEYSTREAM_JSON()                                                                                                      \
    namespace cho::osbase::core::impl {                                                                                             \
        OS_LINK_FACTORY_N(StringKeyStream, RapidJsonStream, 0);                                                                            \
        OS_LINK_FACTORY_N(StringKeyStream, RapidJsonStream, 1);                                                                            \
    }

/** \endcond */
