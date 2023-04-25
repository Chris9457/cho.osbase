// \brief Define simple command parser class

#pragma once
#include "osCore/Misc/TypeCast.h"
#include <optional>
#include <string>
#include <vector>

/**
 * \addtogroup PACKAGE_SERVICE
 * \{
 */

namespace NS_OSBASE::application {

    /**
     * \brief Options used to initialize a service
     */
    struct ServiceOptions {
        std::optional<std::string> brokerUrl;        //!< url of the broker
        std::optional<unsigned short> brokerPort;    //!< port oft the broker
        std::optional<std::string> handshakeScheme;  //!< scheme of the handshake url
        std::optional<std::string> handshakeHost;    //!< host of the handshake url
        std::optional<unsigned short> handshakePort; //!< port of the handshake url
    };

    /**
     * \brief Command parser for the service option
     */
    class ServiceCommandParser {
    public:
        ServiceCommandParser(const ServiceOptions &options); //!< ctor used to build a command line
        ServiceCommandParser(int argc, char **argv);         //!< ctor used to build a ServiceOptions struct

        ServiceOptions parse() const;        //!< build a ServiceOptions struct from a command line
        std::string makeCommandLine() const; //!< buils a command line from a ServiceOptions struct

    private:
        std::string getCommandOption(const std::string &option) const;
        bool optionExist(const std::string &option) const;

        std::vector<std::string> m_tokens;
    };

} // namespace NS_OSBASE::application

/**
 * \brief Conversion ServiceCommandParser ==> ServiceOptions
 */
template <>
struct type_converter<NS_OSBASE::application::ServiceOptions, NS_OSBASE::application::ServiceCommandParser> {
    static NS_OSBASE::application::ServiceOptions convert(
        const NS_OSBASE::application::ServiceCommandParser &cmdParser); //!< \private
};

/**
 * \brief Conversion ServiceOptions ==> string
 */
template <>
struct type_converter<std::string, NS_OSBASE::application::ServiceOptions> {
    static std::string convert(const NS_OSBASE::application::ServiceOptions &options); //!< \private
};
/** \} */
