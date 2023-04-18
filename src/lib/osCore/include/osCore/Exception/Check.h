// \brief Declaration of the "check" functions

#pragma once

namespace oscheck {
    /**
     * \addtogroup PACKAGE_EXCEPTION
     * \{
     */

    /**
     * \brief check if the function doesn't generate a crash (Structured Exception Handling, aka SEH)
     * \param f     function to call
     * \param args  arguments of the function
     * \return the result of f
     * \throws StructuredException if crash
     */
    template <typename TRet, typename TFunc, typename... TArgs>
    TRet throwIfCrashOrReturn(const TFunc &f, TArgs &&...args);

    /**
     * \brief check if the function doesn't generate a crash (Structured Exception Handling, aka SEH)
     * \param f     function to call
     * \param args  arguments of the function
     * \throws StructuredException if crash
     */
    template <typename TFunc, typename... TArgs>
    void throwIfCrash(const TFunc &f, TArgs &&...args);

    /**
     * \brief check if the expression is true
     * \param exp Expression to check
     * \throws StructuredException if false
     */
    void throwIfFalse(const bool exp);

    /**
     * \brief check if the expression is false
     * \param exp Expression to check
     * \throws StructuredException if true
     */
    void throwIfTrue(const bool exp);

    /**
     * \brief check if left & right are equal
     * \param lhs   left value
     * \param rhs   right value
     * \throws StructuredException if left & right are NOT equal
     */
    template <typename TLeft, typename TRight>
    void throwIfNE(const TLeft &lhs, const TRight &rhs);

    /**
     * \brief check if left & right are NOT equal
     * \param lhs   left value
     * \param rhs   right value
     * \throws StructuredException if left & right are equal
     */
    template <typename TLeft, typename TRight>
    void throwIfEQ(const TLeft &lhs, const TRight &rhs);

    /**
     * \brief check if left is lesser (strict) than right
     * \param lhs   left value
     * \param rhs   right value
     * \throws StructuredException if left is NOT lesser (strict) than right
     */
    template <typename TLeft, typename TRight>
    void throwIfNotLT(const TLeft &lhs, const TRight &rhs);

    /**
     * \brief check if left is lesseer or equal to right
     * \param lhs   left value
     * \param rhs   right value
     * \throws StructuredException if left is NOT lesser or equal to right
     */
    template <typename TLeft, typename TRight>
    void throwIfNotLE(const TLeft &lhs, const TRight &rhs);

    /**
     * \brief check if left is greater than right
     * \param lhs   left value
     * \param rhs   right value
     * \throws StructuredException if left is not greater than right
     */
    template <typename TLeft, typename TRight>
    void throwIfNotGT(const TLeft &lhs, const TRight &rhs);

    /**
     * \brief check if left is greater or equal to right
     * \param lhs   left value
     * \param rhs   right value
     * \throws StructuredException if left is NOT greater or equal to right
     */
    template <typename TLeft, typename TRight>
    void throwIfNotGE(const TLeft &lhs, const TRight &rhs);

    /** \} */
} // namespace oscheck

#include "Check.inl"
