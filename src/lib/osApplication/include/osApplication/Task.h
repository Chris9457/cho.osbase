// \file  Task.h
// \brief Declaration of the class Task

#pragma once
#include <memory>
#include <tuple>

/**
 * \defgroup PACKAGE_TASK Tasks
 * \brief   Set of classes and functions that manages tasks
 *
 * A task is an object that encapsulates a dalayed call.
 *
 * \par Creating a task
 * A task is created by the following functions:
 *  - makeTypedTask() : create a task from a function with a typed return
 *  - makeTask() : create a task from a function with no return
 *  - makeTypedMethodTask() : create a task from a class method with a typed return
 *  - makeMethodTask() : create a task from a class method with no return.
 *  \n
 *  Examples:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 int doSomethingWithReturn(int i, std::string) { ... }
 void doSomething(double d) { ... }

 struct MyStruct {
    int doWithRet(char c);
    void do(int i);
 };
 ...
 auto pMyStruct = std::make_shared<MyStruct>();

 auto pRetFnTask = makeTypedTask<int>(doSomethingWithReturn, 2, "toto");
 auto pRetLambdaTask = makeTypedTask<std::string>([](int i) -> std::string { ... }, 2);
 auto pRetMthTask = makeTypedMethodTask<int>(pMyStruct, &MyStruct::doWithRet, 'g');

 auto pFnTask = makeTask(doSomething, 2.);
 auto pLambdaTask = makeTask([](int i) -> void { ... }, 2);
 auto pMthTask = makeMethodTask(pMyStruct, &MyStruct::do, 55);
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * \ingroup PACKAGE_APPLICATION
 */

namespace cho::osbase::application {

    template <typename>
    class ITypedTask;
    template <typename TRet>
    using ITypedTaskPtr = std::shared_ptr<ITypedTask<TRet>>; //!< Alias for shared pointers on ITypedTask

    /**
     * \brief 	This class store the callback and its arguments to be called later
     * \ingroup PACKAGE_TASK
     */
    template <typename TRet>
    class ITypedTask {
    public:
        virtual ~ITypedTask()  = default; //!< dtor
        virtual TRet execute() = 0;       //!< Perform the execution of the task using the arguments stored

        virtual bool isEnabled() const               = 0; //!< indicates if the task is executable
        virtual void setEnabled(const bool bEnabled) = 0; //!< assign the execution control (false: doen't execute)
    };

    /**
     * \brief concrete impl
     * \ingroup PACKAGE_TASK
     */
    template <typename TRet, typename TCallback, typename... TArgs>
    class TypedTask final : public ITypedTask<TRet> {
    public:
        TypedTask(const TCallback &callback, TArgs &&...args); //!< pass args by value (copy mandatory for further calls)
        TRet execute() override;
        bool isEnabled() const override;
        void setEnabled(const bool bEnabled) override;

    private:
        TCallback m_callback;
        std::tuple<TArgs...> m_args;
        bool m_bEnabled = true;
    };

    /**
     * \addtogroup PACKAGE_TASK
     * \{
     */
    using ITask    = ITypedTask<void>;    //!< Alias for tasks with no return
    using ITaskPtr = ITypedTaskPtr<void>; //!< Shared pointer on tasks with no return

    /**
     * \brief Create a task with a return type
     * \param callback  function to call when invoking the <b>execute</b> method
     * \param args      arguments to pass to the callback
     */
    template <typename TRet, typename TCallback, typename... TArgs>
    ITypedTaskPtr<TRet> makeTypedTask(const TCallback &callback, TArgs &&...args);

    /**
     * \brief Create a task with no return type
     * \copydetails makeTypedTask()
     */
    template <typename TCallback, typename... TArgs>
    ITaskPtr makeTask(const TCallback &callback, TArgs &&...args);

    /**
     * \brief Create a task with a return type from a method
     * \param pInstance     shared pointer on the intance associated to the method
     * \param mthCallback   method to call when invoking the <b>execute</b> method
     * \param args          arguments to pass to the callback
     *
     * \remark  This function create a task which stores the weak pointer of <b>pInstance</b> (doesn't take the ownership). Thus, the method
     * is invoked only if <b>pInstance</b> is not expired.
     */
    template <typename TRet, typename TInstance, typename TMthCallback, typename... TArgs>
    ITypedTaskPtr<TRet> makeTypedMethodTask(std::shared_ptr<TInstance> pInstance, const TMthCallback &mthCallback, TArgs &&...args);

    /**
     * \brief Create a task with no return type from a method
     * \copydetails makeTypedMethodTask()
     */
    template <typename TInstance, typename TMthCallback, typename... TArgs>
    ITaskPtr makeMethodTask(std::shared_ptr<TInstance> pInstance, const TMthCallback &mthCallback, TArgs &&...args);
    /**\}*/

} // namespace cho::osbase::application

#include "Task.inl"
