// \brief Private implementation of the class ProcessImpl

#pragma once
#include "osApplication/ProcessSetting.h"
#include "osApplication/ServiceOptions.h"
#include "osData/AsyncData.h"
#include <windows.h>
#include <future>

namespace NS_OSBASE::application {

    class ProcessImpl;
    using ProcessImplPtr = std::shared_ptr<ProcessImpl>;

    class ProcessImpl {
    public:
        ProcessImpl(const ProcessSetting &setting, const std::string &data);
        ~ProcessImpl();

        std::string getStrData() const;
        const ServiceOptions &getOptions() const;

    private:
        void start();
        void stop();

        void waitEndProcess();

        ProcessSetting m_setting;
        PROCESS_INFORMATION m_processInfo = {};
        bool m_bRunning                   = false;
        std::atomic_bool m_bStopRequired  = false;
        std::future<void> m_waitEndProcess;
        ServiceOptions m_options;
    };
} // namespace NS_OSBASE::application
