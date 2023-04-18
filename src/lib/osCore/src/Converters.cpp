// \file  Converters.cpp
// \brief Implementation of the basic converters

#include "osCore/Serialization/Converters.h"
#include <Windows.h>
#include <memory>

// std::string <==> std::wstring
std::string type_converter<std::string, std::wstring>::convert(const std::wstring &wStr) {
    // Convert a Unicode string to an ASCII string
    std::unique_ptr<char[]> szTo(new char[wStr.length() + 1]);
    szTo[wStr.size()] = '\0';
    ::WideCharToMultiByte(CP_ACP, 0, wStr.c_str(), -1, szTo.get(), static_cast<int>(wStr.length()), nullptr, nullptr);
    return szTo.get();
}

std::wstring type_converter<std::wstring, std::string>::convert(const std::string &str) {
    // Convert an ASCII string to a Unicode String
    std::unique_ptr<wchar_t[]> wszTo(new wchar_t[str.length() + 1]);
    wszTo[str.size()] = L'\0';
    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszTo.get(), static_cast<int>(str.length()));
    return wszTo.get();
}

// std::string <==> std::string
const std::string &type_converter<std::string, std::string>::convert(const std::string &str) {
    return str;
}

// std::wstring <==> std::wstring
const std::wstring &type_converter<std::wstring, std::wstring>::convert(const std::wstring &wStr) {
    return wStr;
}
