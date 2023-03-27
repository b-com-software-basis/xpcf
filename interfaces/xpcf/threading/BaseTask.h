/**
 * @copyright Copyright (c) 2018 B-com http://www.b-com.com/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author Loïc Touraine
 *
 * @file
 * @date 2018-07-10
 */

#ifndef ORG_BCOM_XPCF_BASETASK_H
#define ORG_BCOM_XPCF_BASETASK_H

#include "xpcf/threading/ITask.h"
#include "xpcf/threading/SharedFifo.h"
#include "xpcf/core/refs.h"

#include <thread>
#include <future>
#include <functional>
#include <vector>

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API AbstractTask : virtual public ITask
{
public:
    AbstractTask(bool joinable = true): m_joinable(joinable) {}
    virtual ~AbstractTask() override = default;
    virtual bool started() const override { return !m_stop; }
    virtual bool stopped() const override { return m_stop; }
    virtual void cleanup() override;
    virtual void declareFinalizer(const SRef<IFinalize> & fifo)  override;
    virtual void signalRunning() = 0;

protected:
    std::atomic<bool> m_stop = false;
    std::atomic<bool> m_joinable = true;
    std::vector<SRef<IFinalize>> m_finalizers;
};

class XPCF_EXPORT_API BaseTask : virtual public AbstractTask
{
public:
    BaseTask(bool joinable = true): AbstractTask(joinable) {}
    virtual ~BaseTask() override;
    void start() override;
    void stop() override;
    void awaitRunning() override;
    void signalRunning() override;
    static void yield();

private:
    std::promise<void> m_startedPromise;
    std::thread m_thread;

};

class XPCF_EXPORT_API AbstractDelegateTask : public AbstractTask
{
public:
    AbstractDelegateTask(std::function<void(void)> processingFunction, bool joinable): AbstractTask (joinable),
        m_processFunc(processingFunction), m_stopPredicate( [&]() -> auto { return AbstractTask::stopped(); }) {}
    AbstractDelegateTask(std::function<void(void)> processingFunction, std::function<bool(void)> stopPredicate, bool joinable): AbstractTask (joinable),
        m_processFunc(processingFunction), m_stopPredicate( [&]() -> auto { return stopPredicate() | AbstractTask::stopped(); }) {}
    virtual ~AbstractDelegateTask() override = default;
    void process() override;
    bool stopped() const override { return m_stopPredicate(); }

protected:
    std::function<void(void)> m_processFunc;
    std::function<bool(void)> m_stopPredicate;
};

class XPCF_EXPORT_API DelegateTask : public AbstractDelegateTask
{
public:
    DelegateTask(std::function<void(void)> processingFunction, bool joinable = true): AbstractDelegateTask(processingFunction, joinable) {}
    DelegateTask(std::function<void(void)> processingFunction,std::function<bool(void)> stopPredicate, bool joinable):
        AbstractDelegateTask(processingFunction, stopPredicate, joinable) {}
    virtual ~DelegateTask() override = default;
    void start() override;
    void stop() override;
    void awaitRunning() override;
    void signalRunning() override;
    static void yield();

private:
    std::promise<void> m_startedPromise;
    std::future<void> m_startedFuture;
    std::thread m_thread;
};

#ifdef XPCF_USE_BOOST

class XPCF_EXPORT_API FiberTask : public AbstractDelegateTask
{
public:
    FiberTask(std::function<void(void)> processingFunction, bool joinable = true);
    virtual ~FiberTask() override;
    void start() override;
    void stop() override;
    void awaitRunning() override;
    void signalRunning() override;
    static void yield();

private:
    class FiberTaskImpl;
    UniqueRef<FiberTaskImpl> m_pimpl;
};

#endif
// TBB task ??

template <typename T>
SRef<ITask> createDelegateTask(std::function<void(void)> callable, bool joinable = true) {
    //TODO : every task based on delegate callable must have an abstract base class with a common constructor semantic
    static_assert(utils::is_base_of<AbstractDelegateTask, T>::value,
                  "Class type passed to createTask is not a derived class of AbstractDelegateTask !!");
    return utils::make_shared<T>(callable, joinable);
}

template <typename T>
SRef<ITask> createDelegateTask(std::function<void(void)> callable, std::function<bool(void)> stopPredicate, bool joinable) {
    //TODO : every task based on delegate callable must have an abstract base class with a common constructor semantic
    static_assert(utils::is_base_of<AbstractDelegateTask, T>::value,
                  "Class type passed to createTask is not a derived class of AbstractDelegateTask !!");
    return utils::make_shared<T>(callable,stopPredicate,joinable);
}


// Sample use:
// #define TASKMODEL DelegateTask or FiberTask
// std::function<void(void)> f = ...
// SRef<ITask> task = createDelegateTask<TASKMODEL>(f);

// TODOS:
// #define TASKMODELNSPACE fibers
// SRef<ITask> task = createTask<TASKMODEL>(f);
// using yield = TASKMODEL::yield;
// using mutex = TASKMODELNSPACE::mutex;
// Add boost::future (for wait_all/any...) and boost::fiber::future wrapping
// Add scheduling feature introspection and configuration (properties?)
// Add mutex type declaration upon FIFOs instanciation
// Add FIFOs finalize methods to end threading properly (through signal/slot?)
// fifo finalize slot should be signalled by stop() from the thread
// Needs to expose join() ?

}}}

#endif
