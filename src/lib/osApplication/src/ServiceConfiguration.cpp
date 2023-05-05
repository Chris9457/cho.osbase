// \brief Declaration of the class ServiceConfiguration

#include "osApplication/ServiceConfiguration.h"

namespace NS_OSBASE::application {

    namespace {
        data::Uri default_broker_uri{ std::string{ "ws://127.0.0.1:8080" } };
        std::string default_realm{ "service_osbase" };
    } // namespace

    /*
     * \class ServiceConfigration
     */
    const data::Uri &ServiceConfiguration::getBrokerUri() const {
        return m_brokerUri.isValid() ? m_brokerUri : default_broker_uri;
    }

    void ServiceConfiguration::setBrokerUri(const data::Uri &uri) {
        m_brokerUri = uri;
    }

    const std::string &ServiceConfiguration::getRealm() const {
        return m_realm.empty() ? default_realm : m_realm;
    }

    void ServiceConfiguration::setRealm(const std::string &realm) {
        m_realm = realm;
    }
} // namespace NS_OSBASE::application
