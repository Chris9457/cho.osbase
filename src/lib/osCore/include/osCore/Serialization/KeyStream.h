// \file  KeyStream.h
// \brief Declaration of the class KeyStream

#pragma once
#include "KeyValue.h"
#include <ostream>
#include <string>
#include <vector>

/**
 * \defgroup PACKAGE_KEYSTREAM The Key-Streams
 *
 * \brief Realizes generic key-serializations
 *
 * \ingroup PACKAGE_SERIALIZATION
 */

namespace NS_OSBASE::core {

    template <typename>
    class KeyStream;
    template <typename TKey>
    using KeyStreamPtr     = std::shared_ptr<KeyStream<TKey>>;
    using StringKeyStream  = KeyStream<std::string>;
    using WStringKeyStream = KeyStream<std::wstring>;

    template <typename TKey>
    using KeyPath = std::vector<TKey>;

    /**
     * \brief	This class represents:
     *				- a stream using hierarchical key-value organization
     * \remark	TKey can be either std::string or std::wstring
     * \ingroup PACKAGE_KEYSTREAM
     */
    template <typename TKey>
    class KeyStream {
    public:
        virtual ~KeyStream() = default;

        /** \{
         *  \brief  Deserialize the stream at the current position - return the default value in case of failure
         */
        virtual bool getValue(const bool &bDefaultValue)                   = 0;
        virtual int getValue(const int &defaultValue)                      = 0;
        virtual double getValue(const double &defaultValue)                = 0;
        virtual std::wstring getValue(const std::wstring &strDefaultValue) = 0;
        template <typename TValue>
        TValue getValue(const TValue &defaultValue);
        /** \} */

        /** \{
         *  \brief  Serialize a value at the current position - the return indicate the success
         */
        virtual bool setValue(const bool &bValue)           = 0;
        virtual bool setValue(const int &value)             = 0;
        virtual bool setValue(const double &value)          = 0;
        virtual bool setValue(const std::wstring &strValue) = 0;
        template <typename TValue>
        bool setValue(const TValue &value);
        /** \} */

        template <typename TValue>
        TValue getKeyValue(const TKey &key, const TValue &defaultValue); //!< Deserialize the stream at the position pointed by <b>key</b> -
                                                                         //!< return the default value in case of failure
        template <typename TValue>
        bool setKeyValue(const TKey &key, const TValue &value); //!< Serialize a value at the current position with the <b>key</b> - the
                                                                //!< return indicate the success

        /**\{ Navigation */
        virtual KeyValue<TKey, void> createKey(const TKey &key) = 0; //!< Create a new key
        KeyValue<TKey, void> createKeys(const KeyPath<TKey> &keys);  //!< Create a new key using the path

        virtual KeyValue<TKey, void> openKey(const TKey &key) = 0; //!< Open an existing key
        KeyValue<TKey, void> openKeys(const KeyPath<TKey> &keys);  //!< Open an existing key using the path

        virtual KeyValue<TKey, void> getCurrentKey()                           = 0; //!< Return the current key
        virtual KeyValue<TKey, void> firstKey(const KeyValue<TKey, void> &key) = 0;
        virtual KeyValue<TKey, void> nextKey(const KeyValue<TKey, void> &key)  = 0;

        virtual bool isKeyExist(const TKey &key) const = 0; //!< Indicates if the key exists
        virtual bool closeKey()                        = 0; //!< Close the current key
        virtual void rewind()                          = 0; //!< Set position to the beginning
        /**\} */

        // Navigation array
        virtual KeyValue<TKey, void> createArray(const TKey &key) = 0; //!< Create a new key array
        virtual KeyValue<TKey, void> openArray(const TKey &key)   = 0; //!< Create a new key array

        virtual KeyValue<TKey, void> createRow()                 = 0; //!< Create a new row at the end of the array
        virtual KeyValue<TKey, void> openRow(const size_t index) = 0; //!< Open the row at the index
        virtual bool resetArray()                                = 0; //!< Reset / clear the array
        virtual size_t size() const                              = 0; //!< Size of the array (number of rows)

        virtual std::vector<unsigned char> getBuffer() const = 0; //!< Return the human readable buffer of the key-stream

    protected:
        /** \cond */
        KeyStream()                  = default;
        KeyStream(const KeyStream &) = default;
        KeyStream(KeyStream &&)      = default;
        KeyStream &operator=(const KeyStream &) = default;
        KeyStream &operator=(KeyStream &&) = default;
        /** \endcond */
    };

    /**
     * \brief Get the list af available key-strem families (concrete realizations)
     * \ingroup PACKAGE_KEYSTREAM
     */
    std::vector<std::string> getKeyStreamFamilies();

    /**
     * \brief Create a key stream by its factory name
     *
     * \param   factoryName name of the factory
     * \ingroup PACKAGE_KEYSTREAM
     */
    KeyStreamPtr<std::string> makeKeyStream(const std::string &factoryName);

    /**
     * \brief Create a key stream by its factory name and an input stream
     *
     * \param   factoryName name of the factory
     * \param   is          input stream to read
     * \ingroup PACKAGE_KEYSTREAM
     */
    KeyStreamPtr<std::string> makeKeyStream(const std::string &factoryName, std::istream &is);

    /**
     * \brief Create a key stream by its factory name and an input stream
     *
     * \param   factoryName name of the factory
     * \param   is          input stream to read
     * \ingroup PACKAGE_KEYSTREAM
     */
    KeyStreamPtr<std::string> makeKeyStream(const std::string &factoryName, std::istream &&is);

    /**
     * \brief Create an empty key-stream based on an Xml concrete realization
     * \remark  The type of the key is a std::string
     * \ingroup PACKAGE_OSCOREIMPL
     */
    KeyStreamPtr<std::string> makeXmlStream();

    /**
     * \brief Create a key-stream based on an Xml concrete realization.
     *
     * The key-stream is filled based on the input stream <em>is</em>
     *
     * \param is    input stream containing the data (copy ref)
     * \remark  The type of the key is a std::string
     * \ingroup PACKAGE_OSCOREIMPL
     */
    KeyStreamPtr<std::string> makeXmlStream(std::istream &is);

    /**
     * \brief Create a key-stream based on an Xml concrete realization.
     *
     * The key-stream is filled based on the input stream <em>is</em>
     *
     * \param is    input stream containing the data (move)
     * \remark  The type of the key is a std::string
     * \ingroup PACKAGE_OSCOREIMPL
     */
    KeyStreamPtr<std::string> makeXmlStream(std::istream &&is);

    /**
     * \brief Create an empty key-stream based on an Json concrete realization
     * \remark  The type of the key is a std::string
     * \ingroup PACKAGE_OSCOREIMPL
     */
    KeyStreamPtr<std::string> makeJsonStream();

    /**
     * \brief Create a key-stream based on an Json concrete realization.
     *
     * The key-stream is filled based on the input stream <em>is</em>
     *
     * \param is    input stream containing the data (copy ref)
     * \remark  The type of the key is a std::string
     * \ingroup PACKAGE_OSCOREIMPL
     */
    KeyStreamPtr<std::string> makeJsonStream(std::istream &is);

    /**
     * \brief Create a key-stream based on an Json concrete realization.
     *
     * The key-stream is filled based on the input stream <em>is</em>
     *
     * \param is    input stream containing the data (move)
     * \remark  The type of the key is a std::string
     * \ingroup PACKAGE_OSCOREIMPL
     */
    KeyStreamPtr<std::string> makeJsonStream(std::istream &&is);
    /** \}*/

    /**
     * \brief Stream insertion
     * \param   keyStream   key-stream to insert in
     * \param   keyValue    key-value to insert
     * \ingroup PACKAGE_KEYSTREAM
     */
    template <typename TKey, typename TValue>
    KeyStream<TKey> &operator<<(KeyStream<TKey> &keyStream, const KeyValue<TKey, TValue> &keyValue);

    /**
     * \brief Stream extraction
     * \param   keyStream   key-stream to extract from
     * \param   keyValue    key-value extracted
     * \ingroup PACKAGE_KEYSTREAM
     */
    template <typename TKey, typename TValue>
    KeyStream<TKey> &operator>>(KeyStream<TKey> &keyStream, KeyValue<TKey, TValue> &keyValue);

    /**
     * \brief Stream inspection
     *
     * Extraction to get a human readable inspection of the key-stream
     *
     * \param   os          output stream that receive the huma readable buffer
     * \param   keyStream   key-stream to inspect
     * \ingroup PACKAGE_KEYSTREAM
     */
    std::ostream &operator<<(std::ostream &os, const KeyStream<std::string> &keyStream);
} // namespace NS_OSBASE::core

#include "KeyStream.inl"
