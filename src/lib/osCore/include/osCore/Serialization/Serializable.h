// \brief Declaration of the class Serializable

#pragma once
#include <type_traits>
#include <vector>
#include <array>
#include <string>
#include <istream>

namespace cho::osbase::core {
    /**
     * \addtogroup PACKAGE_STREAM
     * \{
     */

    /**
     * \brief tool struct used to overload the stream operators on stl i/o stream
     */
    template <typename TValue, typename TByte = char>
    struct Serializable {
        static_assert(std::is_arithmetic_v<TValue> || std::is_enum_v<TValue>, "Serializable not defined for non arithmetic or enum type");
        static_assert(sizeof(TByte) == 1, "Serializable uses one byte TByte buffer");

        static bool read(TValue &value, const std::vector<TByte> &buffer, size_t &pos); //!< read a value from the buffer
        static bool write(const TValue &value, std::vector<TByte> &buffer);             //!< write the value to the buffer
    };

    /**
     * \private
     */
    template <typename TValue, typename TByte>
    struct Serializable<std::vector<TValue>, TByte> {
        static bool read(std::vector<TValue> &value, const std::vector<TByte> &buffer, size_t &pos);
        static bool write(const std::vector<TValue> &value, std::vector<TByte> &buffer);
    };

    /**
     * \private
     */
    template <typename TValue, size_t N, typename TByte>
    struct Serializable<std::array<TValue, N>, TByte> {
        static bool read(std::array<TValue, N> &value, const std::vector<TByte> &buffer, size_t &pos);
        static bool write(const std::array<TValue, N> &value, std::vector<TByte> &buffer);
    };

    /**
     * \private
     */
    template <typename TChar, typename TByte>
    struct Serializable<std::basic_string<TChar>, TByte> {
        static_assert(sizeof(TByte) == 1, "Serializable uses one byte TByte buffer");

        static bool read(std::basic_string<TChar> &value, const std::vector<TByte> &buffer, size_t &pos);
        static bool write(const std::basic_string<TChar> &value, std::vector<TByte> &buffer);
    };

    /** \} */

} // namespace cho::osbase::core

#include "Serializable.inl"
