// \file  osscopevalue.inl
// \brief Implementation of the class scope_value

#pragma once

namespace cho::osbase::core {

    /*
     * \struct scope_value
     */
    template <class TValue>
    scope_value<TValue>::scope_value(TValue &refValue, const TValue &newValue) noexcept
        : scope_exit<std::function<void()>>([this] { m_refValue = m_initialValue; }), m_refValue(refValue), m_initialValue(refValue) {
        m_refValue = newValue; // NOLINT(cppcoreguidelines-prefer-member-initializer)
    }

    template <class TValue>
    scope_value<TValue>::scope_value(scope_value &&other) noexcept
        : scope_exit<std::function<void()>>([this] { m_refValue = m_initialValue; }),
          m_refValue(other.m_refValue),
          m_initialValue(other.m_initialValue) {
        other.release();
    }

    /*
     * \function make_scope_value
     */
    template <class TValue>
    scope_value<TValue> make_scope_value(TValue &refValue) noexcept {
        return make_scope_value(refValue, refValue);
    }

    template <class TValue>
    scope_value<TValue> make_scope_value(TValue &refValue, const TValue &newValue) noexcept {
        return scope_value<TValue>(refValue, newValue);
    }

} // namespace cho::osbase::core
