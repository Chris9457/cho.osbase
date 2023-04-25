// \brief Just a public header to define the DataExchange Uri abstraction
//        other std::string

#pragma once
#include "osCore/Exception/LogicException.h"
#include "osCore/Misc/TypeCast.h"
#include "osCore/Serialization/CoreKeySerializer.h"
#include "osCore/Serialization/KeySerializerMacros.h"
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>

namespace NS_OSBASE::data {

    /**
     * \brief exception thrown when trying to parse a bad uri
     */
    class BadUriException : public core::LogicException {
        using LogicException::LogicException;
    };

    /**
     * \brief this struct represent an Uri
     * \ingroup PACKAGE_OSDATA
     */
    struct Uri {

        /**
         * \brief host of the authority
         * \remark Performs a mapping with specific host (ex: localhost ==> 127.0.0.1)
         */
        struct Host {
            Host() = default;               //!< ctor
            Host(const std::string &other); //!< ctor with value

            Host &operator=(const std::string &other); //!< set the internal host
            operator std::string() const;              //!< return the host (with mapping)

            bool empty() const;                              //!< indeicates if the host is empty
            bool operator==(const std::string &other) const; //!< comparison another host

            bool isLocal() const;

        private:
            std::string m_host;
            static const std::unordered_map<std::string, std::string> s_mapHost;
        };

        /**
         * \brief Authority of the Uri (user info + host + port)
         */
        struct Authority {
            std::optional<std::string> userInfo; //!< user info part (optinal)
            Host host;                           //!< host part
            std::optional<unsigned short> port;  //!< port part (optional)
        };

        Uri(const std::string &_scheme                  = {},
            const std::optional<Authority> &_authority  = {},
            const std::optional<std::string> &_path     = {},
            const std::optional<std::string> &_query    = {},
            const std::optional<std::string> &_fragment = {}); //!< ctor

        std::string scheme;                  //!< scheme of the uri
        std::optional<Authority> authority;  //!< authority of the uri (optional)
        std::optional<std::string> path;     //!< path of the uri (optional)
        std::optional<std::string> query;    //!< query of the uri (optional)
        std::optional<std::string> fragment; //!< fragment of the uri (optional)

        bool operator==(const Uri &other) const noexcept; //!< comparison equal operator
        bool operator!=(const Uri &other) const noexcept; //!< comparison not equal operator

        /**
         * \brief indicate if the url is valid
         * \remark checks folllow https://datatracker.ietf.org/doc/html/rfc3986
         */
        bool isValid() const;

        static bool isValid(const std::string &strUrl); //!< indicate if the string strUrl is a valid uri

        static const Uri &null() noexcept;                                          //!< return the null uri
        static const std::string &schemeFile() noexcept;                            //!< return the predefined scheme 'file'
        static const std::string &schemeWebsocket() noexcept;                       //!< return the predefined scheme 'ws'
        static const std::string &schemeHyperTextTransferProtocol() noexcept;       //!< return the predefined scheme 'http'
        static const std::string &schemeHyperTextTransferProtocolSecure() noexcept; //!< return the predefined scheme 'https'
        static const std::string &schemeFileTransferProtocol() noexcept;            //!< return the predefined scheme 'ftp'

    private:
        bool m_bNull = false;
    };

    /**
     * \brief Comparison between a string and a host
     * \param lhs string value of the host
     * \param rhs host value
     * \return true if equal
     */
    bool operator==(const std::string &lhs, const Uri::Host &rhs);
} // namespace NS_OSBASE::data

/**
 * \brief Serialization of the Uri
 */
template <>
struct NS_OSBASE::core::KeySerializer<std::string, NS_OSBASE::data::Uri, false> {
    static data::Uri getValue(KeyStream<std::string> &keyStream, const data::Uri &defaultValue); //!< \private
    static bool setValue(KeyStream<std::string> &keyStream, const data::Uri &value);             //!< \private
};

/**
 * \brief Convert an Uri to a string
 */
template <>
struct type_converter<std::string, NS_OSBASE::data::Uri> {
    static std::string convert(const NS_OSBASE::data::Uri &uri); //!< \private
};

/**
 * \brief Convert a string to an uri
 * \throws BadUriException if strUri is not well formed
 */
template <>
struct type_converter<NS_OSBASE::data::Uri, std::string> {
    static NS_OSBASE::data::Uri convert(const std::string &strUri); //!< \private */
};

/**
 * \brief Convert an Uri to a wstring
 */
template <>
struct type_converter<std::wstring, NS_OSBASE::data::Uri> {
    static std::wstring convert(const NS_OSBASE::data::Uri &uri); //!< \private
};

/**
 * \brief Convert a wstring to an uri
 * \throws BadUriException if strUri is not well formed
 */
template <>
struct type_converter<NS_OSBASE::data::Uri, std::wstring> {
    static NS_OSBASE::data::Uri convert(const std::wstring &wStrUri); //!< \private */
};

/**
 * \brief Convert an Uri to a path
 * \throws BadUriException if the scheme of the Uri is not "file" or no path is assigned
 */
template <>
struct type_converter<std::filesystem::path, NS_OSBASE::data::Uri> {
    static std::filesystem::path convert(const NS_OSBASE::data::Uri uri); //!< \private */
};

/**
 * \brief Convert a path to an uri
 */
template <>
struct type_converter<NS_OSBASE::data::Uri, std::filesystem::path> {
    static NS_OSBASE::data::Uri convert(const std::filesystem::path &path); //!< \private */
};

/**
 * \brief Specialization for uri
 */
template <>
struct std::hash<NS_OSBASE::data::Uri> {
    size_t operator()(const NS_OSBASE::data::Uri &uri) const; //!< \private
};

/**
 * \brief Specialization for uri
 */
template <>
struct std::less<NS_OSBASE::data::Uri> {
    bool operator()(const NS_OSBASE::data::Uri &lhs, const NS_OSBASE::data::Uri &rhs) const; //!< \private
};

/**
 * \brief Stream operator for uris
 */
std::ostream &operator<<(std::ostream &os, const NS_OSBASE::data::Uri &uri);
