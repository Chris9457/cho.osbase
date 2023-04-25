// \brief Implementation tests of ByteBuffer
#include "osData/ByteBuffer.h"
#include <gtest/gtest.h>

namespace NS_OSBASE::data::ut {

    class ByteBuffer_UT : public testing::Test {};

    TEST_F(ByteBuffer_UT, constUnsignedCharArrayToByteBuffer) {
        constexpr unsigned char pBufferArray[] = { 0, 1, 2 };
        auto const byteBuffer = type_cast<ByteBuffer>(static_cast<const unsigned char *>(pBufferArray), std::size(pBufferArray));

        ASSERT_EQ(std::size(pBufferArray), byteBuffer.size());
        for (size_t i = 0; i < byteBuffer.size(); ++i) {
            ASSERT_EQ(pBufferArray[i], static_cast<unsigned char>(byteBuffer[i]));
        }
    }

    TEST_F(ByteBuffer_UT, unsignedCharArrayToByteBuffer) {
        unsigned char pBufferArray[] = { 0, 1, 2 };
        auto const byteBuffer        = type_cast<ByteBuffer>(static_cast<unsigned char *>(pBufferArray), std::size(pBufferArray));

        ASSERT_EQ(std::size(pBufferArray), byteBuffer.size());
        for (size_t i = 0; i < byteBuffer.size(); ++i) {
            ASSERT_EQ(pBufferArray[i], static_cast<unsigned char>(byteBuffer[i]));
        }
    }

    TEST_F(ByteBuffer_UT, stringToByteBuffer) {
        std::string str       = "toto";
        auto const byteBuffer = type_cast<ByteBuffer>(str);

        ASSERT_EQ(str.size(), byteBuffer.size());
        for (size_t i = 0; i < byteBuffer.size(); ++i) {
            ASSERT_EQ(str[i], static_cast<char>(byteBuffer[i]));
        }
    }

    TEST_F(ByteBuffer_UT, byteBufferToString) {
        ByteBuffer byteBuffer = { std::byte{ 't' }, std::byte{ 'o' }, std::byte{ 't' }, std::byte{ 'o' } };
        auto const str        = type_cast<std::string>(byteBuffer);

        ASSERT_EQ(byteBuffer.size(), str.size());
        for (size_t i = 0; i < str.size(); ++i) {
            ASSERT_EQ(static_cast<char>(byteBuffer[i]), str[i]);
        }
    }

    TEST_F(ByteBuffer_UT, vectorToByteBuffer) {
        const std::vector<unsigned char> vBuffer = { 't', 'o', 't', 'o' };
        auto const byteBuffer                    = type_cast<ByteBuffer>(vBuffer);

        ASSERT_EQ(vBuffer.size(), byteBuffer.size());
        for (size_t i = 0; i < byteBuffer.size(); ++i) {
            ASSERT_EQ(vBuffer[i], static_cast<unsigned char>(byteBuffer[i]));
        }
    }

    TEST_F(ByteBuffer_UT, byteBufferToVector) {
        ByteBuffer byteBuffer = { std::byte{ 't' }, std::byte{ 'o' }, std::byte{ 't' }, std::byte{ 'o' } };
        auto const vBuffer    = type_cast<std::vector<unsigned char>>(byteBuffer);

        ASSERT_EQ(byteBuffer.size(), vBuffer.size());
        for (size_t i = 0; i < vBuffer.size(); ++i) {
            ASSERT_EQ(static_cast<unsigned char>(byteBuffer[i]), vBuffer[i]);
        }
    }
} // namespace NS_OSBASE::data::ut
