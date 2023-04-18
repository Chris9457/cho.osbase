// \brief Declaration of the log output realization for data exchanges

#include "LogOutputDataExchange.h"
#include "osData/FactoryNames.h"
#include "osCore/DesignPattern/AbstractFactory.h"
#include "osData/IDataExchange.h"
#include <future>

using namespace std::chrono_literals;

namespace cho::osbase::data::impl {
    OS_REGISTER_FACTORY_N(ILogOutput, LogOutputDataExchange, 0, LOGOUTPUT_DATAEXCHANGE_FACTORY_NAME, Uri)
    OS_REGISTER_FACTORY_N(ILogOutput, LogOutputDataExchange, 1, LOGOUTPUT_DATAEXCHANGE_FACTORY_NAME, IDataExchangePtr)

    /*
     * \class LogOutputDataExchange::DataEchangeDelegate
     */
    class LogOutputDataExchange::DataEchangeDelegate : public IDataExchange::IDelegate {
    public:
        DataEchangeDelegate(LogOutputDataExchange &logOutputDataExchange) : m_logOutputDataExchange(logOutputDataExchange) {
        }

        void onConnected(const bool connected) override {
            m_logOutputDataExchange.onConnected(connected);
        }

        void onFailure(std::string &&) override {
        }

        void onDataReceived(ByteBuffer &&) override {
        }

    private:
        LogOutputDataExchange &m_logOutputDataExchange;
    };

    LogOutputDataExchange::LogOutputDataExchange(IDataExchangePtr pDataExchange)
        : m_pDataExchange(pDataExchange), m_pDataExchangeDelegate(std::make_shared<DataEchangeDelegate>(*this)) {
        if (m_pDataExchange != nullptr) {
            m_pDataExchange->setDelegate(m_pDataExchangeDelegate);
        }
    }

    /*
     * \class LogOutputDataExchange
     */
    LogOutputDataExchange::LogOutputDataExchange(const Uri &uri)
        : m_pDataExchange(makeDataExchange(uri.scheme)), m_pDataExchangeDelegate(std::make_shared<DataEchangeDelegate>(*this)) {
        if (m_pDataExchange != nullptr) {
            m_bOpened = true;
            m_pDataExchange->setDelegate(m_pDataExchangeDelegate);
            m_pDataExchange->open(uri);
        }
    }

    LogOutputDataExchange::~LogOutputDataExchange() {
        if (m_pDataExchange != nullptr && m_bOpened && m_bConnected) {
            m_pDataExchange->close();
        }
    }

    void LogOutputDataExchange::log(std::string &&msg) {
        std::lock_guard lock(m_mutex);
        if (m_bConnected) {
            try {
                m_pDataExchange->push(type_cast<ByteBuffer>(std::forward<std::string>(msg)));
            } catch (const DataExchangeException &) {
                m_bConnected = false;
                m_bufferOutOfConnectionLogs.push_back(std::forward<std::string>(msg));
            }
        } else {
            m_bufferOutOfConnectionLogs.push_back(std::forward<std::string>(msg));
        }
    }

    void LogOutputDataExchange::flush(std::ostream &os) {
        for (auto const &msg : m_bufferOutOfConnectionLogs) {
            os << msg << std::endl;
        }
    }

    void LogOutputDataExchange::onConnected(const bool bConnected) {
        std::lock_guard lock(m_mutex);
        m_bConnected = bConnected;
        if (m_bConnected) {
            for (auto &&outOfConnectionLog : m_bufferOutOfConnectionLogs) {
                log(std::string(outOfConnectionLog));
            }
            m_bufferOutOfConnectionLogs.clear();
        }
    }

} // namespace cho::osbase::data::impl
