// \brief An abstract interface intended to  define the point to point data exchange between 2 classes

#pragma once
#include "IExchange.h"
#include <filesystem>

#include <memory>

namespace cho::osbase::data {

    /**
     * \addtogroup PACKAGE_OSBASE_EXCHANGE
     * \{
     */

    /**
     * \brief Exception thrown by FileExchange classes
     */
    class FileExchangeException : public core::LogicException {
        using core::LogicException::LogicException;
    };

    class IFileExchange;
    using IFileExchangePtr = std::shared_ptr<IFileExchange>; //!< alias of shared pointer on IFileExchange

    /**
     * \brief This interface specialize the interface IExchange by adding the "push" method to transfer files
     */
    class IFileExchange : public IExchange {

    public:
        /**
         * \brief Add the data received notification
         */
        class IDelegate : public IExchange::IDelegate {
        public:
            virtual void onFileReceived(const std::filesystem::path &path) = 0; //!< method called on a file is received
        };
        using IDelegatePtr  = std::shared_ptr<IDelegate>; //!< alias for shared pointer on IDelegate
        using IDelegateWPtr = std::weak_ptr<IDelegate>;   //!< alias for weak pointer on IDelegate

        virtual void push(const std::filesystem::path &path) const = 0; //!< push file / directory  associated to the path
        virtual void setDelegate(IDelegatePtr pDelegate) noexcept  = 0; //!< assign a delegate

        inline static const std::string defaultScheme = Uri::schemeFile(); //!< default scheme used to make a file exchange
    };

    IFileExchangePtr makeFileExchange(const std::string &scheme = IFileExchange::defaultScheme); //!< create a IFialeExchange
    /** \} */

} // namespace cho::osbase::data
