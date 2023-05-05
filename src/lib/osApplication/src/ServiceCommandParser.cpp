// \brief Define simple command parser class

#include "osApplication/ServiceCommandParser.h"
#include "getopt.h"
#include <sstream>
#include <iostream>

namespace NS_OSBASE::application {

    constexpr char SERVICE_OPTION_BROKERURL[] = "brokerUrl";
    constexpr char SERVICE_OPTION_REALM[]     = "realm";
    constexpr char SERVICE_OPTION_DATAURL[]   = "dataUrl";

    ServiceCommandParser::ServiceCommandParser(const ServiceOptions &options) : m_options(options) {
    }

    ServiceCommandParser::ServiceCommandParser(int argc, char **argv) {
        constexpr char shortOptions[]    = "b:r:d:";
        constexpr option_a longOptions[] = { { SERVICE_OPTION_BROKERURL, required_argument, nullptr, 'b' },
            { SERVICE_OPTION_REALM, required_argument, nullptr, 'r' },
            { SERVICE_OPTION_DATAURL, required_argument, nullptr, 'd' },
            { nullptr, 0, nullptr, 0 } };

        while (true) {
            auto const opt = getopt_long_a(argc, argv, shortOptions, longOptions, nullptr);

            if (opt == -1) {
                break;
            }

            data::Uri uri;
            switch (opt) {
            case 'b':
            case 'd':
                uri = type_cast<data::Uri>(std::string(optarg_a));
                if (!uri.isValid()) {
                    std::cout << "invalid uri: " << optarg_a;
                    break;
                }

                if (opt == 'b') {
                    m_options.brokerUrl = uri;
                } else {
                    m_options.dataUrl = uri;
                }
                break;
            case 'r':
                m_options.realm = optarg_a;
                break;
            case '?':
            default:
                std::cout << "unrecognized option" << std::endl;
                std::cout << "-b, --" << SERVICE_OPTION_BROKERURL << " <url> url of the broker"
                          << "-r, --" << SERVICE_OPTION_REALM << " <realm> realm to use"
                          << "-d, --" << SERVICE_OPTION_DATAURL << "<url> url of the data" << std::endl;
                break;
            }
        }
    }

    std::string ServiceCommandParser::makeCommandLine(const bool bLong) const {
        std::stringstream cmdLine;

        if (m_options.brokerUrl.has_value()) {
            if (bLong) {
                cmdLine << "--" << SERVICE_OPTION_BROKERURL;
            } else {
                cmdLine << "-b";
            }
            cmdLine << " " << type_cast<std::string>(m_options.brokerUrl.value());
            if (m_options.dataUrl.has_value()) {
                cmdLine << " ";
            }
        }

        if (m_options.dataUrl.has_value()) {
            if (bLong) {
                cmdLine << "--" << SERVICE_OPTION_DATAURL;
            } else {
                cmdLine << "-d";
            }
            cmdLine << " " << type_cast<std::string>(m_options.dataUrl.value());
        }

        return cmdLine.str();
    }

    const ServiceOptions &ServiceCommandParser::getOptions() const {
        return m_options;
    }

} // namespace NS_OSBASE::application
