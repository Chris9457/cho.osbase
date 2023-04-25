// \brief Declaration of the class PagedDataExchange

#pragma once
#include "DataExchangeDecorator.h"
#include <mutex>

namespace NS_OSBASE::data {

    /**
     * \addtogroup PACKAGE_OSBASE_EXCHANGE
     * \{
     */

    class PagedDataExchange;
    using PagedDataExchangePtr = std::shared_ptr<PagedDataExchange>; //!< alias for shared pointer on PagedDataExchange

    /**
     * \brief This class decorates an instance of IDataExchange in order to manage a "paged" buffer
     *
     * In case of "very" large buffer, the instance of data exchange may fail to push the data.
     * This class intends to split the buffer in size scoped buffer enough small to be pushed
     */
    class PagedDataExchange : public DataExchangeDecorator {
        friend PagedDataExchangePtr makePagedDataExchange(IDataExchangePtr pDataExchange, const size_t pageSize);

    public:
        using pagecount_type = unsigned short; //!< alias for the type of page ref

        void push(const ByteBuffer &buffer) const override; //!< throws DataExchangeException if maxSizeInBytes <= headerSize
        void setDelegate(IDelegatePtr pDelegate) noexcept override;

        size_t getPageSize() const; //!< return the page size

        static auto constexpr s_headerSize        = 2 * sizeof(pagecount_type); //!< header size of the message
        static auto constexpr s_websocketPageSize = 32000000;                   //!< default page size for websockets

    private:
        class DataExchangeDelegate;

        PagedDataExchange(IDataExchangePtr pData, const size_t maxSizeInBytes);

        IDataExchange::IDelegatePtr m_pDelegate;
        size_t m_pageSize = s_websocketPageSize;
        mutable std::mutex m_mutexPush;
    };

    PagedDataExchangePtr makePagedDataExchange(IDataExchangePtr pDataExchange, const size_t pageSize); //!< create a paged data exchange
                                                                                                       //!< with a data exchange
    PagedDataExchangePtr makePagedDataExchange(
        const size_t maxSizeInBytes, const std::string &scheme = IDataExchange::defaultScheme); //!< create a paged data exchange with a max
                                                                                                //!< size
    PagedDataExchangePtr makePagedDataExchange(const std::string &scheme = IDataExchange::defaultScheme); //!< create a paged data exchange
                                                                                                          //!< with a scheme

    /** \} */
} // namespace NS_OSBASE::data
