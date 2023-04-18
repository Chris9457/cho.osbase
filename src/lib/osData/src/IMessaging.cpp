// \brief A bit of implementation to have the linker not completely skip this software unit

#include "osData/IMessaging.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"

namespace nscore = cho::osbase::core;

namespace cho::osbase::data {

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

    IMessagingPtr makeMessaging() {
        return nscore::TheFactoryManager.createInstance<IMessaging>(MESSAGINGWAMPCC_FACTORY_NAME);
    }

} // namespace cho::osbase::data
