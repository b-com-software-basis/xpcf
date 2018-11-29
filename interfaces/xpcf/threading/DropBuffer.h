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

#ifndef ORG_BCOM_XPCF_DROPBUFFER_H
#define ORG_BCOM_XPCF_DROPBUFFER_H

#include "xpcf/threading/SharedLifo.h"


namespace org { namespace bcom { namespace xpcf {

template <class T, class NS = StdThreadedNamespace>
class DropBuffer : public SharedLifo<T,NS>
{
public:
    DropBuffer() = default;
    ~DropBuffer() = default;

    virtual inline void push(const T & value) override {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        if (m_nbNotified > 0) {
            m_data.clear();
            m_nbNotified = 0;
        }
        m_data.push_front(value);
        m_nbNotified++;
        m_condQueueNotEmpty.notify_one();
    }

protected:
    using SharedFifo<T,NS>::m_condQueueNotEmpty;
    using SharedFifo<T,NS>::m_nbNotified;
    using SharedFifo<T,NS>::m_data;
    using SharedFifo<T,NS>::m_mutex;


    virtual inline void doPop( T& value )
    {
        value = m_data.front();
        m_data.pop_front();
        m_nbNotified--;
    }
};

template < class T, class NS = StdThreadedNamespace> SRef<IFifo<T>> createDropBuffer() {
    return utils::make_shared<DropBuffer<T,NS>>();
}

}}}

#endif
