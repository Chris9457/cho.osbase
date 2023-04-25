// \file  TaskLoop.inl
// \brief Implementation of the class TaskLoop

#pragma once

namespace NS_OSBASE::application {
    template <typename TCallback, typename... TArgs>
    ITaskPtr TaskLoop::push(TCallback &&callback, TArgs &&...args) {
        return m_taskScheduler.pushTask(std::forward<TCallback>(callback), std::forward<TArgs>(args)...);
    }

    template <typename TCallback, typename... TArgs>
    ITaskPtr TaskLoop::pushImmediate(TCallback &&callback, TArgs &&...args) {
        return m_taskScheduler.pushImmediateTask(std::forward<TCallback>(callback), std::forward<TArgs>(args)...);
    }

    template <typename TCallback, typename... TArgs>
    ITaskPtr TaskLoop::pushSingleShot(const std::chrono::milliseconds &delay, TCallback &&callback, TArgs &&...args) {
        return m_taskScheduler.pushSingleShotTask(delay, std::forward<TCallback>(callback), std::forward<TArgs>(args)...);
    }

    template <typename TCallback, typename... TArgs>
    ITaskPtr TaskLoop::pushRepeated(const std::chrono::milliseconds &delay, TCallback &&callback, TArgs &&...args) {
        return m_taskScheduler.pushRepeatedTask(delay, std::forward<TCallback>(callback), std::forward<TArgs>(args)...);
    }

    template <typename TInstance, typename TMthCallback, typename... TArgs>
    ITaskPtr TaskLoop::pushMethod(std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args) {
        return m_taskScheduler.pushMethodTask(pInstance, std::forward<TMthCallback>(mthCallback), std::forward<TArgs>(args)...);
    }

    template <typename TInstance, typename TMthCallback, typename... TArgs>
    ITaskPtr TaskLoop::pushImmediateMethod(std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args) {
        return m_taskScheduler.pushImmediateMethodTask(pInstance, std::forward<TMthCallback>(mthCallback), std::forward<TArgs>(args)...);
    }

    template <typename TInstance, typename TMthCallback, typename... TArgs>
    ITaskPtr TaskLoop::pushSingleShotMethod(
        const std::chrono::milliseconds &delay, std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args) {
        return m_taskScheduler.pushSingleShotMethodTask(
            delay, pInstance, std::forward<TMthCallback>(mthCallback), std::forward<TArgs>(args)...);
    }

    template <typename TInstance, typename TMthCallback, typename... TArgs>
    ITaskPtr TaskLoop::pushRepeatedMethod(
        const std::chrono::milliseconds &delay, std::shared_ptr<TInstance> pInstance, TMthCallback &&mthCallback, TArgs &&...args) {
        return m_taskScheduler.pushRepeatedMethodTask(
            delay, pInstance, std::forward<TMthCallback>(mthCallback), std::forward<TArgs>(args)...);
    }

} // namespace NS_OSBASE::application
