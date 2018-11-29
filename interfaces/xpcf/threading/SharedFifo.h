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
#include "xpcf/core/refs.h"
#include "xpcf/core/Exception.h"

namespace org { namespace bcom { namespace xpcf {

class IFinalize {
public:
    IFinalize() = default;
    virtual ~IFinalize() = default;
    virtual void release() = 0 ;
};

template <class T>
class IFifo : public IFinalize
{
public:
    IFifo() = default;
    virtual ~IFifo() = default;
    virtual inline void push(const T & value) = 0;

    virtual const T back() = 0 ;

    virtual bool back(T & value) = 0 ;

    virtual const T pop() = 0 ;

    virtual bool pop(T & value) = 0 ;

    virtual bool tryPop( T & value ) = 0;

    virtual bool pop( T & value , std::chrono::milliseconds duration) = 0;

    virtual bool empty() const = 0 ;
};

template <class MutexClass, class ConditionVariableClass, class CVStatusClass>//, class TaskClass >
struct TemplatedThreadedNamespace {
    typedef MutexClass MutexType;
    typedef ConditionVariableClass ConditionVariableType;
    typedef CVStatusClass CVStatusType;
//    typedef TaskClass TaskType;
};

using StdThreadedNamespace = TemplatedThreadedNamespace<std::mutex, std::condition_variable, std::cv_status>;

//template <class T, class M = std::mutex, class CV = std::condition_variable>
template <class T, class NS = StdThreadedNamespace>
class SharedFifo : public IFifo<T>
{
public:
    SharedFifo() = default;
    ~SharedFifo() = default;
    virtual inline void push(const T & value) override {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        m_data.push_front(value);
        m_nbNotified++;
        m_condQueueNotEmpty.notify_one();
    }

    inline bool back(T & value) override {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        while (!m_bReleased && (m_nbNotified == 0)) {  // loop to avoid spurious wakeups
            m_condQueueNotEmpty.wait(lock);
        }
        if (m_nbNotified != 0) {
            value = m_data.back();
        }
        return m_bReleased;
    }

    inline const T back() override {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        while (!m_bReleased && (m_nbNotified == 0)) {  // loop to avoid spurious wakeups
            m_condQueueNotEmpty.wait(lock);
        }
        if (m_bReleased && (m_nbNotified == 0)) {
            throw Exception("IFifo::back aborted");
        }
        return m_data.back();
    }

    inline const T pop() override {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        while (!m_bReleased && (m_nbNotified == 0)) {  // loop to avoid spurious wakeups
            m_condQueueNotEmpty.wait(lock);
        }
        if (m_bReleased && (m_nbNotified == 0)) {
            throw Exception("IFifo::pop aborted");
        }
        T value;
        doPop(value);
        return value;
    }

    inline bool pop(T & value) override {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        while (!m_bReleased && (m_nbNotified == 0)) {  // loop to avoid spurious wakeups
            m_condQueueNotEmpty.wait(lock);
        }
        if (m_bReleased && (m_nbNotified == 0)) {
            return false;
        }
        doPop(value);
        return true;
    }

    inline bool tryPop( T & value ) override
    {
        std::unique_lock<typename NS::MutexType> tryLock(m_mutex, std::defer_lock);
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

    inline bool pop( T & value , std::chrono::milliseconds duration) override
    {
        typename NS::CVStatusType status = NS::CVStatusType::no_timeout;
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        while (!m_bReleased && m_nbNotified == 0 && status != NS::CVStatusType::timeout) {  // loop to avoid spurious wakeups
            status = m_condQueueNotEmpty.wait_for(lock,duration);
        }
        if (status == NS::CVStatusType::no_timeout ) {
            doPop(value);
        }
        return (status == NS::CVStatusType::no_timeout) && !m_bReleased;
    }

    inline std::size_t size() const {
        return m_data.size();
    }

    inline bool empty() const override {
        return m_data.empty();
    }

    inline void release() override {
        m_bReleased = true;
        m_condQueueNotEmpty.notify_all();
    }

protected:
    typename NS::ConditionVariableType m_condQueueNotEmpty;
    std::size_t m_nbNotified = 0;
    std::deque<T> m_data;
    volatile bool m_bReleased = false;
    typename NS::MutexType m_mutex;

    virtual inline void doPop( T& value )
    {
        value = m_data.back();
        m_data.pop_back();
        m_nbNotified--;
    }
};

template <class T> SRef<IFifo<T>> createFifo() {
    return utils::make_shared<SharedFifo<T>>();
}

template <template<typename, typename> class FIFO, class T, class NS = StdThreadedNamespace> SRef<IFifo<T>> createFifo() {
    return utils::make_shared<FIFO<T,NS>>();
}

}}}

#endif
