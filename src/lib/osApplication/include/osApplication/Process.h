// \brief Process launcher

#pragma once
#include "ProcessSetting.h"
#include "osApplication/ServiceOptions.h"
#include <memory>

namespace NS_OSBASE::application {

    class ProcessImpl;
    using ProcessImplPtr = std::shared_ptr<ProcessImpl>;

    class Process;
    using ProcessPtr = std::shared_ptr<Process>;

    class Process {
    public:
        static ProcessPtr create(const ProcessSetting &setting);
        template <typename T>
        static ProcessPtr create(const ProcessSetting &setting, const T &data);

        template <typename T>
        T getData(const std::chrono::milliseconds &timeout) const;

    private:
        Process(const ProcessSetting &setting, const std::string &strData);

        const ServiceOptions &getOptions() const;

        ProcessImplPtr m_pImpl;
    };
} // namespace NS_OSBASE::application

#include "Process.inl"
