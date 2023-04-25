// \file  ScopeValue.h
// \brief Declaration of the class scope_value

#pragma once
#include "Scope.h"
#include <functional>

namespace NS_OSBASE::core {

    /**
     * \brief 	This class represents:
     *				- guard on a value
     * \ingroup PACKAGE_MISC
     */
    template <class TValue>
    struct scope_value : scope_exit<std::function<void()>> {
        scope_value()                                 = delete;
        scope_value(const scope_value<TValue> &other) = delete;
        scope_value &operator=(const scope_value &) = delete;
        scope_value &operator=(scope_value &&) = delete;

        /**
         * \brief   Creates a scope_value from a value and assigne a new value.
         * \param refValue    value to store
         * \param newValue    value to assigne
         */
        explicit scope_value(TValue &refValue, const TValue &newValue) noexcept;

        /**
         * \brief   Creates a scope_value from another scope_value.
         * \param other     scope_value to move from
         */
        scope_value(scope_value &&other) noexcept;

        /**
         * \brief If not released, restore the previous stored value
         */
        ~scope_value() = default;

    private:
        TValue &m_refValue;
        const TValue m_initialValue;
    };

    template <class TValue>
    scope_value<TValue> make_scope_value(TValue &refValue) noexcept;

    template <class TValue>
    scope_value<TValue> make_scope_value(TValue &refValue, const TValue &newValue) noexcept;

} // namespace NS_OSBASE::core

#include "ScopeValue.inl"