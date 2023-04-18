// \file  KeyStream.inl
// \brief Implementation of the class KeyStream

#pragma once
#include "KeySerializer.h"
#include "osCore/Misc/Scope.h"

namespace cho::osbase::core {

#pragma region KeyStream
    namespace internal {
        template <typename TKey>
        bool parseKey(const TKey &key, TKey &keyName, size_t &index, bool &bArray) {
            auto posStartArray = key.find("[");
            bArray             = posStartArray != TKey::npos;
            index              = 0;

            if (bArray) {
                auto posEndArray = key.find("]", posStartArray + 1);
                if (posEndArray == std::string::npos)
                    return false;

                keyName = key.substr(0, posStartArray);
                index   = std::stoi(key.substr(posStartArray + 1, posEndArray - posStartArray - 1));
            } else
                keyName = key;

            return true;
        }

    } // namespace internal

    /*
     * \class KeyStream
     */
    template <typename TKey>
    template <typename TValue>
    TValue KeyStream<TKey>::getValue(const TValue &defaultValue) {
        return KeySerializer<TKey, TValue>::getValue(*this, defaultValue);
    }

    template <typename TKey>
    template <typename TValue>
    bool KeyStream<TKey>::setValue(const TValue &value) {
        return KeySerializer<TKey, TValue>::setValue(*this, value);
    }

    template <typename TKey>
    template <typename TValue>
    TValue KeyStream<TKey>::getKeyValue(const TKey &key, const TValue &defaultValue) {
        return KeyValueSerializer<TKey, TValue>::getValue(*this, key, defaultValue);
    }

    template <typename TKey>
    template <typename TValue>
    bool KeyStream<TKey>::setKeyValue(const TKey &key, const TValue &value) {
        return KeyValueSerializer<TKey, TValue>::setValue(*this, key, value);
    }

    template <typename TKey>
    KeyValue<TKey, void> KeyStream<TKey>::createKeys(const KeyPath<TKey> &keys) {
        static const auto nullKey = KeyValue<std::string, void>::null();
        if (keys.empty())
            return nullKey;

        // Open a key or an array
        std::string keyName;

        rewind();
        for (auto &&key : keys) {
            size_t index = 0;
            bool bArray  = false;
            if (!internal::parseKey(key, keyName, index, bArray))
                return nullKey;

            if (bArray) {
                if (createArray(keyName).isNull())
                    return nullKey;

                const auto count = size();
                if (index >= count) {
                    // Add the row if necessary
                    for (size_t addedIndex = size(); addedIndex <= index; ++addedIndex)
                        createRow();
                } else if (openRow(index).isNull())
                    return nullKey;
            } else {
                // Single version
                if (createKey(key).isNull())
                    return nullKey;
            }
        }

        return makeKey(keyName);
    }

    template <typename TKey>
    KeyValue<TKey, void> KeyStream<TKey>::openKeys(const KeyPath<TKey> &keys) {
        static const auto nullKey = KeyValue<std::string, void>::null();
        if (keys.empty())
            return nullKey;

        // Open a key or an array
        rewind();
        for (auto &&key : keys) {
            std::string keyName;
            size_t index = 0;
            bool bArray  = false;

            if (!internal::parseKey(key, keyName, index, bArray))
                return nullKey;

            if (bArray) {
                if (openArray(keyName).isNull() || openRow(index).isNull())
                    return nullKey;
            } else {
                // Single version
                if (openKey(key).isNull())
                    return nullKey;
            }
        }

        return getCurrentKey();
    }
#pragma endregion

#pragma region Operator serialization
    // Operator serialization
    template <typename TKey, typename TValue>
    KeyStream<TKey> &operator<<(KeyStream<TKey> &keyStream, const KeyValue<TKey, TValue> &keyValue) {
        if constexpr (std::is_void_v<TValue>) {
            keyStream.createKey(keyValue.getKey());
            keyStream.closeKey();
        } else {
            keyStream.setKeyValue(keyValue.getKey(), keyValue.getValue());
        }

        return keyStream;
    }

    template <typename TKey, typename TValue>
    KeyStream<TKey> &operator>>(KeyStream<TKey> &keyStream, KeyValue<TKey, TValue> &keyValue) {
        if constexpr (!std::is_void_v<TValue>) {
            keyValue.setValue(keyStream.getKeyValue(keyValue.getKey(), keyValue.getValue()));

            if (!keyStream.isKeyExist(keyValue.getKey())) {
                auto nullKeyValue = KeyValue<TKey, TValue>::null();
                nullKeyValue.setKey(keyValue.getKey());
                nullKeyValue.setValue(keyValue.getValue());
                keyValue = nullKeyValue;
            }
        }
        return keyStream;
    }
#pragma endregion

} // namespace cho::osbase::core
