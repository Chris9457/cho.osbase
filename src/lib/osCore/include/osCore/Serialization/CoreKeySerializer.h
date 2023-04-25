// \file  CoreKeySerializer.h
// \brief Declaration of basic key-stream serializers

#pragma once
#include "Converters.h"
#include "KeySerializer.h"
#include <array>
#include <tuple>

namespace NS_OSBASE::core {

    /** \cond */
    template <typename TKey>
    struct KeySerializer<TKey, float> {
        static float getValue(KeyStream<TKey> &keyStream, const float defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const float value);
    };

    template <typename TKey>
    struct KeySerializer<TKey, size_t> {
        static size_t getValue(KeyStream<TKey> &keyStream, const size_t defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const size_t value);
    };

    template <typename TKey>
    struct KeySerializer<TKey, long> {
        static long getValue(KeyStream<TKey> &keyStream, const long defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const long value);
    };

    template <typename TKey>
    struct KeySerializer<TKey, std::string> {
        static std::string getValue(KeyStream<TKey> &keyStream, const std::string &defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const std::string &value);
    };

    template <typename TKey, typename... TArgs>
    struct KeyValueSerializer<TKey, std::tuple<TArgs...>> {
        static std::tuple<TArgs...> getValue(KeyStream<TKey> &keyStream, const TKey &key, const std::tuple<TArgs...> &defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const TKey &key, const std::tuple<TArgs...> &value);
    };

    template <typename TKey, typename... TArgs>
    struct KeySerializer<TKey, std::tuple<TArgs...>, false> {
        static std::tuple<TArgs...> getValue(KeyStream<TKey> &keyStream, const std::tuple<TArgs...> &defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const std::tuple<TArgs...> &value);
    };

    template <typename TKey>
    struct KeySerializer<TKey, unsigned short> {
        static unsigned short getValue(KeyStream<TKey> &keyStream, const unsigned short defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const unsigned short value);
    };

    template <typename TKey, typename TValue, size_t N>
    struct KeySerializer<TKey, std::array<TValue, N>, false> {
        static std::array<TValue, N> getValue(KeyStream<TKey> &keyStream, const std::array<TValue, N> &defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const std::array<TValue, N> &value);
    };

    template <typename TKey>
    struct KeySerializer<TKey, unsigned char, false> {
        static unsigned char getValue(KeyStream<TKey> &keyStream, const unsigned char defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const unsigned char value);
    };

    template <typename TKey>
    struct KeySerializer<TKey, long long, false> {
        static long long getValue(KeyStream<TKey> &keyStream, const long long defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const long long value);
    };

    template <typename TKey>
    struct KeySerializer<TKey, unsigned int, false> {
        static unsigned int getValue(KeyStream<TKey> &keyStream, const unsigned int defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const unsigned int value);
    };

    template <typename TKey>
    struct KeySerializer<TKey, short, false> {
        static short getValue(KeyStream<TKey> &keyStream, const short defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const short value);
    };

    template <typename TKey>
    struct KeySerializer<TKey, char, false> {
        static char getValue(KeyStream<TKey> &keyStream, const char defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const char value);
    };

} // namespace NS_OSBASE::core

#include "CoreKeySerializer.inl"
/** \endcond */
