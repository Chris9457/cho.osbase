// \brief Declaration of the class ServiceConfiguration

#pragma once
#include "osCore/DesignPattern/Singleton.h"
#include "osData/Uri.h"

namespace NS_OSBASE::application {

    /**
     * \brief Configuration of the service
     */
    class ServiceConfiguration : public core::Singleton<ServiceConfiguration> {
        friend Singleton<ServiceConfiguration>;

    public:
        const data::Uri &getBrokerUri() const;
        void setBrokerUri(const data::Uri &uri);

        const std::string &getRealm() const;
        void setRealm(const std::string &realm);

    private:
        ServiceConfiguration()           = default;
        ~ServiceConfiguration() override = default;

        data::Uri m_brokerUri;
        std::string m_realm;
    };

#define TheServiceConfiguration ServiceConfiguration::getInstance()
} // namespace NS_OSBASE::application
