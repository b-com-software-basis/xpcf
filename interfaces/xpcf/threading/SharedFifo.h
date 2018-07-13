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

#ifndef ORG_BCOM_XPCF_SHAREDFIFO_HPP
#define ORG_BCOM_XPCF_SHAREDFIFO_HPP

#include <deque>
#include <condition_variable>
#include <mutex>

namespace org { namespace bcom { namespace xpcf {

template <class T>
class SharedFifo
{
public:
    SharedFifo() = default;
    ~SharedFifo() = default;
    virtual inline void push(const T & value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_data.push_front(value);
        m_nbNotified++;
        m_condQueueNotEmpty.notify_one();
    }

    inline const T back() {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_nbNotified == 0) {  // loop to avoid spurious wakeups
            m_condQueueNotEmpty.wait(lock);
        }
        return m_data.back();
    }

    inline const T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_nbNotified == 0) {  // loop to avoid spurious wakeups
            m_condQueueNotEmpty.wait(lock);
        }
        T value;
        doPop(value);
        return value;
    }

    inline bool tryPop( T & value )
    {
        std::unique_lock<std::mutex> tryLock(m_mutex, std::defer_lock);
        bool popSuccessful = false;

        if( tryLock.try_lock() )
        {
            if( m_nbNotified > 0)
            {
                doPop(value);
                popSuccessful = true;
            }
        }
        return popSuccessful;
    }

    inline unsigned int size() const {
        return m_data.size();
    }

    inline bool empty() const {
        return m_data.empty();
    }

protected:
    std::condition_variable m_condQueueNotEmpty;
    unsigned long m_nbNotified = 0;
    std::deque<T> m_data;
    std::mutex m_mutex;

    virtual inline void doPop( T& value )
    {
        value = m_data.back();
        m_data.pop_back();
        m_nbNotified--;
    }
};

}}}

#endif
