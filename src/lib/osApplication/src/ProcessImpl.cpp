// \brief ProcessImpl launcher

#include "ProcessImpl.h"
#include <future>
#include <iostream>
#include <tchar.h>
#include <tlhelp32.h>

using namespace std::chrono_literals;

namespace {
    std::string lastErrorToString() {
        LPVOID lpMsgBuf = nullptr;
        DWORD error     = GetLastError();

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPTSTR>(&lpMsgBuf),
            0,
            nullptr);

        std::wstring tmp(reinterpret_cast<LPTSTR>(lpMsgBuf));
        std::string retval;
        std::transform(tmp.begin(), tmp.end(), std::back_inserter(retval), [](auto c) { return static_cast<char>(c); });
        return retval;
    }
} // namespace

namespace NS_OSBASE::application {
    namespace {
        constexpr auto timeout = 10000ms;

        bool killProcessByName(const std::string &processName) {
            bool bKilled     = false;
            auto toLowerCase = [](const std::wstring &str) {
                std::string strLowerCase;
                for (auto c : str) {
                    strLowerCase.push_back(static_cast<char>(std::tolower(c)));
                }
                return strLowerCase;
            };
            auto const lowerCaseProcessName = toLowerCase(type_cast<std::wstring>(processName));
            auto const hSnapShot            = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
            PROCESSENTRY32 processEntry{ sizeof(PROCESSENTRY32) };

            if (Process32First(hSnapShot, &processEntry) == TRUE) {
                while (Process32Next(hSnapShot, &processEntry) == TRUE) {
                    if (lowerCaseProcessName == toLowerCase(processEntry.szExeFile)) {
                        auto const hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)processEntry.th32ProcessID);
                        if (hProcess != nullptr) {
                            TerminateProcess(hProcess, 9);
                            CloseHandle(hProcess);
                            bKilled = true;
                        }
                    }
                }
            }
            CloseHandle(hSnapShot);

            return bKilled;
        }
    } // namespace

    /*
     * \class ProcessImpl
     */

    ProcessImpl::ProcessImpl(const ProcessSetting &setting, const ServiceOptions &options) : m_setting(setting), m_options(options) {
        ZeroMemory(&m_processInfo, sizeof(m_processInfo));
        start();
    }

    ProcessImpl::~ProcessImpl() {
        stop();
    }

    void ProcessImpl::start() {
        std::cout << "Starting process: " << m_setting.serviceName << "..." << std::endl;

        m_data.create();

        if (m_setting.killIfExist && killProcessByName(m_setting.serviceName)) {
            std::cout << "An existing process: " << m_setting.serviceName << " has been killed" << std::endl;
        }

        m_options.dataUrl = m_data.getUriOfCreator();

        auto const commandLine = m_setting.serviceName + " " + ServiceCommandParser(m_options).makeCommandLine();
        auto const pStrCmdLine = std::make_unique<char[]>(commandLine.size() + 1);
        strcpy_s(pStrCmdLine.get(), commandLine.size() + 1, commandLine.c_str());

        STARTUPINFOA startupInfo{ sizeof(startupInfo) };
        if (!m_setting.displayConsole) {
            startupInfo.dwFlags |= STARTF_USESTDHANDLES;
        }

        m_bRunning = CreateProcessA(nullptr,
                         pStrCmdLine.get(),
                         nullptr,
                         nullptr,
                         TRUE,
                         m_setting.displayConsole ? CREATE_NEW_CONSOLE : 0,
                         nullptr,
                         (std::filesystem::current_path() / m_setting.serviceName).parent_path().string().c_str(),
                         &startupInfo,
                         &m_processInfo) == TRUE;

        if (m_bRunning) {
            std::cout << "ProcessImpl: " << m_setting.serviceName << " started!" << std::endl;
            m_bStopRequired = false;
            if (!m_waitEndProcess.valid()) {
                m_waitEndProcess = std::async([this]() { waitEndProcess(); });
            }

            m_data.waitConnection();
        } else {
            std::cerr << "*** Failed to start the process: " << m_setting.serviceName << ": " << lastErrorToString() << std::endl;
        }

        std::cout << std::endl;
    }

    void ProcessImpl::stop() {
        if (!m_bRunning)
            return;

        std::cout << "Stopping process: " << m_setting.serviceName << "..." << std::endl;
        m_data.set("end");

        m_bStopRequired = true;
        if (m_waitEndProcess.wait_for(timeout) == std::future_status::timeout) {
            std::cerr << "*** Fail to stop the process: " << m_setting.serviceName << "!" << std::endl;
        }
        m_data.reset();
    }

    void ProcessImpl::waitEndProcess() {
        // Manage the au-restart if required
        bool bLastWait                       = false;
        DWORD waitForSingleObjectReturnValue = WAIT_FAILED;

        while (!(m_bStopRequired && bLastWait)) {
            if (!m_bRunning) {
                break;
            }

            waitForSingleObjectReturnValue = WaitForSingleObject(m_processInfo.hProcess, m_setting.endProcessTimeout);
            if (waitForSingleObjectReturnValue == WAIT_OBJECT_0) {
                CloseHandle(m_processInfo.hProcess);
                CloseHandle(m_processInfo.hThread);
                if (m_bStopRequired) {
                    std::cout << "ProcessImpl: " << m_setting.serviceName << " stopped!" << std::endl;
                } else {
                    std::cerr << "*** The process: " << m_setting.serviceName << " is stopped!" << std::endl;
                    if (m_setting.autoRestart) {
                        std::cout << std::endl;
                        std::cout << "Try to restart the process: " << m_setting.serviceName << std::endl;
                        m_bRunning = false;
                        start();
                    } else {
                        break;
                    }
                }
            }

            if (m_bStopRequired) {
                bLastWait = true;
            }
        }

        // managing error cases
        if (waitForSingleObjectReturnValue != WAIT_OBJECT_0) {
            const std::string rootCause = waitForSingleObjectReturnValue == WAIT_TIMEOUT ? "root cause: timeout" : "root cause: unknown";

            std::cout << "ProcessImpl: " << m_setting.serviceName << " unable to stop the process. Try to kill it. " << rootCause
                      << std::endl;

            TerminateProcess(m_processInfo.hProcess, 1);
        }

        std::cout << std::endl;
        m_bRunning = false;
    }

    std::string ProcessImpl::getData() const {
        if (!m_data.isConnected()) {
            return {};
        }

        auto &self = const_cast<ProcessImpl &>(*this);
        return self.m_data.get();
    }
} // namespace NS_OSBASE::application
