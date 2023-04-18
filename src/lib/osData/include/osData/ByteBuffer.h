// \brief Encapsulation of a byte buffer

#pragma once
#include "osCore/Misc/TypeCast.h"
#include <string>
#include <vector>

/**
 * \addtogroup PACKAGE_OSDATA
 * \{
 */
namespace cho::osbase::data {

    using ByteBuffer = std::vector<std::byte>;

} // namespace cho::osbase::data

/**
 * \brief Conversion unsigned char* ==> ByteBuffer
 */
template <>
struct type_converter<cho::osbase::data::ByteBuffer, unsigned char *> {
    static cho::osbase::data::ByteBuffer convert(const unsigned char *pBufferArray, const size_t length); //!< \private
};

/**
 * \brief Conversion const unsigned char* ==> ByteBuffer
 */
template <>
struct type_converter<cho::osbase::data::ByteBuffer, const unsigned char *> {
    static cho::osbase::data::ByteBuffer convert(const unsigned char *pBufferArray, const size_t length); //!< \private
};

/**
 * \brief Conversion char* ==> ByteBuffer
 */
template <>
struct type_converter<cho::osbase::data::ByteBuffer, const char *> {
    static cho::osbase::data::ByteBuffer convert(const char *pBufferArray, const size_t length); //!< \private
};

/**
 * \brief Conversion std::string ==> ByteBuffer
 */
template <>
struct type_converter<cho::osbase::data::ByteBuffer, std::string> {
    static cho::osbase::data::ByteBuffer convert(const std::string &str); //!< \private
};

/**
 * \brief Conversion std::string <== ByteBuffer
 */
template <>
struct type_converter<std::string, cho::osbase::data::ByteBuffer> {
    static std::string convert(const cho::osbase::data::ByteBuffer &buffer); //!< \private
};

/**
 * \brief Conversion std::vector<unsigned char> ==> ByteBuffer
 */
template <>
struct type_converter<cho::osbase::data::ByteBuffer, std::vector<unsigned char>> {
    static cho::osbase::data::ByteBuffer convert(const std::vector<unsigned char> &bufferArray); //!< \private
};

/**
 * \brief Conversion std::vector<unsigned char> <== ByteBuffer
 */
template <>
struct type_converter<std::vector<unsigned char>, cho::osbase::data::ByteBuffer> {
    static std::vector<unsigned char> convert(const cho::osbase::data::ByteBuffer &buffer); //!< \private
};
/** \} */