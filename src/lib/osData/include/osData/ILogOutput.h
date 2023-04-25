// \brief Definition of the interface of the logs

#pragma once
#include "IDataExchange.h"
#include "osCore/Misc/NonCopyable.h"
#include <memory>
#include <string>
#include <vector>

namespace NS_OSBASE::data {

    class ILogOutput;
    using ILogOutputPtr  = std::shared_ptr<ILogOutput>;
    using ILogOutputPtrs = std::vector<ILogOutputPtr>;

    /**
     * \brief Interface for log output
     * \ingroup PACKAGE_OSBASE_LOGS
     */
    class ILogOutput : core::NonCopyable {
    public:
        virtual ~ILogOutput() = default; //!< dtor

        virtual void log(std::string &&msg)  = 0; //!< add the msg to the concrete output
        virtual void flush(std::ostream &os) = 0; //!< flush the unlogged messages to the output stream
    };

    ILogOutputPtr makeLogOutputNull();
    ILogOutputPtr makeLogOutputFile(const std::filesystem::path &path);
    ILogOutputPtr makeLogOutputDataExchange(const Uri &uri);
    ILogOutputPtr makeLogOutputDataExchange(IDataExchangePtr pDataExchange);
    ILogOutputPtr makeLogOutputConsole();
    ILogOutputPtr makeLogOutputDebug();

} // namespace NS_OSBASE::data
