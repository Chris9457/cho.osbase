// \file  CoreKeySerializer.inl
// \brief Implementation of basic key-stream serializers

#pragma once
#include <regex>

namespace NS_OSBASE::core {

    // float
    template <typename TKey>
    float KeySerializer<TKey, float>::getValue(KeyStream<TKey> &keyStream, const float defaultValue) {
        return static_cast<float>(keyStream.getValue(static_cast<double>(defaultValue)));
    }

    template <typename TKey>
    bool KeySerializer<TKey, float>::setValue(KeyStream<TKey> &keyStream, const float value) {
        return keyStream.setValue(static_cast<double>(value));
    }

    // size_t
    template <typename TKey>
    size_t KeySerializer<TKey, size_t>::getValue(KeyStream<TKey> &keyStream, const size_t defaultValue) {
        return static_cast<size_t>(keyStream.getValue(static_cast<int>(defaultValue)));
    }

    template <typename TKey>
    bool KeySerializer<TKey, size_t>::setValue(KeyStream<TKey> &keyStream, const size_t value) {
        return keyStream.setValue(static_cast<int>(value));
    }

    // long
    template <typename TKey>
    long KeySerializer<TKey, long>::getValue(KeyStream<TKey> &keyStream, const long defaultValue) {
        return static_cast<long>(keyStream.getValue(static_cast<int>(defaultValue)));
    }

    template <typename TKey>
    bool KeySerializer<TKey, long>::setValue(KeyStream<TKey> &keyStream, const long value) {
        return keyStream.setValue(static_cast<int>(value));
    }

    // std::string
    template <typename TKey>
    std::string KeySerializer<TKey, std::string>::getValue(KeyStream<TKey> &keyStream, const std::string &defaultValue) {
        return type_cast<std::string>(keyStream.getValue(type_cast<std::wstring>(defaultValue)));
    }

    template <typename TKey>
    bool KeySerializer<TKey, std::string>::setValue(KeyStream<TKey> &keyStream, const std::string &value) {
        return keyStream.setValue(type_cast<std::wstring>(value));
    }

    // std::tuple
    namespace internal {
        // deserializeTuple
        template <typename TKey, size_t I = 0, typename... TArgs>
        std::enable_if_t<I == sizeof...(TArgs), void> deserializeTuple(KeyStream<TKey> &, std::tuple<TArgs...> &) {
        }

        template <typename TKey, size_t I = 0, typename... TArgs>
            std::enable_if_t < I<sizeof...(TArgs), void> deserializeTuple(KeyStream<TKey> &keyStream, std::tuple<TArgs...> &value) {

            using TupleElementType = std::tuple_element_t<I, std::tuple<TArgs...>>;
            if (keyStream.openRow(I).isNull()) {
                return;
            }

            std::get<I>(value) = std::move(keyStream.getValue(TupleElementType{}));

            deserializeTuple<TKey, I + 1>(keyStream, value);
        }

        // serializeTuple
        template <typename TKey, size_t I = 0, typename... TArgs>
        std::enable_if_t<I == sizeof...(TArgs), bool> serializeTuple(KeyStream<TKey> &, const std::tuple<TArgs...> &) {
            return true;
        }

        template <typename TKey, size_t I = 0, typename... TArgs>
            std::enable_if_t < I<sizeof...(TArgs), bool> serializeTuple(KeyStream<TKey> &keyStream, const std::tuple<TArgs...> &value) {

            keyStream.createRow();
            keyStream.setValue(std::get<I>(value));
            return serializeTuple<TKey, I + 1>(keyStream, value);
        }
    } // namespace internal

    template <typename TKey, typename... TArgs>
    std::tuple<TArgs...> KeyValueSerializer<TKey, std::tuple<TArgs...>>::getValue(
        KeyStream<TKey> &keyStream, const TKey &key, const std::tuple<TArgs...> &defaultValue) {
        if (keyStream.openArray(key).isNull())
            return defaultValue;

        auto se = make_scope_exit([&keyStream]() { keyStream.closeKey(); });
        std::tuple<TArgs...> value;
        internal::deserializeTuple(keyStream, value);
        return value;
    }

    template <typename TKey, typename... TArgs>
    bool KeyValueSerializer<TKey, std::tuple<TArgs...>>::setValue(
        KeyStream<TKey> &keyStream, const TKey &key, const std::tuple<TArgs...> &value) {
        if (keyStream.createArray(key).isNull())
            return false;

        auto se = make_scope_exit([&keyStream]() { keyStream.closeKey(); });
        return internal::serializeTuple(keyStream, value);
    }

    template <typename TKey, typename... TArgs>
    std::tuple<TArgs...> KeySerializer<TKey, std::tuple<TArgs...>, false>::getValue(
        KeyStream<TKey> &keyStream, const std::tuple<TArgs...> &defaultValue) {
        if (keyStream.openArray("").isNull())
            return defaultValue;

        auto se = make_scope_exit([&keyStream]() { keyStream.closeKey(); });
        std::tuple<TArgs...> value;
        internal::deserializeTuple(keyStream, value);
        return value;
    }

    template <typename TKey, typename... TArgs>
    bool KeySerializer<TKey, std::tuple<TArgs...>, false>::setValue(KeyStream<TKey> &keyStream, const std::tuple<TArgs...> &value) {
        if (keyStream.createArray("").isNull())
            return false;

        auto se = make_scope_exit([&keyStream]() { keyStream.closeKey(); });
        return internal::serializeTuple(keyStream, value);
    }

    // unisgned short
    template <typename TKey>
    unsigned short KeySerializer<TKey, unsigned short, false>::getValue(KeyStream<TKey> &keyStream, const unsigned short defaultValue) {
        return static_cast<unsigned short>(keyStream.getValue(static_cast<int>(defaultValue)));
    }

    template <typename TKey>
    bool KeySerializer<TKey, unsigned short, false>::setValue(KeyStream<TKey> &keyStream, const unsigned short value) {
        return keyStream.setValue(static_cast<const int>(value));
    }

    // std::array
    template <typename TKey, typename TValue, size_t N>
    std::array<TValue, N> KeySerializer<TKey, std::array<TValue, N>, false>::getValue(
        KeyStream<TKey> &keyStream, const std::array<TValue, N> &defaultValue) {
        auto const vValue = keyStream.getValue(std::vector<TValue>(defaultValue.cbegin(), defaultValue.cend()));
        std::array<TValue, N> arr;
        std::copy_n(vValue.cbegin(), N, arr.begin());
        return arr;
    }

    template <typename TKey, typename TValue, size_t N>
    bool KeySerializer<TKey, std::array<TValue, N>, false>::setValue(KeyStream<TKey> &keyStream, const std::array<TValue, N> &value) {
        return keyStream.setValue(std::vector<TValue>(value.cbegin(), value.cend()));
    }

    // unsigned char
    template <typename TKey>
    unsigned char KeySerializer<TKey, unsigned char, false>::getValue(KeyStream<TKey> &keyStream, const unsigned char defaultValue) {
        return static_cast<unsigned char>(keyStream.getValue(static_cast<int>(defaultValue)));
    }

    template <typename TKey>
    bool KeySerializer<TKey, unsigned char, false>::setValue(KeyStream<TKey> &keyStream, const unsigned char value) {
        return keyStream.setValue(static_cast<const int>(value));
    }

    // long long
    template <typename TKey>
    long long KeySerializer<TKey, long long, false>::getValue(KeyStream<TKey> &keyStream, const long long defaultValue) {
        return static_cast<long long>(keyStream.getValue(static_cast<int>(defaultValue)));
    }

    template <typename TKey>
    bool KeySerializer<TKey, long long, false>::setValue(KeyStream<TKey> &keyStream, const long long value) {
        return keyStream.setValue(static_cast<const int>(value));
    }

    // unsigned int
    template <typename TKey>
    unsigned int KeySerializer<TKey, unsigned int, false>::getValue(KeyStream<TKey> &keyStream, const unsigned int defaultValue) {
        return static_cast<unsigned int>(keyStream.getValue(static_cast<int>(defaultValue)));
    }

    template <typename TKey>
    bool KeySerializer<TKey, unsigned int, false>::setValue(KeyStream<TKey> &keyStream, const unsigned int value) {
        return keyStream.setValue(static_cast<const int>(value));
    }

    // short
    template <typename TKey>
    short KeySerializer<TKey, short, false>::getValue(KeyStream<TKey> &keyStream, const short defaultValue) {
        return static_cast<short>(keyStream.getValue(static_cast<int>(defaultValue)));
    }

    template <typename TKey>
    bool KeySerializer<TKey, short, false>::setValue(KeyStream<TKey> &keyStream, const short value) {
        return keyStream.setValue(static_cast<const int>(value));
    }

    // char
    template <typename TKey>
    char KeySerializer<TKey, char, false>::getValue(KeyStream<TKey> &keyStream, const char defaultValue) {
        return static_cast<char>(keyStream.getValue(static_cast<int>(defaultValue)));
    }

    template <typename TKey>
    bool KeySerializer<TKey, char, false>::setValue(KeyStream<TKey> &keyStream, const char value) {
        return keyStream.setValue(static_cast<const int>(value));
    }

} // namespace NS_OSBASE::core
