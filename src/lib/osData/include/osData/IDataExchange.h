// \brief An abstract interface intended to  define the point to point data exchange between 2 classes

#pragma once
#include "ByteBuffer.h"
#include "IExchange.h"

#include <memory>

namespace NS_OSBASE::data {

    /**
     * \addtogroup PACKAGE_OSBASE_EXCHANGE
     * \{
     */

    /**
     * \brief Exception thrown by DataExchange classes
     */
    class DataExchangeException : public core::LogicException {
        using core::LogicException::LogicException;
    };

    class IDataExchange;
    using IDataExchangePtr = std::shared_ptr<IDataExchange>; //!< alias of shared pointer on IDataExchange

    /**
     * \brief This interface specialize the interface IExchange by adding the "push" method to transfer an array of bytes
     */
    class IDataExchange : public IExchange {

    public:
        /**
         * \brief Add the data received notification
         * \ingroup PACKAGE_OSBASE_EXCHANGE
         */
        class IDelegate : public IExchange::IDelegate {
        public:
            virtual void onDataReceived(ByteBuffer &&buffer) = 0; //!< method called on data received
        };
        using IDelegatePtr  = std::shared_ptr<IDelegate>; //!< alias for shared pointer on IDelegate
        using IDelegateWPtr = std::weak_ptr<IDelegate>;   //!< alias for weak pointer on IDelegate

        virtual void push(const ByteBuffer &buffer) const         = 0; //!< push the byte array "buffer"
        virtual void setDelegate(IDelegatePtr pDelegate) noexcept = 0; //!< assign a delegate

        inline static const std::string defaultScheme = Uri::schemeWebsocket(); //!< default scheme used to make a data exchange
    };

    IDataExchangePtr makeDataExchange(const std::string &scheme = IDataExchange::defaultScheme); //!< create a concrete instance of
                                                                                                 //!< IDataExchange
    /** \} */
} // namespace NS_OSBASE::data
