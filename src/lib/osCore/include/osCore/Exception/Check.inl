// \brief Declaration of the "check" functions

#pragma once
#include "StructuredExceptionHandler.h"

namespace oscheck {

    template <typename TRet, typename TFunc, typename... TArgs>
    TRet throwIfCrashOrReturn(const TFunc &f, TArgs &&...args) {
        if constexpr (std::is_void_v<TRet>) {
            NS_OSBASE::core::TheStructuredExceptionHandler.checkStructuredException<void>(f, std::forward<TArgs>(args)...);
        } else {
            return NS_OSBASE::core::TheStructuredExceptionHandler.checkStructuredException<TRet>(f, std::forward<TArgs>(args)...);
        }
    }

    template <typename TFunc, typename... TArgs>
    void throwIfCrash(const TFunc &f, TArgs &&...args) {
        throwIfCrashOrReturn<void>(f, std::forward<TArgs>(args)...);
    }

    template <typename TLeft, typename TRight>
    void throwIfNE(const TLeft &lhs, const TRight &rhs) {
        throwIfTrue(lhs != rhs);
    }

    template <typename TLeft, typename TRight>
    void throwIfEQ(const TLeft &lhs, const TRight &rhs) {
        throwIfTrue(lhs == rhs);
    }

    template <typename TLeft, typename TRight>
    void throwIfNotLT(const TLeft &lhs, const TRight &rhs) {
        throwIfFalse(lhs < rhs);
    }

    template <typename TLeft, typename TRight>
    void throwIfNotLE(const TLeft &lhs, const TRight &rhs) {
        throwIfFalse(lhs <= rhs);
    }

    template <typename TLeft, typename TRight>
    void throwIfNotGT(const TLeft &lhs, const TRight &rhs) {
        throwIfFalse(lhs > rhs);
    }

    template <typename TLeft, typename TRight>
    void throwIfNotGE(const TLeft &lhs, const TRight &rhs) {
        throwIfFalse(lhs >= rhs);
    }

} // namespace oscheck
