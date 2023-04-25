// \file  TaskQueue.inl
// \brief Implementation of the class TaskQueue

#pragma once

namespace NS_OSBASE::application {
    template <typename TCallback, typename... TArgs>
    ITaskPtr TaskQueue::pushTask(TCallback &&callback, TArgs &&...args) {
        using namespace std::chrono_literals;
        return pushScheduledTask(clock::now(),
            std::chrono::milliseconds::zero(),
            makeTask(std::forward<TCallback>(callback), std::forward<TArgs>(args)...),
            false);
    }

    template <typename TCallback, typename... TArgs>
    ITaskPtr TaskQueue::pushImmediateTask(TCallback &&callback, TArgs &&...args) {
        using namespace std::chrono_literals;
        return pushScheduledTask(clock::now(),
            std::chrono::milliseconds::min(),
            makeTask(std::forward<TCallback>(callback), std::forward<TArgs>(args)...),
            false);
    }

    template <typename TCallback, typename... TArgs>
    ITaskPtr TaskQueue::pushSingleShotTask(const std::chrono::milliseconds &delay, TCallback &&callback, TArgs &&...args) {
        return pushScheduledTask(clock::now(), delay, makeTask(std::forward<TCallback>(callback), std::forward<TArgs>(args)...), false);
    }

    template <typename TCallback, typename... TArgs>
    ITaskPtr TaskQueue::pushRepeatedTask(const std::chrono::milliseconds &delay, TCallback &&callback, TArgs &&...args) {
        return pushScheduledTask(clock::now(), delay, makeTask(std::forward<TCallback>(callback), std::forward<TArgs>(args)...), true);
    }

    template <typename TInstance, typename TMthCallback, typename... TArgs>
    ITaskPtr TaskQueue::pushMethodTask(std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args) {
        using namespace std::chrono_literals;
        return pushScheduledTask(clock::now(),
            std::chrono::milliseconds::zero(),
            makeMethodTask(pInstance, std::forward<TMthCallback>(mthCallback), std::forward<TArgs>(args)...),
            false);
    }

    template <typename TInstance, typename TMthCallback, typename... TArgs>
    ITaskPtr TaskQueue::pushImmediateMethodTask(std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args) {
        using namespace std::chrono_literals;
        return pushScheduledTask(clock::now(),
            std::chrono::milliseconds::min(),
            makeMethodTask(pInstance, std::forward<TMthCallback>(mthCallback), std::forward<TArgs>(args)...),
            false);
    }

    template <typename TInstance, typename TMthCallback, typename... TArgs>
    ITaskPtr TaskQueue::pushSingleShotMethodTask(
        const std::chrono::milliseconds &delay, std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args) {
        return pushScheduledTask(
            clock::now(), delay, makeMethodTask(pInstance, std::forward<TMthCallback>(mthCallback), std::forward<TArgs>(args)...), false);
    }

    template <typename TInstance, typename TMthCallback, typename... TArgs>
    ITaskPtr TaskQueue::pushRepeatedMethodTask(
        const std::chrono::milliseconds &delay, std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args) {
        return pushScheduledTask(
            clock::now(), delay, makeMethodTask(pInstance, std::forward<TMthCallback>(mthCallback), std::forward<TArgs>(args)...), true);
    }

} // namespace NS_OSBASE::application
