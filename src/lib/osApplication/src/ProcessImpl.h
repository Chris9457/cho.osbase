// \brief Private implementation of the class Process

#pragma once
#include "osApplication/ProcessSetting.h"
#include "osApplication/ServiceCommandParser.h"
#include "osData/AsyncData.h"
#include <windows.h>
#include <future>

namespace NS_OSBASE::application {

    class ProcessImpl;
    using ProcessImplPtr = std::shared_ptr<ProcessImpl>;

    class ProcessImpl {
    public:
        ProcessImpl(const ProcessSetting &setting, const NS_OSBASE::application::ServiceOptions &options);
        ~ProcessImpl();

        std::string getData() const;

    private:
        void start();
        void stop();

        void waitEndProcess();

        ProcessSetting m_setting;
        ServiceOptions m_options;
        PROCESS_INFORMATION m_processInfo = {};
        bool m_bRunning                   = false;
        std::atomic_bool m_bStopRequired  = false;
        std::future<void> m_waitEndProcess;
        data::AsyncData<std::string> m_data;
    };
} // namespace NS_OSBASE::application
