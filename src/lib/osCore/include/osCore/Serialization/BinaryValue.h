// \brief Declaration of the class BinaryValue

#pragma once

namespace cho::osbase::core {

    /**
     * \addtogroup PACKAGE_STREAM
     * \{
     */

    /**
     * \brief Utility class used to overload the stream operators
     */
    template <typename TValue>
    class BinaryValue {
    public:
        BinaryValue() = default;                   //!< default ctor
        explicit BinaryValue(const TValue &value); //!< ctor that copy the value
        explicit BinaryValue(TValue &&value);      //!< ctor that move the value

        const TValue &get() const;     //!< return the value for reading
        TValue &get();                 //!< return the value for writing
        void set(const TValue &value); //!< set the value by copy
        void set(TValue &&value);      //!< set the value by move

        bool operator==(const BinaryValue &other) const; //!< equality comparison
        bool operator!=(const BinaryValue &other) const; //!< non-equality comparison

    private:
        TValue m_value = {};
    };

    template <typename TValue>
    auto makeBinaryValue(const TValue &value = TValue{}); //!< create a binary value by copy

    template <typename TValue>
    auto makeBinaryValue(TValue &&value); //!< create a binary value by move
    /** \} */
} // namespace cho::osbase::core

#include "BinaryValue.inl"
