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

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API AbstractTask : public virtual ITask
{
public:
    AbstractTask() = default;
    virtual ~AbstractTask() override = default;
    inline bool stopped() const override final { return m_stop; }
    virtual void signalStarted() = 0;

protected:
    bool m_stop = false;
    bool m_started = false;
};

class XPCF_EXPORT_API BaseTask : public virtual AbstractTask
{
public:
    BaseTask() = default;
    virtual ~BaseTask() override = default;
    void start() override;
    void stop() override;
    void awaitStart() override;
    void signalStarted() override;
    static void yield();

protected:
    std::promise<void> m_startedPromise;
    std::thread m_thread;

};

class XPCF_EXPORT_API AbstractDelegateTask : public virtual AbstractTask
{
public:
    AbstractDelegateTask(std::function<void(void)> processingFunction) :m_processFunc(processingFunction) {}
    virtual ~AbstractDelegateTask() override = default;
    void process() override;

protected:
    std::function<void(void)> m_processFunc;
};

class XPCF_EXPORT_API DelegateTask : public BaseTask, public AbstractDelegateTask
{
public:
    DelegateTask(std::function<void(void)> processingFunction) :AbstractDelegateTask(processingFunction) {}
    virtual ~DelegateTask() override = default;
    static void yield();

};

class XPCF_EXPORT_API FiberTask : public AbstractDelegateTask
{
public:
    FiberTask(std::function<void(void)> processingFunction);
    virtual ~FiberTask() override;
    void start() override;
    void stop() override;
    void awaitStart() override;
    void signalStarted() override;
    static void yield();

private:
    class FiberTaskImpl;
    UniqueRef<FiberTaskImpl> m_pimpl;
};

// TBB task ??

template <typename T>
SRef<ITask> createDelegateTask(std::function<void(void)> callable) {
    //TODO : every task based on delegate callable must have an abstract base class with a common constructor semantic
    static_assert(utils::is_base_of<AbstractDelegateTask, T>::value,
                  "Class type passed to createTask is not a derived class of AbstractDelegateTask !!");
    return utils::make_shared<T>(callable);
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
