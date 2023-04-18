// \file  MacroHelpers.h
// \brief Declaration of various tools...

#pragma once

/**
 * \defgroup MACRO_HELPERS Macros Helpers
 *
 * \brief Set of helpers macros
 *
 * \ingroup PACKAGE_MISC
 */

/**
 * \brief     Join the left part and the right part
 * \param[in] _left   Left part
 * \param[in] _right  Right part
 * \ingroup MACRO_HELPERS
 */
#define OS_STATIC_JOIN(_left, _right) _OS_STATIC_JOIN1(_left, _right)

/** \cond */
#define _OS_STATIC_JOIN1(_left, _right) _OS_STATIC_JOIN2(_left, _right)
#define _OS_STATIC_JOIN2(_left, _right) _left##_right
/** \endcond */

/**
 * \brief     Expand the argument, used for macros building
 * \param[in] _x	argument to expand
 * \ingroup MACRO_HELPERS
 */
#define OS_EXPAND(_x) _x
#define OS_TO_STR(_x) #_x

/**
 * \brief	Apply the macro passed as the 1st argument for each argument
 * \param[in]	_what	macro to use
 * \param[in]	...		list of arguments to apply to the macro
 * \ingroup MACRO_HELPERS
 */
#define OS_FOREACH(_what, ...) OS_FE_(OS_FE_NARG(__VA_ARGS__), _what, __VA_ARGS__)

/** \cond */
/** This painful implementation is used due to a bug with VS2010 around the expansion of the macro __VA_ARGS__*/
#define OS_FE_1(_what, _x)       _what(_x)
#define OS_FE_2(_what, _x, ...)  _what(_x) OS_EXPAND(OS_FE_1(_what, __VA_ARGS__))
#define OS_FE_3(_what, _x, ...)  _what(_x) OS_EXPAND(OS_FE_2(_what, __VA_ARGS__))
#define OS_FE_4(_what, _x, ...)  _what(_x) OS_EXPAND(OS_FE_3(_what, __VA_ARGS__))
#define OS_FE_5(_what, _x, ...)  _what(_x) OS_EXPAND(OS_FE_4(_what, __VA_ARGS__))
#define OS_FE_6(_what, _x, ...)  _what(_x) OS_EXPAND(OS_FE_5(_what, __VA_ARGS__))
#define OS_FE_7(_what, _x, ...)  _what(_x) OS_EXPAND(OS_FE_6(_what, __VA_ARGS__))
#define OS_FE_8(_what, _x, ...)  _what(_x) OS_EXPAND(OS_FE_7(_what, __VA_ARGS__))
#define OS_FE_9(_what, _x, ...)  _what(_x) OS_EXPAND(OS_FE_8(_what, __VA_ARGS__))
#define OS_FE_10(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_9(_what, __VA_ARGS__))
#define OS_FE_11(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_10(_what, __VA_ARGS__))
#define OS_FE_12(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_11(_what, __VA_ARGS__))
#define OS_FE_13(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_12(_what, __VA_ARGS__))
#define OS_FE_14(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_13(_what, __VA_ARGS__))
#define OS_FE_15(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_14(_what, __VA_ARGS__))
#define OS_FE_16(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_15(_what, __VA_ARGS__))
#define OS_FE_17(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_16(_what, __VA_ARGS__))
#define OS_FE_18(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_17(_what, __VA_ARGS__))
#define OS_FE_19(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_18(_what, __VA_ARGS__))
#define OS_FE_20(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_19(_what, __VA_ARGS__))
#define OS_FE_21(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_20(_what, __VA_ARGS__))
#define OS_FE_22(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_21(_what, __VA_ARGS__))
#define OS_FE_23(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_22(_what, __VA_ARGS__))
#define OS_FE_24(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_23(_what, __VA_ARGS__))
#define OS_FE_25(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_24(_what, __VA_ARGS__))
#define OS_FE_26(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_25(_what, __VA_ARGS__))
#define OS_FE_27(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_26(_what, __VA_ARGS__))
#define OS_FE_28(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_27(_what, __VA_ARGS__))
#define OS_FE_29(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_28(_what, __VA_ARGS__))
#define OS_FE_30(_what, _x, ...) _what(_x) OS_EXPAND(OS_FE_29(_what, __VA_ARGS__))

#define OS_FE_NARG(...)  OS_FE_NARG_(__VA_ARGS__, OS_FE_RSEQ_N())
#define OS_FE_NARG_(...) OS_EXPAND(OS_FE_ARG_N(__VA_ARGS__))

#define OS_FE_ARG_N(_1,                                                                                                                    \
    _2,                                                                                                                                    \
    _3,                                                                                                                                    \
    _4,                                                                                                                                    \
    _5,                                                                                                                                    \
    _6,                                                                                                                                    \
    _7,                                                                                                                                    \
    _8,                                                                                                                                    \
    _9,                                                                                                                                    \
    _10,                                                                                                                                   \
    _11,                                                                                                                                   \
    _12,                                                                                                                                   \
    _13,                                                                                                                                   \
    _14,                                                                                                                                   \
    _15,                                                                                                                                   \
    _16,                                                                                                                                   \
    _17,                                                                                                                                   \
    _18,                                                                                                                                   \
    _19,                                                                                                                                   \
    _20,                                                                                                                                   \
    _21,                                                                                                                                   \
    _22,                                                                                                                                   \
    _23,                                                                                                                                   \
    _24,                                                                                                                                   \
    _25,                                                                                                                                   \
    _26,                                                                                                                                   \
    _27,                                                                                                                                   \
    _28,                                                                                                                                   \
    _29,                                                                                                                                   \
    _30,                                                                                                                                   \
    _n,                                                                                                                                    \
    ...)                                                                                                                                   \
    _n

#define OS_FE_RSEQ_N()         30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define OS_FE_(_n, _what, ...) OS_EXPAND(OS_STATIC_JOIN(OS_FE_, _n)(_what, __VA_ARGS__))
/** \endcond */
