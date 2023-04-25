// \file  TaskQueue.cpp
// \brief Implementation of the class TaskQueue

#include "osApplication/TaskQueue.h"
#include <algorithm>

namespace NS_OSBASE::application {

    /*
     * \class TaskQueue
     */
    ITaskPtr TaskQueue::waitForTask(const std::chrono::milliseconds &delay) {
        return waitUntilTask(delay == std::chrono::milliseconds::max() ? std::chrono::time_point<clock>::max() : clock::now() + delay);
    }

    ITaskPtr TaskQueue::waitUntilTask(const time_point &timestamp) {
        std::unique_lock<std::mutex> locker(m_scheduledTasksMutex);
        ITaskPtr pTask;
        bool bTimestampReached = false;
        do {
            auto const taskTimestamp         = m_scheduledTasks.empty() ? timestamp : m_scheduledTasks.front().timestamp;
            const bool bWaitForTaskTimeStamp = taskTimestamp < timestamp;
            m_lastTimeStamp                  = taskTimestamp < timestamp ? taskTimestamp : timestamp;

            bTimestampReached = m_scheduledTasksCV.wait_until(locker, m_lastTimeStamp) == std::cv_status::timeout;
            if (bTimestampReached && bWaitForTaskTimeStamp) {
                locker.unlock();
                pTask = popTask(m_lastTimeStamp);
            }
        } while (!bTimestampReached && !m_scheduledTasks.empty());

        return pTask;
    }

    ITaskPtr TaskQueue::pullTask() {
        std::unique_lock<std::mutex> locker(m_scheduledTasksMutex);
        if (m_scheduledTasks.empty())
            return nullptr;

        ITaskPtr pTask;
        auto const now           = clock::now();
        auto const taskTimestamp = m_scheduledTasks.front().timestamp;

        if (taskTimestamp <= now) {
            m_lastTimeStamp = taskTimestamp;
            locker.unlock();
            pTask = popTask(taskTimestamp);
        }

        return pTask;
    }

    bool TaskQueue::isRemainingTasks() const {
        std::unique_lock<std::mutex> locker(m_scheduledTasksMutex);
        return !m_scheduledTasks.empty();
    }

    void TaskQueue::clearTasks() {
        std::unique_lock<std::mutex> locker(m_scheduledTasksMutex);
        m_scheduledTasks.clear();
    }

    const TaskQueue::time_point &TaskQueue::getLastTimeStamp() const {
        return m_lastTimeStamp;
    }

    ITaskPtr TaskQueue::pushScheduledTask(
        const time_point &timeStampRef, const std::chrono::milliseconds &delay, ITaskPtr pTask, const bool bRepeated) {
        using namespace std::chrono_literals;

        if (pTask == nullptr)
            return nullptr;

        std::unique_lock<std::mutex> locker(m_scheduledTasksMutex);

        time_point timeStamp;
        if (delay == std::chrono::milliseconds::min()) {
            if (m_scheduledTasks.empty())
                timeStamp = timeStampRef;
            else
                timeStamp = std::min(m_scheduledTasks.front().timestamp, timeStampRef) - 1ms;
        } else {
            timeStamp = timeStampRef + delay;
        }

        m_scheduledTasks.push_back(ScheduledTask({ timeStamp, pTask, bRepeated ? delay : std::chrono::milliseconds::min() }));
        m_scheduledTasks.sort([](auto const &lhs, auto const &rhs) { return lhs.timestamp < rhs.timestamp; });
        m_scheduledTasksCV.notify_one();
        return pTask;
    }

    ITaskPtr TaskQueue::popTask(const time_point &timeStamp) {
        using namespace std::chrono_literals;
        std::unique_lock<std::mutex> locker(m_scheduledTasksMutex);
        auto const scheduledTask = m_scheduledTasks.front();
        m_scheduledTasks.pop_front();

        auto pTask = scheduledTask.pTask;

        if (pTask->isEnabled() && scheduledTask.delayRepetition >= 0ms) {
            locker.unlock();
            pushScheduledTask(timeStamp, scheduledTask.delayRepetition, pTask, true);
        }

        return pTask;
    }

} // namespace NS_OSBASE::application
