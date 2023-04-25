// \brief Declaration of the macros used for the srialization

#pragma once
#include "Serializable.h"
#include "osCore/Misc/MacroHelpers.h"
#include <istream>
#include <ostream>

#ifndef NSCORE
#define OS_NSCORE ::NS_OSBASE::core
#endif

/**
 * \brief This macro helper implements the key-serialization of a structure
 *
 * This macro helps to implement the serialization of structures.\n
 * Constraints:
 *  - all fields must be public
 *  - must be placed at the global namespace
 *  .
 *  \n
 *  Example:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 namespace mynamespace {
 struct MyStruct {
    int field1;
    std::string field2;
 }
 OS_SERIALIZE_STRUCT(mynamespace::MyStruct, field1, field2)
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * \ingroup PACKAGE_STREAM
 */
#define OS_SERIALIZE_STRUCT(...) OS_EXPAND(OS_SERIALIZE_STRUCT_MACRO(__VA_ARGS__)(__VA_ARGS__))

// This macro returns OS_SERIALIZE_STRUCT_NOFIELD or OS_SERIALIZE_STRUCT_FIELDS, depending if the list of arguments contains 1 or more
// items
// For the 1st case, the argument is assumed to be only the structure name and thus the no data (no field) to serialize
// For the 2nd case, same for the first argument, the other(s) is/are the field(s) of the stucture to serialize
#define OS_SERIALIZE_STRUCT_MACRO(...) OS_STATIC_JOIN(OS_SERIALIZE_STRUCT_, OS_SERIALIZE_NOFIELD_OR_FIELD(__VA_ARGS__))

// Serialization without fields
#define OS_SERIALIZE_STRUCT_NOFIELD(_struct_name)                                                                                          \
    template <typename TByte>                                                                                                              \
    struct OS_NSCORE::Serializable<_struct_name, TByte> {                                                                                  \
        static bool read(_struct_name &, const std::vector<TByte> &, size_t &) {                                                           \
            return true;                                                                                                                   \
        };                                                                                                                                 \
        static bool write(const _struct_name &, std::vector<TByte> &) {                                                                    \
            return true;                                                                                                                   \
        }                                                                                                                                  \
    };

// Serialization with field(s)
#define OS_SERIALIZE_STRUCT_FIELD(_struct_name, ...)                                                                                       \
    template <typename TByte>                                                                                                              \
    struct OS_NSCORE::Serializable<_struct_name, TByte> {                                                                                  \
        static bool read(_struct_name &value, const std::vector<TByte> &buffer, size_t &pos) {                                             \
            OS_DESERIALIZE_FIELDS(__VA_ARGS__);                                                                                            \
            return true;                                                                                                                   \
        };                                                                                                                                 \
        static bool write(const _struct_name &value, std::vector<TByte> &buffer) {                                                         \
            OS_SERIALIZE_FIELDS(__VA_ARGS__);                                                                                              \
            return true;                                                                                                                   \
        }                                                                                                                                  \
    };

// The field list has 2 patterns: < field > or < field, field, ...> (single or multi)
// The macro OS_SERIALIZE_NOFIELD_OR_FIELD return the suffix "_NOFIELD", _FIELD" if the "..." contains a single argument, "_FIELDS"
// otherwize
#define OS_SERIALIZE_FIELD_SEQ()                                                                                                           \
    FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD,   \
        FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, FIELD, NOFIELD
#define OS_SERIALIZE_FIELDN(_1,                                                                                                            \
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

#define OS_SERIALIZE_NOFIELD_OR_FIELD(...)  OS_SERIALIZE_NOFIELD_OR_FIELD_(__VA_ARGS__, OS_SERIALIZE_FIELD_SEQ())
#define OS_SERIALIZE_NOFIELD_OR_FIELD_(...) OS_EXPAND(OS_SERIALIZE_FIELDN(__VA_ARGS__))

#define OS_DESERIALIZE_FIELDS(...) OS_FOREACH(OS_DESERIALIZE_FIELD, __VA_ARGS__)
#define OS_DESERIALIZE_FIELD(_field)                                                                                                       \
    if (!OS_NSCORE::Serializable<decltype(value._field), TByte>::read(value._field, buffer, pos)) {                                        \
        return false;                                                                                                                      \
    }

#define OS_SERIALIZE_FIELDS(...) OS_FOREACH(OS_SERIALIZE_FIELD, __VA_ARGS__)
#define OS_SERIALIZE_FIELD(_field)                                                                                                         \
    if (!OS_NSCORE::Serializable<decltype(value._field), TByte>::write(value._field, buffer)) {                                            \
        return false;                                                                                                                      \
    }
