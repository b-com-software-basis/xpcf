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

#ifndef ORG_BCOM_XPCF_SHAREDCIRCULARBUFFER_HPP
#define ORG_BCOM_XPCF_SHAREDCIRCULARBUFFER_HPP

#include "xpcf/threading/SharedFifo.h"

namespace org { namespace bcom { namespace xpcf {

template <class T, class NS = StdThreadedNamespace>
class SharedCircularBuffer  : public SharedFifo<T,NS>
{
public:
    SharedCircularBuffer( unsigned int size )
        : SharedFifo<T,NS>()
        , m_pushCursor( 0 )
        , m_popCursor( 0 )
    {
        m_data = std::deque<T>( size );
    }

    ~SharedCircularBuffer() = default;

    virtual void push(const T & value) override
    {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        m_data[m_pushCursor] = value;
        increaseCursor( m_pushCursor );
        ++m_nbNotified;

        if( m_nbNotified > m_data.size() )
        {
            increaseCursor(m_popCursor);
            --m_nbNotified;
        }

        m_condQueueNotEmpty.notify_one();
    }

protected:
    using SharedFifo<T,NS>::m_condQueueNotEmpty;
    using SharedFifo<T,NS>::m_nbNotified;
    using SharedFifo<T,NS>::m_data;
    using SharedFifo<T,NS>::m_mutex;

    std::size_t m_pushCursor;
    std::size_t m_popCursor;

    inline void increaseCursor( std::size_t& cursor )
    {
        ++cursor;
        if( cursor == m_data.size() )
        {
            cursor = 0;
        }
    }

    virtual void doPop( T& value )
    {
        value = m_data[m_popCursor];
        increaseCursor(m_popCursor);
        --m_nbNotified;
    }
};

}}}

#endif //__SHAREDCIRCULARBUFFER_HPP__
