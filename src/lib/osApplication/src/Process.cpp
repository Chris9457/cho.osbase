// \brief Process launcher

#include "osApplication//Process.h"
#include "ProcessImpl.h"

using namespace std::chrono_literals;

namespace NS_OSBASE::application {
    ProcessPtr Process::create(const ProcessSetting &setting) {
        return ProcessPtr(new Process(setting, std::string{}));
    }

    Process::Process(const ProcessSetting &setting, const std::string &strData) : m_pImpl(std::make_shared<ProcessImpl>(setting, strData)) {
    }

    const ServiceOptions &Process::getOptions() const {
        return m_pImpl->getOptions();
    }
} // namespace NS_OSBASE::application
