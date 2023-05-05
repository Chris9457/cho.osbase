// \brief Process launcher

#include "osApplication//Process.h"
#include "ProcessImpl.h"

using namespace std::chrono_literals;

namespace NS_OSBASE::application {
    ProcessPtr Process::create(const ProcessSetting &setting, const ServiceOptions &options) {
        return ProcessPtr(new Process(setting, options));
    }

    Process::Process(const ProcessSetting &setting, const ServiceOptions &options)
        : m_pImpl(std::make_shared<ProcessImpl>(setting, options)) {
    }

    std::string Process::getData() const {
        return m_pImpl->getData();
    }
} // namespace NS_OSBASE::application
