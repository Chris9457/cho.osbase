// \file  KeySerializer.inl
// \brief Implementation of the class KeySerializer

#pragma once
#include "osCore/Misc/Scope.h"

#pragma region KeySerializer < TKey, TEnum>
namespace NS_OSBASE::core {
    /*
     * KeySerializer<TKey, TEnum>
     */
    template <typename TKey, typename TEnum>
    TEnum KeySerializer<TKey, TEnum, true>::getValue(KeyStream<TKey> &keyStream, const TEnum defaultValue) {
        return static_cast<TEnum>(keyStream.getValue(static_cast<std::underlying_type_t<TEnum>>(defaultValue)));
    }

    template <typename TKey, typename TEnum>
    bool KeySerializer<TKey, TEnum, true>::setValue(KeyStream<TKey> &keyStream, const TEnum value) {
        return keyStream.setValue(static_cast<std::underlying_type_t<TEnum>>(value));
    }
#pragma endregion

#pragma region KeySerializer < TKey, std::vector < TValue>>
    /*
     * KeySerializer<TKey, std::vector<TValue>>
     */
    template <typename TKey, typename TValue>
    std::vector<TValue> KeySerializer<TKey, std::vector<TValue>, false>::getValue(
        KeyStream<TKey> &keyStream, const std::vector<TValue> &defaultValues) {
        if (keyStream.openArray("").isNull())
            return defaultValues;

        auto se = make_scope_exit([&keyStream]() { keyStream.closeKey(); });
        std::vector<TValue> values;

        const auto nullKey = KeyValue<std::string, void>::null();
        for (auto currentKey = keyStream.firstKey(nullKey); !currentKey.isNull(); currentKey = keyStream.nextKey(nullKey))
            values.push_back(keyStream.getValue(TValue{}));

        return values.empty() ? defaultValues : values;
    }

    template <typename TKey, typename TValue>
    bool KeySerializer<TKey, std::vector<TValue>, false>::setValue(KeyStream<TKey> &keyStream, const std::vector<TValue> &values) {
        if (keyStream.createArray("").isNull())
            return false;

        keyStream.resetArray();
        auto se     = make_scope_exit([&keyStream]() { keyStream.closeKey(); });
        bool bSetOK = true;
        for (const TValue &element : values) {
            keyStream.createRow();
            bSetOK |= keyStream.setValue(element);
            if (!bSetOK) {
                return false;
            }
        }
        return true;
    }
#pragma endregion

#pragma region KeyValueSerializer < TKey, TValue>
    /*
     * KeyValueSerializer<TKey, TValue>
     */
    template <typename TKey, typename TValue>
    TValue KeyValueSerializer<TKey, TValue>::getValue(KeyStream<TKey> &keyStream, const TKey &key, const TValue &defaultValue) {
        if (keyStream.openKey(key).isNull())
            return defaultValue;

        auto se = make_scope_exit([&keyStream]() { keyStream.closeKey(); });
        return keyStream.getValue(defaultValue);
    }

    template <typename TKey, typename TValue>
    bool KeyValueSerializer<TKey, TValue>::setValue(KeyStream<TKey> &keyStream, const TKey &key, const TValue &value) {
        if (keyStream.createKey(key).isNull())
            return false;

        auto se = make_scope_exit([&keyStream]() { keyStream.closeKey(); });
        return keyStream.setValue(value);
    }
#pragma endregion

#pragma region KeyValueSerializer < TKey, std::vector < TValue>>
    /*
     * KeyValueSerializer<TKey, std::vector<TValue>>
     */
    template <typename TKey, typename TValue>
    std::vector<TValue> KeyValueSerializer<TKey, std::vector<TValue>>::getValue(
        KeyStream<TKey> &keyStream, const TKey &key, const std::vector<TValue> &defaultValue) {
        if (keyStream.openArray(key).isNull())
            return defaultValue;

        auto se = make_scope_exit([&keyStream]() { keyStream.closeKey(); });
        std::vector<TValue> values;

        const auto nullKey = KeyValue<std::string, void>::null();
        for (auto currentKey = keyStream.firstKey(nullKey); !currentKey.isNull(); currentKey = keyStream.nextKey(nullKey))
            values.push_back(keyStream.getValue(TValue{}));

        return values.empty() ? defaultValue : values;
    }

    template <typename TKey, typename TValue>
    bool KeyValueSerializer<TKey, std::vector<TValue>>::setValue(
        KeyStream<TKey> &keyStream, const TKey &key, const std::vector<TValue> &value) {
        if (keyStream.createArray(key).isNull())
            return false;

        keyStream.resetArray();
        auto se     = make_scope_exit([&keyStream]() { keyStream.closeKey(); });
        bool bSetOK = true;
        for (const TValue &element : value) {
            keyStream.createRow();
            bSetOK |= keyStream.setValue(element);
            if (!bSetOK) {
                return false;
            }
        }
        return true;
    }
#pragma endregion

#pragma region KeyValueSerializer < TKey, std::optional < TValue>>
    template <typename TKey, typename TValue>
    std::optional<TValue> KeyValueSerializer<TKey, std::optional<TValue>>::getValue(
        KeyStream<TKey> &keyStream, const TKey &key, const std::optional<TValue> &defaultValue) {
        if (keyStream.isKeyExist(key)) {
            return KeyValueSerializer<TKey, TValue>::getValue(keyStream, key, defaultValue ? *defaultValue : TValue{});
        }

        return {};
    }

    template <typename TKey, typename TValue>
    bool KeyValueSerializer<TKey, std::optional<TValue>>::setValue(
        KeyStream<TKey> &keyStream, const TKey &key, const std::optional<TValue> &value) {
        if (value) {
            KeyValueSerializer<TKey, TValue>::setValue(keyStream, key, *value);
        }

        return true;
    }
#pragma endregion

} // namespace NS_OSBASE::core