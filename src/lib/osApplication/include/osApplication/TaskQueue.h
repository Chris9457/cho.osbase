// \file  TaskQueue.h
// \brief Declaration of the class TaskQueue

#pragma once
#include "Task.h"
#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>

namespace cho::osbase::application {

    /**
     * \brief 	This class represents a task queue:
     *
     * \remark  This class manages tasks that has no return (return void)
     * The tasks are sorted according their timestamp and their priority:
     *  - push: stores the task at the time the method is invoked (now)
     *  - push immediat: stores the task on top of the others if any (and so with no delay)
     *  - push single shot / repeated: stores the task at the time the method is invoked (now) plus a delay
     *
     * \ingroup PACKAGE_TASK
     */
    class TaskQueue {
    public:
        using clock      = std::chrono::system_clock; //!< alias for the clock used in the TaskQueue
        using time_point = clock::time_point;         //!< alias for the time point used in the TaskQueue

        /** \name For functions
         * \{
         */

        /**
         * \brief   Push a task at the timestamp now
         * \param   callback    function to invoke when executing the task
         * \param   args        arguments to pass to the callback
         */
        template <typename TCallback, typename... TArgs>
        ITaskPtr pushTask(TCallback &&callback, TArgs &&...args);

        /**
         * \brief   Push a task on top of the others if any, at the timestamp now
         * \param   callback    function to invoke when executing the task
         * \param   args        arguments to pass to the callback
         */
        template <typename TCallback, typename... TArgs>
        ITaskPtr pushImmediateTask(TCallback &&callback, TArgs &&...args); //!< Push a task on top of the others if any, at the timestamp
                                                                           //!< now

        /**
         * \brief   Push a task at the timestamp now plus the delay
         * \param   delay       delay since now the callback is invoked
         * \param   callback    function to invoke when executing the task
         * \param   args        arguments to pass to the callback
         */
        template <typename TCallback, typename... TArgs>
        ITaskPtr pushSingleShotTask(const std::chrono::milliseconds &delay, TCallback &&callback, TArgs &&...args);

        /**
         * \brief   Push a task at the timestamp now plus the delay, pushed again after wait or pull at this timestamp plus the delay
         * \param   delay       delay since now the callback is invoked
         * \param   callback    function to invoke when executing the task
         * \param   args        arguments to pass to the callback
         */
        template <typename TCallback, typename... TArgs>
        ITaskPtr pushRepeatedTask(const std::chrono::milliseconds &delay, TCallback &&callback, TArgs &&...args);
        /** \} */

        /** \name For methods
         * \{
         */

        /**
         * \brief   Push a method task at the timestamp now
         * \param   pInstance   shared pointer of the instance associated to the method
         * \param   mthCallback method to invoke when executing the task
         * \param   args        arguments to pass to the callback
         */
        template <typename TInstance, typename TMthCallback, typename... TArgs>
        ITaskPtr pushMethodTask(std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args);

        /**
         * \brief   Push a method task on top of the others if any, at the timestamp now
         * \param   pInstance   shared pointer of the instance associated to the method
         * \param   mthCallback method to invoke when executing the task
         * \param   args        arguments to pass to the callback
         */
        template <typename TInstance, typename TMthCallback, typename... TArgs>
        ITaskPtr pushImmediateMethodTask(std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args);

        /**
         * \brief   Push a method task at the timestamp now plus the delay
         * \param   delay       delay since now the callback is invoked
         * \param   pInstance   shared pointer of the instance associated to the method
         * \param   mthCallback method to invoke when executing the task
         * \param   args        arguments to pass to the callback
         */
        template <typename TInstance, typename TMthCallback, typename... TArgs>
        ITaskPtr pushSingleShotMethodTask(
            const std::chrono::milliseconds &delay, std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args);

        /**
         * \brief   Push a method task at the timestamp now plus the delay, pushed again after wait or pull at this timestamp plus the delay
         * \param   delay       delay since now the callback is invoked
         * \param   pInstance   shared pointer of the instance associated to the method
         * \param   mthCallback method to invoke when executing the task
         * \param   args        arguments to pass to the callback
         */
        template <typename TInstance, typename TMthCallback, typename... TArgs>
        ITaskPtr pushRepeatedMethodTask(
            const std::chrono::milliseconds &delay, std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args);
        /** \} */

        ITaskPtr pullTask(); //!< return the next available task

        ITaskPtr waitForTask(const std::chrono::milliseconds &delay = std::chrono::milliseconds::max()); //!< Wait until the delay expired
                                                                                                         //!< or a task is available
        ITaskPtr waitUntilTask(const time_point &timestamp); //!< Wait until the timestamp is reached or a task is available
        bool isRemainingTasks() const;                       //!< Indicates if tasks are still present
        void clearTasks();                                   //!< Remove all the pending tasks

        const time_point &getLastTimeStamp() const; //!< Return the timestamp of the last waited of pulled task

    private:
        struct ScheduledTask {
            time_point timestamp;
            ITaskPtr pTask;
            std::chrono::milliseconds delayRepetition;
        };

        ITaskPtr pushScheduledTask(
            const time_point &timeStampRef, const std::chrono::milliseconds &delay, ITaskPtr pTask, const bool bRepeated);
        ITaskPtr popTask(const time_point &timeStamp);

        std::list<ScheduledTask> m_scheduledTasks;
        mutable std::mutex m_scheduledTasksMutex;
        std::condition_variable m_scheduledTasksCV;
        time_point m_lastTimeStamp = std::chrono::time_point<clock>::min();
    };

} // namespace cho::osbase::application

#include "TaskQueue.inl"
