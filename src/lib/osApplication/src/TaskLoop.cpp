// \file  TaskLoop.cpp
// \brief Implementation of the class TaskLoop

#include "osApplication/TaskLoop.h"
#include "osData/Log.h"
#include "osCore/Exception/Exception.h"
#include "osCore/Misc/ScopeValue.h"

namespace {
    const char *runningError = "loop is already running!";
}

namespace NS_OSBASE::application {
    TaskLoop::~TaskLoop() {
        if (isRunning()) {
            stop();
        }
    }

    const TaskQueue::time_point &TaskLoop::getLastTimeStamp() const {
        return m_taskScheduler.getLastTimeStamp();
    }

    void TaskLoop::run() {
        m_thId = std::this_thread::get_id();

        {
            const std::lock_guard locker(m_runningMutex);
            if (isRunning()) {
                throw TaskLoopException(runningError);
            }
        }

        m_bEnd = false;
        while (!m_bEnd) {
            if (auto const pTask = m_taskScheduler.waitForTask(); pTask != nullptr) {
                try {
                    oscheck::throwIfCrash([&pTask]() { pTask->execute(); });
                } catch (const core::LogicException &e) {
                    oslog::error(data::OS_LOG_CHANNEL_APPLICATION) << e.what() << oslog::end();
                } catch (const core::RuntimeException &e) {
                    oslog::error(data::OS_LOG_CHANNEL_APPLICATION) << e.what() << oslog::end();
                    if (auto const pDelegate = m_pRuntimeErrorDelegate.lock(); pDelegate != nullptr) {
                        pDelegate->onRuntimeError(e.what());
                    } else {
                        throw e;
                    }
                }
            }
        }
    }

    std::shared_future<void> TaskLoop::runAsync() {
        const std::lock_guard<std::recursive_mutex> locker(m_runningMutex);
        if (isRunning()) {
            throw TaskLoopException(runningError);
        }

        m_asyncRunSharedReturn = std::async([this]() { run(); }).share();
        return m_asyncRunSharedReturn;
    }

    bool TaskLoop::isRunning() const {
        const std::lock_guard locker(m_runningMutex);
        return !m_bEnd;
    }

    void TaskLoop::stop() {
        push([this]() { m_bEnd = true; });
        auto const currentThId = std::this_thread::get_id();
        if (m_thId != currentThId && m_asyncRunSharedReturn.valid()) {
            m_asyncRunSharedReturn.wait();
        }
        m_bEnd = true;
    }

    void TaskLoop::setRuntimeErrorDelegate(IRuntimeErrorDelegatePtr pDelegate) {
        m_pRuntimeErrorDelegate = pDelegate;
    }
} // namespace NS_OSBASE::application
