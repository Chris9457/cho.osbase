// \file  osbasictype.h
// \brief Declaration of the enum BasicType

#pragma once
#include <string>

/**
 * \defgroup PACKAGE_BASICTYPE The Basic Types
 *
 * \brief Definition of different basic (fundamental) types managed by the Serialzation
 *
 * The (key-)serialization manages the following basic types:
 *  - boolean
 *  - integer (signed & unsigned)
 *  - floating (float & double)
 *  - string (std::string & std::wstring)
 *  - array : list of the same types desctibes above + composite
 *  - composite : list of the same types desctibes above + composite
 *
 * \ingroup PACKAGE_SERIALIZATION
 */

namespace cho::osbase::core {

    /**
     * \brief 	This enum represents:
     *				- the basic types used for the serialization
     * \ingroup PACKAGE_BASICTYPE
     */
    enum class BasicType {
        Void      = 0,      //!< for void type
        Boolean   = 1 << 0, //!< for bool type
        Integer   = 1 << 1, //!< for integer types
        Floating  = 1 << 2, //!< for floating types
        String    = 1 << 3, //!< for string types
        Composite = 1 << 4, //!< for composite types (custom data)
        Array     = 1 << 5  //!< for arrays
    };

    /**
     *  switch/case on types to associate with an enum value BasicType
     * \addtogroup PACKAGE_BASICTYPE
     *  \{*/

    /** generic declaration for composites */
    template <typename T>
    struct BasicTypeT {
        static constexpr BasicType value = BasicType::Composite; //!< related enum constant
    };

    /** specialization for bool */
    template <>
    struct BasicTypeT<bool> {
        static constexpr BasicType value = BasicType::Boolean; //!< related enum constant
    };

    /** specialization for int */
    template <>
    struct BasicTypeT<int> {
        static constexpr BasicType value = BasicType::Integer; //!< related enum constant
    };

    /** specialization for float */
    template <>
    struct BasicTypeT<float> {
        static constexpr BasicType value = BasicType::Floating; //!< related enum constant
    };

    /** specialization for double */
    template <>
    struct BasicTypeT<double> {
        static constexpr BasicType value = BasicType::Floating; //!< related enum constant
    };

    /** specialization for std::wstring */
    template <>
    struct BasicTypeT<std::wstring> {
        static constexpr BasicType value = BasicType::String; //!< related enum constant
    };

    /** specialization for arrays */
    template <typename T>
    struct BasicTypeT<T[]> {
        /**
         * \brief related enum constant
         */
        static constexpr BasicType value = static_cast<BasicType>(static_cast<std::underlying_type_t<BasicType>>(BasicTypeT<T>::value) |
                                                                  static_cast<std::underlying_type_t<BasicType>>(BasicType::Array));
    };

    BasicType operator|(const BasicType lhs, const BasicType rhs); //!< overload of the bitwise OR operator
    BasicType operator&(const BasicType lhs, const BasicType rhs); //!< overload of the bitwise AND operator
    /** \}*/
} // namespace cho::osbase::core
