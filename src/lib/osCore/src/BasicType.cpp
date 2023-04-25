// \file  BasicType.cpp
// \brief Implementation of the operators for the enum BasicType

#include "osCore/Serialization/BasicType.h"
#include <type_traits>

namespace NS_OSBASE::core {
    /*
     * \Operators
     */
    BasicType operator|(const BasicType lhs, const BasicType rhs) {
        using EnumType = std::underlying_type_t<BasicType>;
        return static_cast<BasicType>(static_cast<EnumType>(lhs) | static_cast<EnumType>(rhs));
    }

    BasicType operator&(const BasicType lhs, const BasicType rhs) {
        using EnumType = std::underlying_type_t<BasicType>;
        return static_cast<BasicType>(static_cast<EnumType>(lhs) & static_cast<EnumType>(rhs));
    }
} // namespace NS_OSBASE::core
