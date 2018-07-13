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

#include <thread>
#include <functional>

namespace org { namespace bcom { namespace xpcf {

class BaseTask : public ITask
{
public:
    BaseTask() = default;
    virtual ~BaseTask() override = default;
    void start() override;
    void stop() override;
    virtual void process() = 0;
    inline bool stopped() const override final { return m_stop; }

protected:
    std::thread m_thread;
    bool m_stop = false;
};

class DelegateTask : public BaseTask
{
public:
    DelegateTask(std::function<void(void)> processingFunction) :m_processFunc(processingFunction) {}
    virtual ~DelegateTask() override = default;
    void process() override;

protected:
    std::function<void(void)> m_processFunc;
};

}}}

#endif
