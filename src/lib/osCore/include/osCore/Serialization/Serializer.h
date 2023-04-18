// \brief Declaration of the class Serializer

#pragma once
#include "BinaryValue.h"
#include "SerializableMacros.h"
#include "osCore/Exception/RuntimeException.h"
#include <iostream>

/**
 * \defgroup PACKAGE_STREAM The Streams
 *
 * \brief Realizes generic serializations
 *
 * \ingroup PACKAGE_SERIALIZATION
 */

namespace cho::osbase::core {
    /**
     * \addtogroup PACKAGE_STREAM
     * \{
     */

    /**
     * \brief Exception thrown in case of serilization error
     */
    class SerializerException : public RuntimeException {
        using RuntimeException::RuntimeException;
    };

    /**
     * \brief Utility class used to read and write serialized data on standard steams
     */
    template <typename TValue>
    struct Serializer {
        static TValue getValue(std::istream &is);                    //!< deserialize the value
        static void setValue(std::ostream &os, const TValue &value); //!< serializa the value
    };

    template <typename TValue>
    std::istream &operator>>(std::istream &is, BinaryValue<TValue> &value); //!< stream "deserialize" operator

    template <typename TValue>
    std::ostream &operator<<(std::ostream &os, const BinaryValue<TValue> &value); //!< stream "serialize" operator
    /** \} */

} // namespace cho::osbase::core

#include "Serializer.inl"
