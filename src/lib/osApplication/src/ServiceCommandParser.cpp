// \brief Define simple command parser class

#include "osApplication/ServiceCommandParser.h"
#include <sstream>

namespace cho::osbase::application {

    constexpr char *SERVICE_OPTION_BROKERURL        = "-brokerUrl";
    constexpr char *SERVICE_OPTION_BROKERPORT       = "-brokerPort";
    constexpr char *SERVICE_OPTION_HANDSHAKERSCHEME = "-handshakeScheme";
    constexpr char *SERVICE_OPTION_HANDSHAKERHOST   = "-handshakeHost";
    constexpr char *SERVICE_OPTION_HANDSHAKERPORT   = "-handshakePort";

    ServiceCommandParser::ServiceCommandParser(const ServiceOptions &options) {
        if (options.brokerUrl) {
            m_tokens.push_back(SERVICE_OPTION_BROKERURL);
            m_tokens.push_back(*options.brokerUrl);
        }

        if (options.brokerPort) {
            m_tokens.push_back(SERVICE_OPTION_BROKERPORT);
            m_tokens.push_back(std::to_string(*options.brokerPort));
        }

        if (options.handshakeScheme) {
            m_tokens.push_back(SERVICE_OPTION_HANDSHAKERSCHEME);
            m_tokens.push_back(*options.handshakeScheme);
        }

        if (options.handshakeHost) {
            m_tokens.push_back(SERVICE_OPTION_HANDSHAKERHOST);
            m_tokens.push_back(*options.handshakeHost);
        }

        if (options.handshakePort) {
            m_tokens.push_back(SERVICE_OPTION_HANDSHAKERPORT);
            m_tokens.push_back(std::to_string(*options.handshakePort));
        }
    }

    ServiceCommandParser::ServiceCommandParser(int argc, char **argv) {
        for (int i = 0; i < argc; ++i) {
            m_tokens.push_back(argv[i]);
        }
    }

    ServiceOptions ServiceCommandParser::parse() const {
        ServiceOptions options;

        if (optionExist(SERVICE_OPTION_BROKERURL)) {
            options.brokerUrl = getCommandOption(SERVICE_OPTION_BROKERURL);
        }

        if (optionExist(SERVICE_OPTION_BROKERPORT)) {
            options.brokerPort = static_cast<unsigned short>(std::stoul(getCommandOption(SERVICE_OPTION_BROKERPORT)));
        }

        if (optionExist(SERVICE_OPTION_HANDSHAKERSCHEME)) {
            options.handshakeScheme = getCommandOption(SERVICE_OPTION_HANDSHAKERSCHEME);
        }

        if (optionExist(SERVICE_OPTION_HANDSHAKERHOST)) {
            options.handshakeHost = getCommandOption(SERVICE_OPTION_HANDSHAKERHOST);
        }

        if (optionExist(SERVICE_OPTION_HANDSHAKERPORT)) {
            options.handshakePort = static_cast<unsigned short>(std::stoul(getCommandOption(SERVICE_OPTION_HANDSHAKERPORT)));
        }

        return options;
    }

    std::string ServiceCommandParser::makeCommandLine() const {
        std::string cmdLine;

        for (auto &&opt : { SERVICE_OPTION_BROKERURL,
                 SERVICE_OPTION_BROKERPORT,
                 SERVICE_OPTION_HANDSHAKERSCHEME,
                 SERVICE_OPTION_HANDSHAKERHOST,
                 SERVICE_OPTION_HANDSHAKERPORT }) {

            if (optionExist(opt)) {
                std::ostringstream oss;
                oss << opt << " " << getCommandOption(opt);
                cmdLine += (cmdLine.empty() ? "" : " ") + oss.str();
            }
        }

        return cmdLine;
    }

    std::string ServiceCommandParser::getCommandOption(const std::string &option) const {
        auto it = std::find(m_tokens.cbegin(), m_tokens.cend(), option);
        if (it == m_tokens.cend() || ++it == m_tokens.cend()) {
            return "";
        }

        return *it;
    }

    bool ServiceCommandParser::optionExist(const std::string &option) const {
        auto it = std::find(m_tokens.cbegin(), m_tokens.cend(), option);
        return it != m_tokens.cend() && ++it != m_tokens.cend();
    };

} // namespace cho::osbase::application

using namespace cho::osbase::application;
ServiceOptions type_converter<ServiceOptions, ServiceCommandParser>::convert(const ServiceCommandParser &cmdParser) {
    return cmdParser.parse();
}

std::string type_converter<std::string, ServiceOptions>::convert(const ServiceOptions &options) {
    return ServiceCommandParser(options).makeCommandLine();
}
