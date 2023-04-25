// \file  Task.inl
// \brief Implementation of the class Task

#pragma once
#include <functional>
#include <type_traits>

#define TASK_ASSERT_VALUE "All arguments must be passed by value"

namespace NS_OSBASE::application {

    namespace internal {

        ////////////////////////////////////////////////////////////////////////
        // Associates all calls to their std::function equivalent

        // For generic types that are functors, delegate to its 'operator()'
        // Useful to convert lambda types to their std::function equivalent
        template <typename T>
        struct function_traits : function_traits<decltype(&T::operator())> {};

        // for pointers to member function const
        template <typename TClass, typename TRet, typename... TArgs>
        struct function_traits<TRet (TClass::*)(TArgs...) const> {
            using type = std::function<TRet(TArgs...)>;
        };

        // for pointers to member function
        template <typename TClass, typename TRet, typename... TArgs>
        struct function_traits<TRet (TClass::*)(TArgs...)> {
            using type = std::function<TRet(TArgs...)>;
        };

        // for function pointers
        template <typename TRet, typename... TArgs>
        struct function_traits<TRet (*)(TArgs...)> {
            using type = std::function<TRet(TArgs...)>;
        };
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Check if all types are "decay"
        template <typename... TArgs>
        struct is_all_decay;

        template <typename T>
        struct is_all_decay<T> {
            static constexpr bool value = std::is_same_v<T, std::decay_t<T>>;
        };

        template <typename H, typename... Q>
        struct is_all_decay<H, Q...> {
            static constexpr bool value = is_all_decay<H>::value && is_all_decay<Q...>::value;
        };
        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // Check if all aguments are passed by value
        template <typename T>
        struct is_all_args_passed_by_value {
            static constexpr bool value = is_all_decay<typename function_traits<T>::type>::value;
        };

        template <typename TRet>
        struct is_all_args_passed_by_value<TRet()> {
            static constexpr bool value = true;
        };

        template <typename TRet, typename... TArgs>
        struct is_all_args_passed_by_value<TRet(TArgs...)> {
            static constexpr bool value = is_all_decay<TArgs...>::value;
        };

        template <typename TRet, typename... TArgs>
        struct is_all_args_passed_by_value<std::function<TRet(TArgs...)>> {
            static constexpr bool value = is_all_args_passed_by_value<TRet(TArgs...)>::value;
        };
        ////////////////////////////////////////////////////////////////////////

    } // namespace internal

    /*
     * \class Task
     */
    template <typename TRet, typename TCallback, typename... TArgs>
    TypedTask<TRet, TCallback, TArgs...>::TypedTask(const TCallback &callback, TArgs &&...args)
        : m_callback(callback), m_args(std::forward<TArgs>(args)...) {
        using TFunction = typename internal::function_traits<TCallback>::type;
        static_assert(internal::is_all_args_passed_by_value<TFunction>::value, TASK_ASSERT_VALUE);
    }

    template <typename TRet, typename TCallback, typename... TArgs>
    TRet TypedTask<TRet, TCallback, TArgs...>::execute() {
        if constexpr (std::is_void_v<TRet>) {
            if (m_bEnabled) {
                std::apply(m_callback, m_args);
            }
        } else {
            if (m_bEnabled) {
                return std::apply(m_callback, m_args);
            }
            return TRet{};
        }
    }

    template <typename TRet, typename TCallback, typename... TArgs>
    bool TypedTask<TRet, TCallback, TArgs...>::isEnabled() const {
        return m_bEnabled;
    }

    template <typename TRet, typename TCallback, typename... TArgs>
    void TypedTask<TRet, TCallback, TArgs...>::setEnabled(const bool bEnabled) {
        m_bEnabled = bEnabled;
    }

    // maker
    template <typename TRet, typename TCallback, typename... TArgs>
    ITypedTaskPtr<TRet> makeTypedTask(const TCallback &callback, TArgs &&...args) {
        using TFunction = typename internal::function_traits<TCallback>::type;
        static_assert(internal::is_all_args_passed_by_value<TFunction>::value, TASK_ASSERT_VALUE);
        return std::make_shared<TypedTask<TRet, TCallback, TArgs...>>(callback, std::forward<TArgs>(args)...);
    }

    template <typename TCallback, typename... TArgs>
    ITaskPtr makeTask(const TCallback &callback, TArgs &&...args) {
        using TFunction = typename internal::function_traits<TCallback>::type;
        static_assert(internal::is_all_args_passed_by_value<TFunction>::value, TASK_ASSERT_VALUE);
        return makeTypedTask<void>(callback, std::forward<TArgs>(args)...);
    }

    template <typename TRet, typename TInstance, typename TMthCallback, typename... TArgs>
    ITypedTaskPtr<TRet> makeTypedMethodTask(std::shared_ptr<TInstance> pInstance, const TMthCallback &mthCallback, TArgs &&...args) {
        using TFunction = typename internal::function_traits<TMthCallback>::type;
        static_assert(internal::is_all_args_passed_by_value<TFunction>::value, TASK_ASSERT_VALUE);

        return makeTypedTask<TRet>([pWInstance = std::weak_ptr<TInstance>(pInstance), mthCallback, args...]() {
            auto pLockedInstance = pWInstance.lock();
            if (pLockedInstance != nullptr) {
                return (pLockedInstance.get()->*mthCallback)(args...);
            }

            return TRet{};
        });
    }

    template <typename TInstance, typename TMthCallback, typename... TArgs>
    ITaskPtr makeMethodTask(std::shared_ptr<TInstance> pInstance, const TMthCallback &mthCallback, TArgs &&...args) {
        using TFunction = typename internal::function_traits<TMthCallback>::type;
        static_assert(internal::is_all_args_passed_by_value<TFunction>::value, TASK_ASSERT_VALUE);

        return makeTypedTask<void>([pWInstance = std::weak_ptr<TInstance>(pInstance), mthCallback, args...]() {
            auto pLockedInstance = pWInstance.lock();
            if (pLockedInstance != nullptr) {
                (pLockedInstance.get()->*mthCallback)(args...);
            }
        });
    }
} // namespace NS_OSBASE::application
