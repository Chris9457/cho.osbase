// \brief This class implement a handshake when lauching a service

#include "osApplication/ServiceHandshake.h"
#include "osData/IDataExchange.h"
#include <random>

namespace cho::osbase::application {

    namespace {
        constexpr size_t s_uuidLength = 64;

        unsigned randomChar() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 255);
            return dis(gen);
        }

        data::ByteBuffer generateUUID(const size_t len) {
            data::ByteBuffer buffer;
            for (size_t i = 0; i < len; i++) {
                buffer.push_back(std::byte{ static_cast<unsigned char>(randomChar()) });
            }
            return buffer;
        }

    } // namespace

    /*
     * \class ServiceHandshake::ExchangeDelegate
     */
    class ServiceHandshake::ExchangeDelegate : public data::IDataExchange::IDelegate {
    public:
        ExchangeDelegate(ServiceHandshake &serviceHandshake) : m_serviceHandshake(serviceHandshake) {
        }

        void onDataReceived(data::ByteBuffer &&buffer) override {
            m_serviceHandshake.onDataReceived(std::forward<data::ByteBuffer>(buffer));
        }

        void onConnected(const bool connected) override {
            m_serviceHandshake.onConnected(connected);
        }

        void onFailure(std::string &&failure) override {
            m_serviceHandshake.onFailure(std::forward<std::string>(failure));
        }

    private:
        ServiceHandshake &m_serviceHandshake;
    };

    /*
     * \class ServiceHandshake
     */
    ServiceHandshakePtr ServiceHandshake::create() {
        return ServiceHandshakePtr(new ServiceHandshake());
    }

    ServiceHandshakePtr ServiceHandshake::create(const data::Uri &uri) {
        return ServiceHandshakePtr(new ServiceHandshake(uri));
    }

    ServiceHandshake::~ServiceHandshake() {
        if (m_isCreator) {
            m_pDataExchange->destroy();
        }
    }

    data::Uri ServiceHandshake::getUri() const {
        return m_pDataExchange->getUriOfCreator();
    }

    ServiceHandshake::ServiceHandshake() : ServiceHandshake(true) {
        m_pDataExchange->create();
    }

    ServiceHandshake::ServiceHandshake(const data::Uri &uri) : ServiceHandshake(false) {
        m_uri  = uri;
        m_uuid = generateUUID(s_uuidLength);
    }

    ServiceHandshake::ServiceHandshake(const bool bCreate)
        : m_isCreator(bCreate), m_pDataExchange(data::makeDataExchange()), m_pExchangeDelegate(std::make_shared<ExchangeDelegate>(*this)) {
        m_pDataExchange->setDelegate(m_pExchangeDelegate);
    }

    bool ServiceHandshake::engage(const std::chrono::milliseconds &timeout) {
        if (m_bEngaged) {
            return true;
        }

        if (m_isCreator) {
            // Creator side: wait for the uuid
            std::unique_lock lock(m_mutex);
            if (m_cvHandshake.wait_for(lock, timeout) == std::cv_status::no_timeout) {
                m_bEngaged = true;
                return true;
            }

            return false;
        }

        // Client side: open the connection and push the uuid
        std::unique_lock lock(m_mutex);
        m_pDataExchange->open(m_uri);

        if (m_cvConnected.wait_for(lock, timeout) == std::cv_status::no_timeout) {
            m_pDataExchange->push(m_uuid);
            m_bEngaged = true;
            return true;
        }

        return false;
    }

    bool ServiceHandshake::disengage(const std::chrono::milliseconds &timeout) {
        if (!m_bEngaged) {
            return true;
        }

        if (m_isCreator) {
            // Creator side: send the uuid, wait for the remote deconnection and destroy the channel
            m_pDataExchange->push(m_uuid);

            std::unique_lock lock(m_mutex);
            if (m_cvDisconnected.wait_for(lock, timeout) == std::cv_status::no_timeout) {
                m_bEngaged = false;
                return true;
            }

            return false;
        }

        // client side: wait for the uuid and close the connection
        std::unique_lock lock(m_mutex);
        if (m_cvHandshake.wait_for(lock, timeout) == std::cv_status::no_timeout && m_bEngaged) {
            m_pDataExchange->close();
            m_bEngaged = false;
            return true;
        }

        return false;
    }

    void ServiceHandshake::setDelegate(IDelegatePtr pDelegate) {
        m_pDelegate = pDelegate;
    }

    void ServiceHandshake::onDataReceived(data::ByteBuffer &&buffer) {
        if (m_isCreator && !m_bEngaged) {
            m_uuid = buffer;
            m_cvHandshake.notify_one();

            auto const pDelegate = m_pDelegate.lock();
            if (pDelegate != nullptr) {
                m_bEngaged = true;
                pDelegate->onEngage();
            }
        } else if (m_bEngaged && m_uuid == buffer) {
            m_cvHandshake.notify_one();

            auto const pDelegate = m_pDelegate.lock();
            if (pDelegate != nullptr) {
                pDelegate->onDisengage();
                m_bEngaged = false;
            }
        }
    }

    void ServiceHandshake::onConnected(const bool connected) {
        if (connected) {
            m_cvConnected.notify_one();
        } else {
            m_cvDisconnected.notify_one();
        }
    }

    void ServiceHandshake::onFailure(std::string &&) {
    }
} // namespace cho::osbase::application
