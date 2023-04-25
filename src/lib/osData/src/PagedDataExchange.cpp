// \brief Declaration of the class PagedDataExchange

#include "osData/PagedDataExchange.h"

namespace NS_OSBASE::data {

    /*
     * \class PagedDataExchange::DataExchangeDelegate
     */
    class PagedDataExchange::DataExchangeDelegate : public IDataExchange::IDelegate {
    public:
        DataExchangeDelegate(IDataExchange::IDelegatePtr pDelegate) : m_pDelegate(pDelegate) {
        }

        void onConnected(const bool connected) override {
            if (const auto pDelegate = m_pDelegate.lock(); pDelegate != nullptr) {
                pDelegate->onConnected(connected);
            }
        }

        void onFailure(std::string &&failure) override {
            if (const auto pDelegate = m_pDelegate.lock(); pDelegate != nullptr) {
                pDelegate->onFailure(std::move(failure));
            }
        }

        void onDataReceived(ByteBuffer &&buffer) override {
            if (buffer.size() < s_headerSize) {
                throw DataExchangeException("DataExchangeDecorator::DataExchangeDelegate: received buffer too small!");
            }

            auto const page    = *reinterpret_cast<pagecount_type *>(buffer.data());
            auto const nbPages = *reinterpret_cast<pagecount_type *>(buffer.data() + sizeof(pagecount_type));

            // Check the expected nbPages
            if (m_currentNbPages == 0) {
                m_buffer.clear();
                m_currentNbPages = nbPages;
            } else if (m_currentNbPages != nbPages) {
                throw DataExchangeException("DataExchangeDecorator::DataExchangeDelegate: unexpected nb pages!");
            }

            // Check the current page
            if (m_currentPage + 1 != page) {
                throw DataExchangeException("DataExchangeDecorator::DataExchangeDelegate: unexpected current page!");
            }
            ++m_currentPage;

            m_buffer.insert(m_buffer.cend(), buffer.cbegin() + s_headerSize, buffer.cend());
            if (page == nbPages) {
                if (const auto pDelegate = m_pDelegate.lock(); pDelegate != nullptr) {
                    pDelegate->onDataReceived(std::move(m_buffer));
                }
            }

            if (m_currentPage == m_currentNbPages) {
                m_currentNbPages = 0;
                m_currentPage    = 0;
            }
        }

    private:
        IDataExchange::IDelegateWPtr m_pDelegate;
        ByteBuffer m_buffer;
        pagecount_type m_currentPage    = 0;
        pagecount_type m_currentNbPages = 0;
    };

    /*
     * \class PagedDataExchange
     */
    void PagedDataExchange::push(const ByteBuffer &buffer) const {
        if (m_pageSize <= s_headerSize) {
            throw DataExchangeException("makePagedDataExchange: maxSizeInBytes must be greater than " +
                                        std::to_string(PagedDataExchange::s_headerSize) + " (currently " + std::to_string(m_pageSize) +
                                        ")");
        }

        std::lock_guard lock(m_mutexPush);

        auto const payloadSize = m_pageSize - s_headerSize;
        auto const nbPages     = static_cast<pagecount_type>(buffer.size() / payloadSize + (buffer.size() % payloadSize == 0 ? 0 : 1));

        for (pagecount_type page = 1; page <= nbPages; ++page) {
            ByteBuffer pagedBuffer(s_headerSize);

            *reinterpret_cast<pagecount_type *>(pagedBuffer.data())                          = page;
            *reinterpret_cast<pagecount_type *>(pagedBuffer.data() + sizeof(pagecount_type)) = nbPages;
            if (page < nbPages) {
                pagedBuffer.insert(pagedBuffer.cend(), buffer.cbegin() + (page - 1) * payloadSize, buffer.cbegin() + page * payloadSize);
            } else {
                pagedBuffer.insert(pagedBuffer.cend(), buffer.cbegin() + (page - 1) * payloadSize, buffer.cend());
            }

            DataExchangeDecorator::push(pagedBuffer);
        }
    }

    void PagedDataExchange::setDelegate(IDelegatePtr pDelegate) noexcept {
        m_pDelegate = std::make_shared<DataExchangeDelegate>(pDelegate);
        DataExchangeDecorator::setDelegate(m_pDelegate);
    }

    size_t PagedDataExchange::getPageSize() const {
        return m_pageSize;
    }

    PagedDataExchange::PagedDataExchange(IDataExchangePtr pDataEchange, const size_t maxSizeInBytes)
        : DataExchangeDecorator(pDataEchange), m_pageSize(maxSizeInBytes) {
    }

    /*
     * maker
     */
    PagedDataExchangePtr makePagedDataExchange(IDataExchangePtr pDataExchange, const size_t pageSize) {
        return PagedDataExchangePtr(new PagedDataExchange(pDataExchange, pageSize));
    }

    PagedDataExchangePtr makePagedDataExchange(const size_t maxSizeInBytes, const std::string &scheme) {
        auto const pDataExchange = makeDataExchange(scheme);
        return makePagedDataExchange(pDataExchange, maxSizeInBytes);
    }

    PagedDataExchangePtr makePagedDataExchange(const std::string &scheme) {
        auto const pDataExchange = makeDataExchange(scheme);
        return makePagedDataExchange(pDataExchange, PagedDataExchange::s_websocketPageSize);
    }

} // namespace NS_OSBASE::data
