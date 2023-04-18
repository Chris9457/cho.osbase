// \file  KeyValue.h
// \brief Declaration of the class KeyValue

#pragma once
#include "BasicType.h"
#include <tuple>
#include <vector>

/**
 * \defgroup PACKAGE_KEYVALUE The Key-Values
 *
 * \brief Definition of classes KeyValue
 *
 * A key-value represents an entry of a dictionnary
 *
 * \par Creating a Key
 * To create a key, use the function makeKey():
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 auto const myKey1 = makeKey("myKey"); // Return a KeyValue<std::string, void>
 auto const myKey2 = makeKey(L"myKey"); // Return a KeyValue<std::wstring, void>
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * \par Creating a Key-value
 * To create a key, use the function makeKeyValue():
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 ...
 auto const myKeyValue1 = makeKeyValue("myKey", 123); // Return a KeyValue<std::string, int>
 auto const myKeyValue2 = makeKeyValue(L"myKey", MyStruct({...})); // Return a KeyValue<std::wstring, MyStruct>
 ...
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * \ingroup PACKAGE_SERIALIZATION
 */

namespace cho::osbase::core {

#pragma region UntypedKeyValue
    /**
     * \brief	This class represents:
     *				- the base class for key values
     * \internal
     * \ingroup PACKAGE_KEYVALUE
     */
    template <typename TKey>
    class UntypedKeyValue {
    public:
        using KeyType = TKey; //!< Alias for the type of key

        virtual ~UntypedKeyValue() = default;

        const TKey &getKey() const;   //!< return the key
        void setKey(const TKey &key); //!< assign the key

        bool operator==(const UntypedKeyValue &rhs) const; //!< Comparison operator equal
        bool operator!=(const UntypedKeyValue &rhs) const; //!< Comparison operator not equal

        virtual BasicType getBasicType() const = 0; //!< Return the related basic type
        bool isNull() const;                        //!< indicate a null key-value, ie unusable object

    protected:
        UntypedKeyValue();
        explicit UntypedKeyValue(const TKey &key);                          //!< ctor with a key
        UntypedKeyValue(const UntypedKeyValue &other) = default;            //!< copy ctor
        UntypedKeyValue(UntypedKeyValue &&other)      = default;            //!< move ctor
        UntypedKeyValue &operator=(const UntypedKeyValue &other) = default; //!< copy assignment
        UntypedKeyValue &operator=(UntypedKeyValue &&other) = default;      //!< move assignment

    private:
        TKey m_key   = {};
        bool m_bNull = false;
    };
#pragma endregion

#pragma region KeyValue < TKey, TValue>
    /**
     * \brief	This class represents a pair key - value
     * \ingroup PACKAGE_KEYVALUE
     */
    template <typename TKey, typename TValue>
    class KeyValue : public UntypedKeyValue<TKey> {
    public:
        using ValueType = TValue; //!< Alias for the type of value

        KeyValue(const TKey &key, const ValueType &value); //!< Ctor with key and value
        KeyValue(const TKey &key, ValueType &&value);      //!< Ctor with key and value

        const ValueType &getValue() const;     //!< Return the value
        ValueType getValue();                  //!< Return the value
        void setValue(const ValueType &value); //!< Assign the value
        void setValue(ValueType &&value);      //!< Assign the value

        BasicType getBasicType() const override;

        bool operator==(const KeyValue &rhs) const; //!< Comparison operator equal
        bool operator!=(const KeyValue &rhs) const; //!< Comparison operator not equal

        static const KeyValue &null(); //!< return a null key-value, ie unusable object (can be checked with isNull)

    protected:
        KeyValue();

    private:
        TValue m_value;
    };
#pragma endregion

#pragma region KeyValue < TKey, TValue[]>
    /**
     * \brief	This class represents a pair key - value for arrays
     * \ingroup PACKAGE_KEYVALUE
     */
    template <typename TKey, typename TValue>
    class KeyValue<TKey, TValue[]> : public UntypedKeyValue<TKey> {
    public:
        using ValueType = std::vector<TValue>; //!< Alias for the type of value

        KeyValue(const TKey &key, const ValueType &value); //!< ctor with a key and a value

        const ValueType &getValue() const;     //!< return the array
        void setValue(const ValueType &value); //!< assign the array

        TValue getValue(const size_t index) const; //!< get the value at the index <em>index</em> - remark : the index is not checked
                                                   //!< (reponsibility to the caller)
        void setValue(const size_t index, const TValue &value); //!< assign the value <em>value</em> at the index <em>index</em> - remark :
                                                                //!< the index is not checked (reponsibility to the caller)
        void addValue(const TValue &value);                     //!< add the value <em>value</em> at the end of the array
        void clear();                                           //!< clear (reset) the array - the new size is 0
        size_t size() const;                                    //!< number of element of the array

        BasicType getBasicType() const override;

        bool operator==(const KeyValue &rhs) const; //!< Comparison operator equal
        bool operator!=(const KeyValue &rhs) const; //!< Comparison operator not equal

        typename ValueType::const_reference operator[](const size_t index) const; //!< get the value at the index <em>index</em>
        typename ValueType::reference operator[](const size_t index);             //!< get the value at the index <em>index</em>

        static const KeyValue &null(); //!< \copydoc KeyValue::null

    private:
        KeyValue() = default;
        ValueType m_value;
    };

    /**
     * \brief	Specialization for vector
     * \ingroup PACKAGE_KEYVALUE
     */
    template <typename TKey, typename TValue>
    class KeyValue<TKey, std::vector<TValue>> final : public KeyValue<TKey, TValue[]> {
    public:
        using ValueType = std::vector<TValue>; //!< Alias for the type of value

        KeyValue(const TKey &key, const ValueType &value);        //!< ctor with a key and a value
        KeyValue &operator=(const KeyValue<TKey, TValue[]> &rhs); //!< assignment operator
    };
#pragma endregion

#pragma region KeyValue < TKey, void>
    /**
     * \brief	This class represents a pair key - value specialized for void
     * \ingroup PACKAGE_KEYVALUE
     */
    template <typename TKey>
    class KeyValue<TKey, void> : public UntypedKeyValue<TKey> {
    public:
        explicit KeyValue(const TKey &key); //!< ctor with a key

        BasicType getBasicType() const override;

        bool operator==(const KeyValue &rhs) const; //!< Comparison operator equal
        bool operator!=(const KeyValue &rhs) const; //!< Comparison operator not equal

        static const KeyValue &null(); //!< \copydoc KeyValue::null

    private:
        KeyValue() = default;
    };
#pragma endregion

#pragma region Makers
    /**
     * \brief create an "non-typed" key-value
     *
     * \param key  identifier (name in case of string) of the key
     * \ingroup PACKAGE_KEYVALUE
     */
    template <typename TKey>
    KeyValue<TKey, void> makeKey(const TKey &key);

    /**
     * \brief create a typed key-value with a default value
     *
     * \param key   identifier (name in case of string) of the key
     * \ingroup PACKAGE_KEYVALUE
     */
    template <typename TValue, typename TKey>
    KeyValue<TKey, TValue> makeKeyValue(const TKey &key);

    /**
     * \brief create a typed key-value
     *
     * \param key   identifier (name in case of string) of the key
     * \param value value of the key value
     * \ingroup PACKAGE_KEYVALUE
     */
    template <typename TValue, typename TKey>
    KeyValue<TKey, TValue> makeKeyValue(const TKey &key, const TValue &value);

    /** \cond */
    // Specific declaration for char* & wchar_t*
    template <typename TKey>
    KeyValue<TKey, std::string> makeKeyValue(const TKey &key, const char *str);

    template <typename TKey>
    KeyValue<TKey, std::wstring> makeKeyValue(const TKey &key, const wchar_t *wStr);

    // Specific declaration for tuples
    template <typename TKey, typename TFirst, typename TSecond, typename... TArgs>
    KeyValue<TKey, std::tuple<TFirst, TSecond, TArgs...>> makeKeyValue(
        const TKey &key, TFirst &&first, TSecond &&second, TArgs &&...args); // minimum 2 args to create a tuple
    /** \endcond */

#pragma endregion

} // namespace cho::osbase::core

#include "KeyValue.inl"
