// \brief Declaration of the class Serializable

#pragma once
#include "Serializable.h"
#include "osCore/Misc/Scope.h"

namespace NS_OSBASE::core {
    /*
     * \class Serializable<arithmetic_type || enum_type>
     */
    template <typename TValue, typename TByte>
    bool Serializable<TValue, TByte>::read(TValue &value, const std::vector<TByte> &buffer, size_t &pos) {
        static auto const nbBytes = sizeof(TValue);

        if (nbBytes > buffer.size() - pos) {
            return false;
        }

        value = *reinterpret_cast<const TValue *>(&buffer[pos]);
        pos += nbBytes;
        return true;
    }

    template <typename TValue, typename TByte>
    bool Serializable<TValue, TByte>::write(const TValue &value, std::vector<TByte> &buffer) {
        static auto const nbBytes = sizeof(TValue);
        auto const pValue         = reinterpret_cast<const TByte *>(&value);

        buffer.insert(buffer.cend(), pValue, pValue + nbBytes);
        return true;
    }

    /*
     * \class Serializable<std::basic_string<TChar>>
     */
    template <typename TChar, typename TByte>
    bool Serializable<std::basic_string<TChar>, TByte>::read(
        std::basic_string<TChar> &value, const std::vector<TByte> &buffer, size_t &pos) {
        if (buffer.empty()) {
            return false;
        }

        auto const start = reinterpret_cast<const TChar *>(&buffer[pos]);
        auto const end   = reinterpret_cast<const TChar *>(&buffer[buffer.size() - 1]);
        auto endString   = start;
        while (endString < end && *endString != TChar(0)) {
            ++endString;
        }

        value.assign(start, endString);
        pos += endString - start + 1;
        return true;
    }

    template <typename TChar, typename TByte>
    bool Serializable<std::basic_string<TChar>, TByte>::write(const std::basic_string<TChar> &value, std::vector<TByte> &buffer) {
        auto const nbBytes = sizeof(TChar) * (value.size() + 1);
        auto const pValue  = reinterpret_cast<const TByte *>(value.data());
        buffer.insert(buffer.cend(), pValue, pValue + nbBytes);
        return true;
    }

    /*
     * \class Serializable<std::vector>
     */
    template <typename TValue, typename TByte>
    bool Serializable<std::vector<TValue>, TByte>::read(std::vector<TValue> &value, const std::vector<TByte> &buffer, size_t &pos) {
        size_t nbElt{};
        if (!Serializable<size_t, TByte>::read(nbElt, buffer, pos)) {
            return false;
        }

        value.clear();
        if (nbElt == 0) {
            return true;
        }

        value.reserve(nbElt);

        if constexpr (std::is_arithmetic_v<TValue> && !std::is_same_v<TValue, bool>) {
            auto const size = sizeof(TValue) * nbElt;
            if (pos + size > buffer.size()) {
                pos = buffer.size();
                return false;
            }

            auto const pBuffer = reinterpret_cast<const TValue *>(buffer.data() + pos);
            value.assign(pBuffer, pBuffer + nbElt);
            pos += size;

        } else {
            while (nbElt != 0) {
                TValue elt;
                if (!Serializable<TValue, TByte>::read(elt, buffer, pos)) {
                    return false;
                }
                value.emplace_back(elt);
                --nbElt;
            }
        }

        return true;
    }

    template <typename TValue, typename TByte>
    bool Serializable<std::vector<TValue>, TByte>::write(const std::vector<TValue> &value, std::vector<TByte> &buffer) {
        if (!Serializable<size_t, TByte>::write(value.size(), buffer)) {
            return false;
        }

        if constexpr (std::is_arithmetic_v<TValue> && !std::is_same_v<TValue, bool>) {
            auto const pValue = reinterpret_cast<const TByte *>(value.data());
            buffer.insert(buffer.cend(), pValue, pValue + value.size() * sizeof(TValue));
        } else {
            for (auto const elt : value) {
                if (!Serializable<TValue, TByte>::write(elt, buffer)) {
                    return false;
                }
            }
        }

        return true;
    }

    /*
     * \class Serializable<std::array>
     */
    template <typename TValue, size_t N, typename TByte>
    bool Serializable<std::array<TValue, N>, TByte>::read(std::array<TValue, N> &value, const std::vector<TByte> &buffer, size_t &pos) {
        std::vector<TValue> v;
        if (Serializable<std::vector<TValue>, TByte>::read(v, buffer, pos) && v.size() <= N) {
            for (size_t index = 0; index < std::min(N, v.size()); ++index) {
                value[index] = v[index];
            }
            return true;
        }

        return false;
    }

    template <typename TValue, size_t N, typename TByte>
    bool Serializable<std::array<TValue, N>, TByte>::write(const std::array<TValue, N> &value, std::vector<TByte> &buffer) {
        const std::vector<TValue> v(value.cbegin(), value.cend());
        return Serializable<std::vector<TValue>, TByte>::write(v, buffer);
    }

    /*
     * \class Serializable<std::optional>
     */
    template <typename TValue, typename TByte>
    bool Serializable<std::optional<TValue>, TByte>::read(std::optional<TValue> &value, const std::vector<TByte> &buffer, size_t &pos) {
        bool hasValue = false;

        value.reset();
        if (Serializable<bool, TByte>::read(hasValue, buffer, pos)) {
            if (hasValue) {
                TValue val{};
                auto const guard = core::make_scope_exit([&value, &val] { value = val; });
                return Serializable<TValue, TByte>::read(val, buffer, pos);
            }
            return true;
        }

        return false;
    }

    template <typename TValue, typename TByte>
    bool Serializable<std::optional<TValue>, TByte>::write(const std::optional<TValue> &value, std::vector<TByte> &buffer) {
        if (!Serializable<bool, TByte>::write(value.has_value(), buffer)) {
            return false;
        }

        if (value.has_value()) {
            return Serializable<TValue, TByte>::write(value.value(), buffer);
        }

        return true;
    }

} // namespace NS_OSBASE::core
