// \brief Base interface of services

#include "osApplication/IService.h"
namespace NS_OSBASE::application {
    /*
     * \struct ServiceConnectionMsg
     */
    ServiceConnectionMsg::ServiceConnectionMsg(const bool bConnected) : m_bConnected(bConnected) {
    }

    bool ServiceConnectionMsg::isConnected() const {
        return m_bConnected;
    }
} // namespace NS_OSBASE::application
