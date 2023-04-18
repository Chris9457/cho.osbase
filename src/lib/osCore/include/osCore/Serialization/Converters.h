// \file  Converters.h
// \brief Declaration of the basic converters

#pragma once
#include "osCore/Misc/TypeCast.h"
#include <string>

/** \cond */
// std::string <==> std::wstring
template <>
struct type_converter<std::string, std::wstring> {
    static std::string convert(const std::wstring &wStr);
};

template <>
struct type_converter<std::wstring, std::string> {
    static std::wstring convert(const std::string &str);
};

// std::string <==> std::string
template <>
struct type_converter<std::string, std::string> {
    static const std::string &convert(const std::string &str);
};

// std::wstring <==> std::wstring
template <>
struct type_converter<std::wstring, std::wstring> {
    static const std::wstring &convert(const std::wstring &wStr);
};

// std::basic_string<T> <==> const T*
template <typename TChar>
struct type_converter<const TChar *, std::basic_string<TChar>> {
    static const TChar *convert(const std::basic_string<TChar> &str) {
        return str.c_str();
    }
};

template <typename TChar>
struct type_converter<std::basic_string<TChar>, const TChar *> {
    static std::basic_string<TChar> convert(const TChar *pStr) {
        return std::basic_string<TChar>(pStr);
    }
};
/** \endcond*/