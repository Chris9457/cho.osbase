// \file  Scope.inl
// \brief Implementation of the class scope_exit

namespace cho::osbase::core {

    /*
     * \class scope_exit
     */
    template <class Fn>
    scope_exit<Fn>::scope_exit(Fn &&fn) noexcept : m_fn(std::move(fn)), m_run(true) {
    }

    template <class Fn>
    scope_exit<Fn>::scope_exit(scope_exit &&other) noexcept : m_fn(std::move(other.m_fn)), m_run(true) {
        other.m_run = false;
    }

    template <class Fn>
    scope_exit<Fn>::~scope_exit() {
        if (m_run)
            m_fn();
    }

    template <class Fn>
    void scope_exit<Fn>::release() noexcept {
        m_run = false;
    }

    /*
     * \function make_scope_exit
     */
    template <typename Fn>
    scope_exit<Fn> make_scope_exit(Fn &&fn) noexcept {
        return scope_exit<Fn>(std::forward<Fn>(fn));
    }

} // namespace cho::osbase::core
