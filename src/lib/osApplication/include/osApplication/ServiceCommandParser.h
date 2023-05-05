// \brief Define simple command parser class

#pragma once
#include "osData/Uri.h"
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
        std::optional<data::Uri> brokerUrl; //!< url of the broker
        std::optional<std::string> realm;
        std::optional<data::Uri> dataUrl; //!< url of the data
    };

    /**
     * \brief Command parser for the service option
     */
    class ServiceCommandParser {
    public:
        ServiceCommandParser(const ServiceOptions &options); //!< ctor used to build a command line
        ServiceCommandParser(int argc, char **argv);         //!< ctor used to build a ServiceOptions struct

        std::string makeCommandLine(const bool bLong = false) const; //!< buils a command line from a ServiceOptions struct
        const ServiceOptions &getOptions() const;

    private:
        ServiceOptions m_options;
    };

} // namespace NS_OSBASE::application
