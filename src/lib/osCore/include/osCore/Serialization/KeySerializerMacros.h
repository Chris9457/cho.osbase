// \brief Macros helpers for KeySerializer

#pragma once
#include "KeySerializer.h"
#include "osCore/Misc/MacroHelpers.h"
#include <string>
#include <utility>

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
 OS_KEY_SERIALIZE_STRUCT(mynamespace::MyStruct, field1, field2)
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * \ingroup PACKAGE_KEYSTREAM
 */
#define OS_KEY_SERIALIZE_STRUCT(...) OS_KEY_SERIALIZE_STRUCT_MACRO(__VA_ARGS__)(__VA_ARGS__)

// This macro returns OS_KEY_SERIALIZE_STRUCT_FIELD or OS_KEY_SERIALIZE_STRUCT_FIELDS, depending if the list of arguments contains 1 or more
// items
// For the 1st case, the argument is assumed to be only the structure name and thus the no data (field) to serialize
// For the 2nd case, same for the first argument, the other(s) is/are the field(s) of the stucture to serialize
#define OS_KEY_SERIALIZE_STRUCT_MACRO(...) OS_EXPAND(OS_STATIC_JOIN(OS_KEY_SERIALIZE_STRUCT, OS_KEY_SERIALIZE_FIELD_OR_FIELDS(__VA_ARGS__)))

// Serialization without fields
#define OS_KEY_SERIALIZE_STRUCT_FIELD(_struct_name)                                                                                        \
    template <>                                                                                                                            \
    struct OS_NSCORE::KeySerializer<std::string, _struct_name> {                                                                           \
        static inline _struct_name getValue(KeyStream<std::string> &, const _struct_name &) {                                              \
            return {};                                                                                                                     \
        }                                                                                                                                  \
        static inline bool setValue(KeyStream<std::string> &, const _struct_name &) {                                                      \
            return true;                                                                                                                   \
        }                                                                                                                                  \
    };

// Serialization with field(s)
#define OS_KEY_SERIALIZE_STRUCT_FIELDS(_struct_name, ...)                                                                                  \
    template <>                                                                                                                            \
    struct OS_NSCORE::KeySerializer<std::string, _struct_name> {                                                                           \
    private:                                                                                                                               \
        using type = _struct_name;                                                                                                         \
                                                                                                                                           \
    public:                                                                                                                                \
        static inline _struct_name getValue(KeyStream<std::string> &keyStream, const _struct_name &defaultValue) {                         \
            _struct_name result{};                                                                                                         \
            OS_KEY_DESERIALIZE_FIELD_LIST(__VA_ARGS__)                                                                                     \
            return result;                                                                                                                 \
        }                                                                                                                                  \
        static bool setValue(KeyStream<std::string> &keyStream, const _struct_name &value) {                                               \
            keyStream OS_KEY_SERIALIZE_FIELD_LIST(__VA_ARGS__);                                                                            \
            return true;                                                                                                                   \
        }                                                                                                                                  \
    };

// The field list has 2 patterns: < field > or < field, field, ...> (single or multi)
// The macro OS_KEY_SERIALIZE_FIELD_OR_FIELDS return the suffix "_NOFIELD", _FIELD" if the "..." contains a single argument, "_FIELDS"
// otherwize
#define OS_KEY_SERIALIZE_FIELD_SEQ()                                                                                                       \
    _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, \
        _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS, _FIELDS,      \
        _FIELD
#define OS_KEY_SERIALIZE_FIELDN(_1,                                                                                                        \
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

#define OS_KEY_SERIALIZE_FIELD_OR_FIELDS(...)  OS_KEY_SERIALIZE_FIELD_OR_FIELDS_(__VA_ARGS__, OS_KEY_SERIALIZE_FIELD_SEQ())
#define OS_KEY_SERIALIZE_FIELD_OR_FIELDS_(...) OS_EXPAND(OS_KEY_SERIALIZE_FIELDN(__VA_ARGS__))

#define OS_KEY_SERIALIZE_FIRST_FIELD(_1, ...)  _1
#define OS_KEY_SERIALIZE_OTHER_FIELDS(_1, ...) __VA_ARGS__

// By joining this suffix with a base name, the macro OS_KEY_DESERIALIZE_FIELD_MACRO return OS_KEY_DESERIALIZE_FIELD or
// OS_KEY_DESERIALIZE_FIELDS
#define OS_KEY_DESERIALIZE_FIELD_MACRO(...) OS_EXPAND(OS_STATIC_JOIN(OS_KEY_DESERIALIZE, OS_KEY_SERIALIZE_FIELD_OR_FIELDS(__VA_ARGS__)))
#define OS_KEY_DESERIALIZE_FIELD_LIST(...)  OS_KEY_DESERIALIZE_FIELD_MACRO(__VA_ARGS__)(__VA_ARGS__)

#define OS_KEY_DESERIALIZE_FIELDS(...)                                                                                                     \
    OS_KEY_DESERIALIZE_FIELD(OS_KEY_SERIALIZE_FIRST_FIELD(__VA_ARGS__))                                                                    \
    OS_FOREACH(OS_KEY_DESERIALIZE_NEXT_FIELD, OS_KEY_SERIALIZE_OTHER_FIELDS(__VA_ARGS__))

#define OS_KEY_DESERIALIZE_NEXT_FIELD(_field)                                                                                              \
    ;                                                                                                                                      \
    OS_KEY_DESERIALIZE_FIELD(_field)
#define OS_KEY_DESERIALIZE_FIELD(_field) result._field = keyStream.getKeyValue(OS_TO_STR(_field), defaultValue._field);

// By joining this suffix with a base name, the macro OS_KEY_SERIALIZE_FIELD_MACRO return OS_KEY_SERIALIZE_FIELD or
// OS_KEY_SERIALIZE_FIELDS
#define OS_KEY_SERIALIZE_FIELD_MACRO(...) OS_EXPAND(OS_STATIC_JOIN(OS_KEY_SERIALIZE, OS_KEY_SERIALIZE_FIELD_OR_FIELDS(__VA_ARGS__)))
#define OS_KEY_SERIALIZE_FIELD_LIST(...)  OS_KEY_SERIALIZE_FIELD_MACRO(__VA_ARGS__)(__VA_ARGS__)

#define OS_KEY_SERIALIZE_FIELDS(...)   OS_FOREACH(OS_KEY_SERIALIZE_FIELD, __VA_ARGS__)
#define OS_KEY_SERIALIZE_FIELD(_field) << OS_NSCORE::makeKeyValue(std::string(OS_TO_STR(_field)), value._field)
