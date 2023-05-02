// \brief A bit of implementation to have the linker not completely skip this software unit

#include "osData/IMessaging.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace nscore = NS_OSBASE::core;

namespace NS_OSBASE::data {

    /*
     * \class IMessaging::MessagingConnectionMsg
     */
    IMessaging::MessagingConnectionMsg::MessagingConnectionMsg(const bool bConnected) : m_bConnected(bConnected) {
    }

    bool IMessaging::MessagingConnectionMsg::isConnected() const {
        return m_bConnected;
    }

    /*
     * \class IMessaging
     */
    IMessaging::~IMessaging() = default;

    IMessaging::IClientDelegate::~IClientDelegate()     = default;
    IMessaging::ISupplierDelegate::~ISupplierDelegate() = default;
    IMessaging::IEventDelegate::~IEventDelegate()       = default;
    IMessaging::IErrorDelegate::~IErrorDelegate()       = default;

    IMessagingPtr makeWampMessaging(const Uri &uri, const std::string &realm) {
        return nscore::TheFactoryManager.createInstance<IMessaging>(MESSAGINGWAMPCC_FACTORY_NAME, uri, realm);
    }

} // namespace NS_OSBASE::data
