// \brief Process launcher

#include "osApplication//Process.h"
#include "ProcessImpl.h"

using namespace std::chrono_literals;

namespace cho::osbase::application {
    ProcessPtr Process::create(const ProcessSetting &setting, const ServiceOptions &options) {
        return ProcessPtr(new Process(setting, options));
    }

    Process::Process(const ProcessSetting &setting, const ServiceOptions &options)
        : m_pImpl(std::make_shared<ProcessImpl>(setting, options)) {
    }
} // namespace cho::osbase::application
