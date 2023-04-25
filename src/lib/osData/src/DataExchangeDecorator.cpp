//\ brief Daclaration of the base class of all DataExchange decorators

#include "osData/DataExchangeDecorator.h"

namespace NS_OSBASE::data {

    /*
     * \class DataExchangeDecorator
     */
    Uri DataExchangeDecorator::getUriOfCreator() const {
        throwIfNull();
        return m_pDataExchange->getUriOfCreator();
    }

    void DataExchangeDecorator::open(const Uri &uri) {
        throwIfNull();
        m_pDataExchange->open(uri);
    }

    void DataExchangeDecorator::close() {
        throwIfNull();
        m_pDataExchange->close();
    }

    void DataExchangeDecorator::create() {
        throwIfNull();
        m_pDataExchange->create();
    }

    void DataExchangeDecorator::destroy() {
        throwIfNull();
        m_pDataExchange->destroy();
    }

    IExchange::AccessType DataExchangeDecorator::getAccessType() const noexcept {
        throwIfNull();
        return m_pDataExchange->getAccessType();
    }

    bool DataExchangeDecorator::isWired() const noexcept {
        throwIfNull();
        return m_pDataExchange->isWired();
    }

    void DataExchangeDecorator::push(const ByteBuffer &buffer) const {
        throwIfNull();
        m_pDataExchange->push(buffer);
    }

    void DataExchangeDecorator::setDelegate(IDelegatePtr pDelegate) noexcept {
        throwIfNull();
        m_pDataExchange->setDelegate(pDelegate);
    }

    DataExchangeDecorator::DataExchangeDecorator(IDataExchangePtr pDataExchange) : m_pDataExchange(pDataExchange) {
    }

    void DataExchangeDecorator::throwIfNull() const {
        if (m_pDataExchange == nullptr) {
            throw DataExchangeException("DataExchangeDecorator: decorated instance null!");
        }
    }
} // namespace NS_OSBASE::data
