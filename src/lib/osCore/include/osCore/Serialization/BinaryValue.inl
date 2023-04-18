// \brief Declaration of the class BinaryValue

#pragma once
#include <utility>

namespace cho::osbase::core {
    /*
     * \class BinaryValue
     */
    template <typename TValue>
    BinaryValue<TValue>::BinaryValue(const TValue &value) : m_value(value) {
    }

    template <typename TValue>
    BinaryValue<TValue>::BinaryValue(TValue &&value) : m_value(std::move(value)) {
    }

    template <typename TValue>
    const TValue &BinaryValue<TValue>::get() const {
        return m_value;
    }

    template <typename TValue>
    TValue &BinaryValue<TValue>::get() {
        return m_value;
    }

    template <typename TValue>
    void BinaryValue<TValue>::set(const TValue &value) {
        m_value = value;
    }

    template <typename TValue>
    void BinaryValue<TValue>::set(TValue &&value) {
        m_value = std::move(value);
    }

    template <typename TValue>
    bool BinaryValue<TValue>::operator==(const BinaryValue &other) const {
        return m_value == other.m_value;
    }

    template <typename TValue>
    bool BinaryValue<TValue>::operator!=(const BinaryValue &other) const {
        return !(*this == other);
    }

    template <typename TValue>
    auto makeBinaryValue(const TValue &value) {
        return BinaryValue<TValue>(value);
    }

    template <typename TValue>
    auto makeBinaryValue(TValue &&value) {
        return BinaryValue<TValue>(std::forward<TValue>(value));
    }
} // namespace cho::osbase::core
