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

#ifndef ORG_BCOM_XPCF_SHAREDBUFFER_HPP
#define ORG_BCOM_XPCF_SHAREDBUFFER_HPP

#include "xpcf/threading/SharedFifo.h"

namespace org { namespace bcom { namespace xpcf {

template <class T, class NS = StdThreadedNamespace>
class SharedBuffer : public SharedFifo<T,NS>
{
public:
    SharedBuffer(std::size_t max, std::size_t startAt = 0) :
        m_maxSize(max),
        m_bufferedStartOffset( max < startAt ? max : startAt),
        m_started(startAt == 0 ? true : false) {
        m_data.resize(m_maxSize);
    }
    
    ~SharedBuffer() = default;
    
    virtual inline void push(const T & value) override {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);

        while(m_nbNotified >= m_maxSize) {
            m_condQueueNotFull.wait(lock);
        }
        m_data.push_front(value);
        m_nbNotified++;
        
        if (m_started) {
            m_condQueueNotEmpty.notify_one();
        }
        else {
            if (m_nbNotified >= m_bufferedStartOffset) {
                m_started = true;
            }
        }
    }

private:
    std::size_t m_maxSize;
    std::size_t m_bufferedStartOffset;
    bool m_started = false;
    typename NS::ConditionVariableType m_condQueueNotFull;
    using SharedFifo<T,NS>::m_condQueueNotEmpty;
    using SharedFifo<T,NS>::m_nbNotified;
    using SharedFifo<T,NS>::m_data;
    using SharedFifo<T,NS>::m_mutex;

    virtual inline void doPop( T& value ) override
    {
        SharedFifo<T,NS>::doPop(value);
        m_condQueueNotFull.notify_one();
    }
};

}}}

#endif
