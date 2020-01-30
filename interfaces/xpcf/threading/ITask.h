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

#ifndef ORG_BCOM_XPCF_ITASK_H
#define ORG_BCOM_XPCF_ITASK_H

#include "xpcf/xpcf_api_define.h"
#include <xpcf/core/refs.h>
#include <xpcf/threading/SharedFifo.h>

#include <functional>

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API ITask
{
public:
	ITask() = default;
	virtual ~ITask() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void process() = 0;
    virtual void awaitRunning() = 0;
    virtual bool started() const = 0;
    virtual bool stopped() const = 0;
    virtual void cleanup() = 0;
    virtual void declareFinalizer(const SRef<IFinalize> & fifo) = 0;
};

class XPCF_EXPORT_API ITaskFactory
{
public:
    ITaskFactory() = default;
    virtual ~ITaskFactory() = default;
    virtual SRef<ITask> newTask(std::function<void(void)> command) = 0;
};

class XPCF_EXPORT_API IExecutor
{
public:
    IExecutor() = default;
    virtual ~IExecutor() = default;
    virtual void execute(std::function<void(void)> command) = 0;
};

}}}
#endif
