// \brief Encapsulation of a byte buffer

#pragma once
#include "osCore/Misc/TypeCast.h"
#include <string>
#include <vector>

/**
 * \addtogroup PACKAGE_OSDATA
 * \{
 */
namespace NS_OSBASE::data {

    using ByteBuffer = std::vector<std::byte>;

} // namespace NS_OSBASE::data

/**
 * \brief Conversion unsigned char* ==> ByteBuffer
 */
template <>
struct type_converter<NS_OSBASE::data::ByteBuffer, unsigned char *> {
    static NS_OSBASE::data::ByteBuffer convert(const unsigned char *pBufferArray, const size_t length); //!< \private
};

/**
 * \brief Conversion const unsigned char* ==> ByteBuffer
 */
template <>
struct type_converter<NS_OSBASE::data::ByteBuffer, const unsigned char *> {
    static NS_OSBASE::data::ByteBuffer convert(const unsigned char *pBufferArray, const size_t length); //!< \private
};

/**
 * \brief Conversion char* ==> ByteBuffer
 */
template <>
struct type_converter<NS_OSBASE::data::ByteBuffer, const char *> {
    static NS_OSBASE::data::ByteBuffer convert(const char *pBufferArray, const size_t length); //!< \private
};

/**
 * \brief Conversion std::string ==> ByteBuffer
 */
template <>
struct type_converter<NS_OSBASE::data::ByteBuffer, std::string> {
    static NS_OSBASE::data::ByteBuffer convert(const std::string &str); //!< \private
};

/**
 * \brief Conversion std::string <== ByteBuffer
 */
template <>
struct type_converter<std::string, NS_OSBASE::data::ByteBuffer> {
    static std::string convert(const NS_OSBASE::data::ByteBuffer &buffer); //!< \private
};

/**
 * \brief Conversion std::vector<unsigned char> ==> ByteBuffer
 */
template <>
struct type_converter<NS_OSBASE::data::ByteBuffer, std::vector<unsigned char>> {
    static NS_OSBASE::data::ByteBuffer convert(const std::vector<unsigned char> &bufferArray); //!< \private
};

/**
 * \brief Conversion std::vector<unsigned char> <== ByteBuffer
 */
template <>
struct type_converter<std::vector<unsigned char>, NS_OSBASE::data::ByteBuffer> {
    static std::vector<unsigned char> convert(const NS_OSBASE::data::ByteBuffer &buffer); //!< \private
};
/** \} */