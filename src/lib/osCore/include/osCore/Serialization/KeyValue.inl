// \file  KeyValue.inl
// \brief Implementation of the class KeyValue

#pragma once
namespace cho::osbase::core {

#pragma region UntypedKeyValue
    /*
     * \class UntypedKeyValue<TKey>
     */
    template <typename TKey>
    UntypedKeyValue<TKey>::UntypedKeyValue() : m_bNull(true) {
    }

    template <typename TKey>
    UntypedKeyValue<TKey>::UntypedKeyValue(const TKey &key) : m_key(key) {
    }

    template <typename TKey>
    const TKey &UntypedKeyValue<TKey>::getKey() const {
        return m_key;
    }

    template <typename TKey>
    void UntypedKeyValue<TKey>::setKey(const TKey &key) {
        m_key = key;
    }

    template <typename TKey>
    bool UntypedKeyValue<TKey>::operator==(const UntypedKeyValue &rhs) const {
        return m_key == rhs.m_key;
    }

    template <typename TKey>
    bool UntypedKeyValue<TKey>::operator!=(const UntypedKeyValue &rhs) const {
        return !(*this == rhs);
    }

    template <typename TKey>
    bool UntypedKeyValue<TKey>::isNull() const {
        return m_bNull;
    }
#pragma endregion

#pragma region KeyValue < TKey, TValue>
    /*
     * \class KeyValue<TKey, TValue>
     */
    template <typename TKey, typename TValue>
    KeyValue<TKey, TValue>::KeyValue() : m_value{} {
    }

    template <typename TKey, typename TValue>
    KeyValue<TKey, TValue>::KeyValue(const TKey &key, const ValueType &value) : UntypedKeyValue<TKey>(key), m_value(value) {
    }

    template <typename TKey, typename TValue>
    KeyValue<TKey, TValue>::KeyValue(const TKey &key, ValueType &&value) : UntypedKeyValue<TKey>(key), m_value(std::move(value)) {
    }

    template <typename TKey, typename TValue>
    const TValue &KeyValue<TKey, TValue>::getValue() const {
        return m_value;
    }

    template <typename TKey, typename TValue>
    TValue KeyValue<TKey, TValue>::getValue() {
        return std::move(m_value);
    }

    template <typename TKey, typename TValue>
    void KeyValue<TKey, TValue>::setValue(const ValueType &value) {
        m_value = value;
    }

    template <typename TKey, typename TValue>
    void KeyValue<TKey, TValue>::setValue(ValueType &&value) {
        m_value = std::move(value);
    }

    template <typename TKey, typename TValue>
    BasicType KeyValue<TKey, TValue>::getBasicType() const {
        return BasicTypeT<TValue>::value;
    }

    template <typename TKey, typename TValue>
    bool KeyValue<TKey, TValue>::operator==(const KeyValue<TKey, TValue> &rhs) const {
        return UntypedKeyValue<TKey>::operator==(rhs) && m_value == rhs.m_value;
    }

    template <typename TKey, typename TValue>
    bool KeyValue<TKey, TValue>::operator!=(const KeyValue<TKey, TValue> &rhs) const {
        return !(*this == rhs);
    }

    template <typename TKey, typename TValue>
    const KeyValue<TKey, TValue> &KeyValue<TKey, TValue>::null() {
        static const KeyValue<TKey, TValue> nullKeyValue;
        return nullKeyValue;
    }
#pragma endregion

#pragma region KeyValue < TKey, TValue[]>
    /*
     * \class KeyValue<TKey, TValue[]>
     */
    template <typename TKey, typename TValue>
    KeyValue<TKey, TValue[]>::KeyValue(const TKey &key, const ValueType &value) : UntypedKeyValue<TKey>(key), m_value(value) {
    }

    template <typename TKey, typename TValue>
    const typename KeyValue<TKey, TValue[]>::ValueType &KeyValue<TKey, TValue[]>::getValue() const {
        return m_value;
    }

    template <typename TKey, typename TValue>
    void KeyValue<TKey, TValue[]>::setValue(const typename KeyValue<TKey, TValue[]>::ValueType &value) {
        m_value = value;
    }

    template <typename TKey, typename TValue>
    TValue KeyValue<TKey, TValue[]>::getValue(const size_t index) const {
        return m_value[index];
    }

    template <typename TKey, typename TValue>
    void KeyValue<TKey, TValue[]>::setValue(const size_t index, const TValue &value) {
        m_value[index] = value;
    }

    template <typename TKey, typename TValue>
    void KeyValue<TKey, TValue[]>::addValue(const TValue &value) {
        m_value.push_back(value);
    }

    template <typename TKey, typename TValue>
    void KeyValue<TKey, TValue[]>::clear() {
        m_value.clear();
    }

    template <typename TKey, typename TValue>
    size_t KeyValue<TKey, TValue[]>::size() const {
        return m_value.size();
    }

    template <typename TKey, typename TValue>
    BasicType KeyValue<TKey, TValue[]>::getBasicType() const {
        return BasicTypeT<TValue[]>::value;
    }

    template <typename TKey, typename TValue>
    bool KeyValue<TKey, TValue[]>::operator==(const KeyValue &rhs) const {
        return UntypedKeyValue<TKey>::operator==(rhs) && m_value == rhs.m_value;
    }

    template <typename TKey, typename TValue>
    bool KeyValue<TKey, TValue[]>::operator!=(const KeyValue &rhs) const {
        return !(*this == rhs);
    }

    template <typename TKey, typename TValue>
    typename KeyValue<TKey, TValue[]>::ValueType::const_reference KeyValue<TKey, TValue[]>::operator[](const size_t index) const {
        return getValue(index);
    }

    template <typename TKey, typename TValue>
    typename KeyValue<TKey, TValue[]>::ValueType::reference KeyValue<TKey, TValue[]>::operator[](const size_t index) {
        return m_value.at(index);
    }

    template <typename TKey, typename TValue>
    const KeyValue<TKey, TValue[]> &KeyValue<TKey, TValue[]>::null() {
        static const KeyValue<TKey, TValue[]> nullKeyValue;
        return nullKeyValue;
    }
#pragma endregion

    template <typename TKey, typename TValue>
    KeyValue<TKey, std::vector<TValue>>::KeyValue(const TKey &key, const ValueType &value) : KeyValue<TKey, TValue[]>(key, value) {
    }

    template <typename TKey, typename TValue>
    KeyValue<TKey, std::vector<TValue>> &KeyValue<TKey, std::vector<TValue>>::operator=(const KeyValue<TKey, TValue[]> &rhs) {
        KeyValue<TKey, TValue[]>::operator=(rhs);
        return *this;
    }

#pragma region KeyValue < TKey, void>
    /*
     * \class KeyValue<TKey, void>
     */
    template <typename TKey>
    KeyValue<TKey, void>::KeyValue(const TKey &key) : UntypedKeyValue<TKey>(key) {
    }

    template <typename TKey>
    BasicType KeyValue<TKey, void>::getBasicType() const {
        return BasicType::Void;
    }

    template <typename TKey>
    bool KeyValue<TKey, void>::operator==(const KeyValue &rhs) const {
        return UntypedKeyValue<TKey>::operator==(rhs);
    }

    template <typename TKey>
    bool KeyValue<TKey, void>::operator!=(const KeyValue &rhs) const {
        return !(*this == rhs);
    }

    template <typename TKey>
    const KeyValue<TKey, void> &KeyValue<TKey, void>::null() {
        static const KeyValue<TKey, void> nullKeyValue;
        return nullKeyValue;
    }
#pragma endregion

#pragma region Makers
    namespace internal {
        template <typename T>
        struct DefaultValue {
            static T get() {
                return {};
            }
        };

        template <typename T>
        struct DefaultValue<T[]> {
            static std::vector<T> get() {
                return std::vector<T>{};
            }
        };
    } // namespace internal

    /*
     * \function makeKey && makeKeyValue
     */
    template <typename TKey>
    KeyValue<TKey, void> makeKey(const TKey &key) {
        return KeyValue<TKey, void>(key);
    }

    template <typename TValue, typename TKey>
    KeyValue<TKey, TValue> makeKeyValue(const TKey &key, const TValue &value) {
        return KeyValue<TKey, TValue>(key, value);
    }

    template <typename TValue, typename TKey>
    KeyValue<TKey, TValue> makeKeyValue(const TKey &key) {
        return KeyValue<TKey, TValue>(key, internal::DefaultValue<TValue>::get());
    }

    template <typename TKey>
    KeyValue<TKey, std::string> makeKeyValue(const TKey &key, const char *str) {
        return makeKeyValue(key, std::string(str));
    }

    template <typename TKey>
    KeyValue<TKey, std::wstring> makeKeyValue(const TKey &key, const wchar_t *wStr) {
        return makeKeyValue(key, std::wstring(wStr));
    }

    template <typename TKey, typename TFirst, typename TSecond, typename... TArgs>
    KeyValue<TKey, std::tuple<TFirst, TSecond, TArgs...>> makeKeyValue(const TKey &key, TFirst &&first, TSecond &&second, TArgs &&...args) {
        return makeKeyValue(key, std::make_tuple(std::forward<TFirst>(first), std::forward<TSecond>(second), std::forward<TArgs>(args)...));
    }

#pragma endregion

} // namespace cho::osbase::core
