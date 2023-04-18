// \brief Declaration of the class Network

#include "Network.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include <WinSock2.h>

namespace cho::osbase::data::impl {

    namespace {
        std::vector<std::string> getIPv4Address() {
            std::vector<std::string> addresses;
            auto const guard = core::make_scope_exit([] { WSACleanup(); });

            WSADATA WSAData;

            // Initialize winsock dll
            if (::WSAStartup(MAKEWORD(1, 0), &WSAData)) {
                // Error handling
            }

            // Get local host name
            char szHostName[128] = "";

            if (::gethostname(szHostName, sizeof(szHostName))) {
                // Error handling -> call 'WSAGetLastError()'
            }

            // Get local IP addresses
            sockaddr_in SocketAddress{};
            const hostent *pHost = nullptr;

            pHost = ::gethostbyname(szHostName);
            if (!pHost) {
                // Error handling -> call 'WSAGetLastError()'
            }

            for (int iCnt = 0; ((pHost->h_addr_list[iCnt]) && (iCnt < 10)); ++iCnt) {
                memcpy(&SocketAddress.sin_addr, pHost->h_addr_list[iCnt], pHost->h_length);
                addresses.emplace_back(inet_ntoa(SocketAddress.sin_addr));
            }

            return addresses;
        }

    } // namespace

    OS_REGISTER_FACTORY_N(INetwork, Network, 0, NETWORK_FACTORY_NAME)

    /*
     * \class Network
     */
    Uri::Host Network::getLocalHost() {
        return getIPv4Address()[0];
    }
} // namespace cho::osbase::data::impl
