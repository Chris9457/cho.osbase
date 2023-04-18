// \brief Declaration of the log output realization for data exchanges

#include "osData/ILogOutput.h"
#include "osData/IDataExchange.h"
#include <mutex>

namespace cho::osbase::data::impl {

    /**
     * \brief Realization of the interface ILogOutput
     */
    class LogOutputDataExchange final : public ILogOutput {
    public:
        LogOutputDataExchange(IDataExchangePtr pDataExchange);
        LogOutputDataExchange(const Uri &uri);
        ~LogOutputDataExchange() override;

        void log(std::string &&msg) override;
        void flush(std::ostream &os) override;

    private:
        class DataEchangeDelegate;
        using DataEchangeDelegatePtr = std::shared_ptr<DataEchangeDelegate>;

        void onConnected(const bool bConnected);

        IDataExchangePtr m_pDataExchange;
        DataEchangeDelegatePtr m_pDataExchangeDelegate;
        std::atomic_bool m_bConnected = false;
        std::vector<std::string> m_bufferOutOfConnectionLogs;
        std::recursive_mutex m_mutex;
        bool m_bOpened = false;
    };
} // namespace cho::osbase::data::impl
