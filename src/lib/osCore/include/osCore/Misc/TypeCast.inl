// \file  TypeCast.h
// \brief Implementation of the converters mecanism

#pragma once

/*
 * type_cast
 */
template <typename TDest, typename TSrc>
TDest type_cast(const TSrc &src) {
    return type_converter<TDest, TSrc>::convert(src);
};

template <typename TDest, typename TSrc, typename TPolicy>
TDest type_cast(const TSrc &src, const TPolicy &policy) {
    return type_converter<TDest, TSrc>::convert(src, policy);
};
