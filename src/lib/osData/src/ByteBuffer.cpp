// \brief Encapsulation of a byte buffer

#include "osData/ByteBuffer.h"

using namespace cho::osbase::data;

// unsigned char* ==> ByteBuffer
ByteBuffer type_converter<ByteBuffer, unsigned char *>::convert(const unsigned char *pBufferArray, const size_t length) {
    return type_cast<ByteBuffer>(std::vector<unsigned char>(pBufferArray, pBufferArray + length));
}

// const unsigned char* ==> ByteBuffer
ByteBuffer type_converter<std::vector<std::byte>, const unsigned char *>::convert(const unsigned char *pBufferArray, const size_t length) {
    return type_cast<ByteBuffer>(std::vector<unsigned char>(pBufferArray, pBufferArray + length));
}

// const char* ==> ByteBuffer
ByteBuffer type_converter<std::vector<std::byte>, const char *>::convert(const char *pBufferArray, const size_t length) {
    return type_cast<ByteBuffer>(reinterpret_cast<const unsigned char *>(pBufferArray), length);
}

// std::string <==> ByteBuffer
ByteBuffer type_converter<ByteBuffer, std::string>::convert(const std::string &str) {
    ByteBuffer buffer;
    auto const pByteBuffer = reinterpret_cast<const ByteBuffer::value_type *>(str.c_str());
    buffer.insert(buffer.end(), pByteBuffer, pByteBuffer + str.size());
    return buffer;
}

std::string type_converter<std::string, ByteBuffer>::convert(const ByteBuffer &buffer) {
    std::string str;
    auto const pStrBuffer = reinterpret_cast<const char *>(buffer.data());
    str.insert(str.end(), pStrBuffer, pStrBuffer + buffer.size());
    return str;
}

// std::vector<unsigned char> <==> ByteBuffer
ByteBuffer type_converter<ByteBuffer, std::vector<unsigned char>>::convert(const std::vector<unsigned char> &bufferArray) {
    ByteBuffer buffer;
    auto const pByteBuffer = reinterpret_cast<const ByteBuffer::value_type *>(bufferArray.data());
    buffer.insert(buffer.end(), pByteBuffer, pByteBuffer + bufferArray.size());
    return buffer;
}

std::vector<unsigned char> type_converter<std::vector<unsigned char>, ByteBuffer>::convert(const ByteBuffer &buffer) {
    std::vector<unsigned char> bufferArray;
    ;
    auto const pBuffer = reinterpret_cast<const unsigned char *>(buffer.data());
    bufferArray.insert(bufferArray.end(), pBuffer, pBuffer + buffer.size());
    return bufferArray;
}
