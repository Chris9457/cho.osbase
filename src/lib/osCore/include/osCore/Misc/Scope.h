// \file  Sscope.h
// \brief Declaration of the class scope_exit

#pragma once

namespace cho::osbase::core {

    /**
     * \brief 	This class represents:
     *				- RAII concept of scope guard, to be removed as soon as it is available in the STL
     * \ingroup PACKAGE_MISC
     */
    template <class Fn>
    struct scope_exit {
        scope_exit()                   = delete;
        scope_exit(const scope_exit &) = delete;
        scope_exit &operator=(const scope_exit &) = delete;
        scope_exit &operator=(scope_exit &&) = delete;

        /**
         * \brief   Creates a scope_exit from a function or a function object.
         * \param fn    function or function object used for initializing the stored Fn
         */
        explicit scope_exit(Fn &&fn) noexcept;

        /**
         * \brief   Creates a scope_exit from another scope_exit.
         * \param other     scope_exit to move from
         */
        scope_exit(scope_exit &&other) noexcept;

        /**
         * \brief Calls the exit function if the scope_exit is active, then destroys the stored EF (if it is a function object) and any
         * other non-static data members.
         */
        ~scope_exit();

        void release() noexcept; //!< Makes the scope_exit inactive.

    private:
        Fn m_fn;
        bool m_run;
    };

    template <typename Fn>
    scope_exit<Fn> make_scope_exit(Fn &&fn) noexcept;

} // namespace cho::osbase::core

#include "Scope.inl"