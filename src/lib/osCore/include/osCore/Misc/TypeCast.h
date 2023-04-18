// \file  TypeCast.h
// \brief Declaration of the converters mecanism

#pragma once

/**
 * \defgroup PACKAGE_TYPES_CONVERSION Types Conversions
 *
 * \brief Give a normalize convention to write converters between types
 *
 * \ingroup PACKAGE_MISC
 */

/**
 * \brief  This structure allows the conversion between a instance of type TSrc to an instance of type Test.
 * \ingroup PACKAGE_TYPES_CONVERSION
 */
template <typename TDest, typename TSrc>
struct type_converter;

/**
 * \brief This template function convert the variable src to an instance of type TDest.
 * \ingroup PACKAGE_TYPES_CONVERSION
 */
template <typename TDest, typename TSrc>
TDest type_cast(const TSrc &src);

/**
 * \brief This template function convert the variable src to an instance of type TDest using the policy.
 * \ingroup PACKAGE_TYPES_CONVERSION
 */
template <typename TDest, typename TSrc, typename TPolicy>
TDest type_cast(const TSrc &src, const TPolicy &policy);

#include "TypeCast.inl"
