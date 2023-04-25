// \brief Declaration of the group of output logs

#include "osData/LogOutputGroup.h"

namespace NS_OSBASE::data {

    /*
     * \class LogOutputGroup
     */
    void LogOutputGroup::log(std::string &&msg) {
        for (auto &&pLogOutput : m_pLogOutputs) {
            pLogOutput->log(std::string(msg));
        }
    }

    void LogOutputGroup::flush(std::ostream &os) {
        for (auto &&pLogOutput : m_pLogOutputs) {
            pLogOutput->flush(os);
        }
    }

    void LogOutputGroup::add(ILogOutputPtr pLogOutput) {
        if (pLogOutput != nullptr) {
            m_pLogOutputs.push_back(pLogOutput);
        }
    }

    void LogOutputGroup::remove(ILogOutputPtr pLogOutput) {
        m_pLogOutputs.erase(std::remove_if(m_pLogOutputs.begin(),
                                m_pLogOutputs.end(),
                                [&pLogOutput](auto const pCurrentLogOutput) { return pLogOutput == pCurrentLogOutput; }),
            m_pLogOutputs.cend());
    }

    size_t LogOutputGroup::getCount() const {
        return m_pLogOutputs.size();
    }

    void LogOutputGroup::clear() {
        m_pLogOutputs.clear();
    }

    ILogOutputPtrs::iterator LogOutputGroup::begin() {
        return m_pLogOutputs.begin();
    }

    ILogOutputPtrs::iterator LogOutputGroup::end() {
        return m_pLogOutputs.end();
    }

    /*
     * \brief maker
     */
    LogOutputGroupPtr makeLogOutputGroup() {
        return LogOutputGroupPtr(new LogOutputGroup());
    }
} // namespace NS_OSBASE::data
