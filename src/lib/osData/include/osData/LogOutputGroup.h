// \brief Declaration of the group of output logs

#pragma once
#include "ILogOutput.h"

namespace cho::osbase::data {
    /**
     * \addtogroup PACKAGE_OSBASE_LOGS
     * \{
     */

    class LogOutputGroup;
    using LogOutputGroupPtr = std::shared_ptr<LogOutputGroup>; //!< alias for the shared pointer of LogOutputGroup

    /**
     * \brief Group of logs
     */
    class LogOutputGroup final : public ILogOutput {
        friend LogOutputGroupPtr makeLogOutputGroup();

    public:
        void log(std::string &&msg) override;
        void flush(std::ostream &os) override;

        void add(ILogOutputPtr pLogOutput);    //!< append a log output
        void remove(ILogOutputPtr pLogOutput); //!< remove a log output
        size_t getCount() const;               //!< return the number of log outputs in the log
        void clear();                          //!< remove all the log ouptuts from the group

        ILogOutputPtrs::iterator begin(); //!< used for the for-range-loop
        ILogOutputPtrs::iterator end();   //!< used for the for-range-loop

    private:
        LogOutputGroup() = default;

        ILogOutputPtrs m_pLogOutputs;
    };

    LogOutputGroupPtr makeLogOutputGroup(); //!< build and return a log output group
    /** \} */

} // namespace cho::osbase::data
