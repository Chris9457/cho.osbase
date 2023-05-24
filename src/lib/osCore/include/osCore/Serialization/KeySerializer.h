// \file  KeySerializer.h
// \brief Declaration of the class KeySerializer

#pragma once
#include "KeySerializerMacros.h"
#include "KeyStream.h"
#include <optional>
#include <type_traits>
#include <vector>
#include <optional>

namespace NS_OSBASE::core {

    /**
     * \brief   This class represents the serialization / desrialization strategies
     * \internal
     * \ingroup PACKAGE_KEYSTREAM
     */
    template <typename TKey, typename TValue, bool = std::is_enum_v<TValue>>
    struct KeySerializer {
        /**
         * \brief deserialize a value at the key \e key
         * \param keyStream     KeyStream to deserialize
         * \param defaultValue  Default value to return if not found
         * \return              The value deserialized, the default value if not
         */
        static TValue getValue(KeyStream<TKey> &keyStream, const TValue &defaultValue) {
            static_assert(false, "KeySerializer undefined!");
        }

        /**
         * \brief serialize the array \e value at the key \e key
         * \param keyStream     KeyStream to serialize
         * \param value         Value to write
         * \return              The succes of the operation
         */
        static bool setValue(KeyStream<TKey> &keyStream, const TValue &value) {
            static_assert(false, "KeySerializer undefined!");
        }
    };

    /** \cond */
    template <typename TKey, typename TEnum>
    struct KeySerializer<TKey, TEnum, true> {
        static TEnum getValue(KeyStream<TKey> &keyStream, const TEnum defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const TEnum value);
    };

    template <typename TKey, typename TValue>
    struct KeySerializer<TKey, std::vector<TValue>, false> {
        static std::vector<TValue> getValue(KeyStream<TKey> &keyStream, const std::vector<TValue> &defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const std::vector<TValue> &value);
    };

    template <typename TKey, typename TValue>
    struct KeySerializer<TKey, std::optional<TValue>, false> {
        static std::optional<TValue> getValue(KeyStream<TKey> &keyStream, const std::optional<TValue> &defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const std::optional<TValue> &value);
    };
    /** \endcond */

    /**
     * \brief   This class represents the serialization / desrialization strategies for key-values
     * \internal
     * \ingroup PACKAGE_KEYSTREAM
     */
    template <typename TKey, typename TValue>
    struct KeyValueSerializer {
        /**
         * \brief deserialize a value at the key \e key
         * \param keyStream     KeyStream to deserialize
         * \param key           Key associated to the value
         * \param defaultValue  Default value to return if not found
         * \return              The value deserialized, the default value if not
         */
        static TValue getValue(KeyStream<TKey> &keyStream, const TKey &key, const TValue &defaultValue);

        /**
         * \brief serialize the array \e value at the key \e key
         * \param keyStream     KeyStream to serialize
         * \param key           Key associated to the value
         * \param value         Value to write
         * \return              The succes of the operation
         */
        static bool setValue(KeyStream<TKey> &keyStream, const TKey &key, const TValue &value);
    };

    /** \cond */
    template <typename TKey>
    struct KeyValueSerializer<TKey, void> {
        static bool getValue(KeyStream<TKey> &keyStream, const TKey &key);
        static bool setValue(KeyStream<TKey> &keyStream, const TKey &key);
    };

    template <typename TKey, typename TValue>
    struct KeyValueSerializer<TKey, std::vector<TValue>> {
        static std::vector<TValue> getValue(KeyStream<TKey> &keyStream, const TKey &key, const std::vector<TValue> &defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const TKey &key, const std::vector<TValue> &value);
    };

    template <typename TKey, typename TValue>
    struct KeyValueSerializer<TKey, std::optional<TValue>> {
        static std::optional<TValue> getValue(KeyStream<TKey> &keyStream, const TKey &key, const std::optional<TValue> &defaultValue);
        static bool setValue(KeyStream<TKey> &keyStream, const TKey &key, const std::optional<TValue> &value);
    };
    /** \endcond */
} // namespace NS_OSBASE::core

#include "KeySerializer.inl"
