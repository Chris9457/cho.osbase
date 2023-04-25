//\ brief Daclaration of the base class of all DataExchange decorators

#pragma once
#include "IDataExchange.h"

namespace NS_OSBASE::data {

    /**
     * \brief Base class for all DataExchange decorators
     * \throws DataExchangeException if internal instance is null
     * \ingroup PACKAGE_OSBASE_EXCHANGE
     */
    class DataExchangeDecorator : public IDataExchange {
    public:
        Uri getUriOfCreator() const override;
        void open(const Uri &uri) override;
        void close() override;
        void create() override;
        void destroy() override;
        AccessType getAccessType() const noexcept override;
        bool isWired() const noexcept override;
        void push(const ByteBuffer &buffer) const override;
        void setDelegate(IDelegatePtr pDelegate) noexcept override;

    protected:
        explicit DataExchangeDecorator(IDataExchangePtr pDataExchange); //!< \private

    private:
        void throwIfNull() const;

        IDataExchangePtr m_pDataExchange;
    };
} // namespace NS_OSBASE::data
