// \brief Define simple command parser class

#include "osApplication/ServiceOptions.h"
#include "osData/AsyncData.h"
#include "getopt.h"
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std::chrono_literals;

namespace NS_OSBASE::application {
    namespace {
        constexpr char SERVICE_OPTION_URL[]    = "url";
        constexpr char SERVICE_OPTION_STRING[] = "string";

    } // namespace

    ServiceOptions::ServiceOptions() {
        m_data.create();
        m_data.get([this](std::string &&data) { onData(std::move(data)); });
    }

    ServiceOptions::~ServiceOptions() {
        reset();
    }

    ServiceOptions::ServiceOptions(const std::string &options) : m_content(options) {
    }

    ServiceOptions::ServiceOptions(const data::Uri &options) : m_data(data::AsyncData<std::string>(options)) {
        m_data.get([this](std::string &&data) { onData(std::move(data)); });
    }

    ServiceOptions::ServiceOptions(int argc, char **argv) {
        auto const guard = core::make_scope_exit([this] {
            if (!m_data.isValid()) {
                std::cout << "options (from " << SERVICE_OPTION_URL << ")" << std::endl;
            } else if (!m_content.empty()) {
                std::cout << "options (from " << SERVICE_OPTION_STRING << ")" << std::endl;
            } else {
                std::cout << "no options" << std::endl;
            }
        });

        constexpr char shortOptions[]    = "u:s:";
        constexpr option_a longOptions[] = { { SERVICE_OPTION_URL, required_argument, nullptr, 'u' },
            { SERVICE_OPTION_STRING, required_argument, nullptr, 's' },
            { nullptr, 0, nullptr, 0 } };

        while (true) {
            auto const opt = getopt_long_a(argc, argv, shortOptions, longOptions, nullptr);

            if (opt == -1) {
                break;
            }

            switch (opt) {
            case 'u': {
                auto const inputUri = type_cast<data::Uri>(std::string(optarg_a));
                if (inputUri.isValid()) {
                    m_data = data::AsyncData<std::string>(inputUri);
                    m_data.get([this](std::string &&data) { onData(std::move(data)); });
                    return;
                }
            } break;
            case 's':
                m_content = optarg_a;
                return;
            case '?':
            default:
                std::cout << "unrecognized option" << std::endl;
                std::cout << "-u, --" << SERVICE_OPTION_URL << " <url> url of the string stream settings"
                          << "-s, --" << SERVICE_OPTION_STRING << " <string> string containing the settings" << std::endl;
                break;
            }
        }
    }

    std::string ServiceOptions::makeCommandLine(const bool bLong) const {
        std::stringstream cmdLine;

        if (!m_data.isValid()) {
            if (bLong) {
                cmdLine << "--" << SERVICE_OPTION_STRING;
            } else {
                cmdLine << "-s";
            }

            cmdLine << " "
                    << "\"" << m_content << "\"";
        } else {
            if (bLong) {
                cmdLine << "--" << SERVICE_OPTION_URL;
            } else {
                cmdLine << "-u";
            }

            cmdLine << " " << type_cast<std::string>(m_data.getUriOfCreator());
        }

        return cmdLine.str();
    }

    void ServiceOptions::reset() {
        if (m_data.isValid()) {
            m_data.reset();
        }
    }

    void ServiceOptions::setDelegate(IDelegatePtr pDelegate) {
        m_pDelegate = pDelegate;
    }

    std::string ServiceOptions::getContent() const {
        return m_content;
    }

    void ServiceOptions::setContent(const std::string &content) {
        m_content = content;
        m_data.set(m_content);
    }

    void ServiceOptions::onData(std::string &&data) {
        {
            std::lock_guard lock(m_mutContent);
            m_content          = std::move(data);
            m_bContentReceived = true;
            m_cvContent.notify_one();
        }

        if (auto const pDelegate = m_pDelegate.lock(); pDelegate != nullptr) {
            pDelegate->onOptionsReceived();
        }
    }

} // namespace NS_OSBASE::application
