// \brief Declaration of the class ServiceConfiguration

#include "osApplication/ServiceConfiguration.h"

namespace NS_OSBASE::application {

    /*
     * \class ServiceConfigration
     */
    const data::Uri &ServiceConfiguration::getBrokerUri() const {
        return m_brokerUri;
    }

    void ServiceConfiguration::setBrokerUri(const data::Uri &uri) {
        m_brokerUri = uri;
    }

    const std::string &ServiceConfiguration::getRealm() const {
        return m_realm;
    }

    void ServiceConfiguration::setRealm(const std::string &realm) {
        m_realm = realm;
    }
} // namespace NS_OSBASE::application
