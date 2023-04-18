// \file  TaskLoop.h
// \brief Declaration of the class TaskLoop

#pragma once
#include "TaskLoopException.h"
#include "TaskQueue.h"
#include "osCore/Misc/NonCopyable.h"
#include <future>
#include <memory>
#include <mutex>
#include <thread>

namespace cho::osbase::application {
    class TaskLoop;
    using TaskLoopPtr = std::shared_ptr<TaskLoop>; //!< Alias on shared pointers on TaskLoop

    /**
     * \brief This class implement a loop over tasks
     * \copydetails cho::osbase::application::TaskQueue
     *
     * \remark
     * This class can be used to perform a loop on the current thread of its creation via the method run() or in a new thread via
     * the method runAsync().
     *
     * \par RuntimeErrorException management
     * When a RuntimeErrorException is thrown, if a IRuntimeErrorDelegate is set the method "onRuntimeError" is called and the loop is
     * not broken. Otherwise the exception is re-thrown and the loop is broken.
     *
     * \throws RuntimeErrorException if no IRuntimeErrorDelegate is set
     *
     * \ingroup PACKAGE_TASK
     */
    class TaskLoop : private core::NonCopyable {
    public:
        using clock = TaskQueue::clock; //!< Alias for the clock used in TaskLoop

        /**
         * \brief Delegate called in case of runtime error
         */
        class IRuntimeErrorDelegate {
        public:
            virtual ~IRuntimeErrorDelegate()                         = default; //!< dtor
            virtual void onRuntimeError(const std::string &strError) = 0;       //!< called when a RuntimeErrorException is thrown
        };
        using IRuntimeErrorDelegatePtr  = std::shared_ptr<IRuntimeErrorDelegate>; //!< alias on shared pointer
        using IRuntimeErrorDelegateWPtr = std::weak_ptr<IRuntimeErrorDelegate>;   //!< alias on weak pointer

        ~TaskLoop();

        /** \name For functions
         * \{
         */

        /**
         * \copydoc TaskQueue::pushTask
         */
        template <typename TCallback, typename... TArgs>
        ITaskPtr push(TCallback &&callback, TArgs &&...args);

        /**
         * \copydoc TaskQueue::pushImmediateTask
         */
        template <typename TCallback, typename... TArgs>
        ITaskPtr pushImmediate(TCallback &&callback, TArgs &&...args);

        /**
         * \copydoc TaskQueue::pushSingleShotTask
         */
        template <typename TCallback, typename... TArgs>
        ITaskPtr pushSingleShot(const std::chrono::milliseconds &delay, TCallback &&callback, TArgs &&...args);

        /**
         * \copydoc TaskQueue::pushRepeatedTask
         */
        template <typename TCallback, typename... TArgs>
        ITaskPtr pushRepeated(const std::chrono::milliseconds &delay, TCallback &&callback, TArgs &&...args);
        /** \} */

        /** \name For methods
         * \{
         */

        /**
         * \copydoc TaskQueue::pushMethodTask
         */
        template <typename TInstance, typename TMthCallback, typename... TArgs>
        ITaskPtr pushMethod(std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args);

        /**
         * \copydoc TaskQueue::pushImmediateMethodTask
         */
        template <typename TInstance, typename TMthCallback, typename... TArgs>
        ITaskPtr pushImmediateMethod(std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args);

        /**
         * \copydoc TaskQueue::pushSingleShotMethodTask
         */
        template <typename TInstance, typename TMthCallback, typename... TArgs>
        ITaskPtr pushSingleShotMethod(
            const std::chrono::milliseconds &delay, std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args);

        /**
         * \copydoc TaskQueue::pushRepeatedMethodTask
         */
        template <typename TInstance, typename TMthCallback, typename... TArgs>
        ITaskPtr pushRepeatedMethod(
            const std::chrono::milliseconds &delay, std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args);
        /** \} */

        /**
         * \copydoc TaskQueue::getLastTimeStamp
         */
        const TaskQueue::time_point &getLastTimeStamp() const;

        /**
         * \brief   Perform a synchroneous loop
         * \throws  TaskLoopException if the loop is already running (by run or runAsync)
         */
        void run();

        /**
         * \brief   Perform an asynchroneous loop
         * \throws  TaskLoopException if the loop is already running (by run or runAsync)
         * \return  The future on which the user can wait the end
         */
        std::shared_future<void> runAsync();

        bool isRunning() const; //!< Indicates if the loop is waiting to execute tasks
        void stop();            //!< Stop the synchroneous or asynchroneous loop

        void setRuntimeErrorDelegate(IRuntimeErrorDelegatePtr pDelegate); //!< assign the delegate to call when a RuntimeErrorException is
                                                                          //!< thrown

    private:
        TaskQueue m_taskScheduler;
        std::shared_future<void> m_asyncRunSharedReturn;
        std::atomic_bool m_bEnd = true;
        mutable std::recursive_mutex m_runningMutex;
        IRuntimeErrorDelegateWPtr m_pRuntimeErrorDelegate;
        std::atomic<std::thread::id> m_thId = std::this_thread::get_id();
    };
} // namespace cho::osbase::application

#include "TaskLoop.inl"
