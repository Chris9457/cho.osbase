// \brief Process launcher

#pragma once
#include "ProcessSetting.h"
#include "osApplication/ServiceCommandParser.h"
#include <memory>

namespace NS_OSBASE::application {

    class ProcessImpl;
    using ProcessImplPtr = std::shared_ptr<ProcessImpl>;

    class Process;
    using ProcessPtr = std::shared_ptr<Process>;

    class Process {
    public:
        static ProcessPtr create(const ProcessSetting &setting, const ServiceOptions &options);

    private:
        Process(const ProcessSetting &setting, const ServiceOptions &options);

        ProcessImplPtr m_pImpl;
    };
} // namespace NS_OSBASE::application
