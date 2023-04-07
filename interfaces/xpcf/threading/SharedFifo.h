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
#include "xpcf/core/BaseException.h"

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

    virtual void clear() = 0;
};

template <class MutexClass, class ConditionVariableClass, class CVStatusClass>//, class TaskClass >
struct TemplatedThreadedNamespace {
    typedef MutexClass MutexType;
    typedef ConditionVariableClass ConditionVariableType;
    typedef CVStatusClass CVStatusType;
//    typedef TaskClass TaskType;
};

using StdThreadedNamespace = TemplatedThreadedNamespace<std::mutex, std::condition_variable, std::cv_status>;

template<typename T, typename = void>
struct FifoCleaner
{
    static void cleanup([[maybe_unused]] const std::deque<T> & data) {}
};

template<typename T>
struct FifoCleaner<T, std::enable_if_t< std::is_pointer_v<T>>>
{
    static void cleanup(const std::deque<T> & data) {
        for (auto element: data) {
            delete element;
        }
    }
};

//template <class T, class M = std::mutex, class CV = std::condition_variable>
template <class T, class NS = StdThreadedNamespace>
class SharedFifo : public IFifo<T>
{
public:
    SharedFifo() = default;
    ~SharedFifo() = default;
    SharedFifo(const SharedFifo & other) {
        std::unique_lock<typename NS::MutexType> lock(const_cast<SharedFifo&>(other).m_mutex);
        m_nbNotified = other.m_nbNotified;
        m_data.assign(other.m_data.begin(),other.m_data.end());
        m_bReleased = other.m_bReleased;
    }

    virtual void push(const T & value) override {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        m_data.push_front(value);
        m_nbNotified++;
        m_condQueueNotEmpty.notify_one();
    }

    bool back(T & value) override {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        while (!m_bReleased && (m_nbNotified == 0)) {  // loop to avoid spurious wakeups
            m_condQueueNotEmpty.wait(lock);
        }
        if (m_nbNotified != 0) {
            value = m_data.back();
        }
        return m_bReleased;
    }

    const T back() override {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        while (!m_bReleased && (m_nbNotified == 0)) {  // loop to avoid spurious wakeups
            m_condQueueNotEmpty.wait(lock);
        }
        if (m_bReleased && (m_nbNotified == 0)) {
            throw Exception("IFifo::back aborted");
        }
        return m_data.back();
    }

    const T pop() override {
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

    bool pop(T & value) override {
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

    bool tryPop( T & value ) override
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

    bool pop( T & value , std::chrono::milliseconds duration) override
    {
        typename NS::CVStatusType status = NS::CVStatusType::no_timeout;
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        while (!m_bReleased && m_nbNotified == 0 && status != NS::CVStatusType::timeout) {  // loop to avoid spurious wakeups
            status = m_condQueueNotEmpty.wait_for(lock,duration);
        }
        if ((status != NS::CVStatusType::no_timeout || m_bReleased) && (m_nbNotified == 0) ) {
            return false;
        }
        doPop(value);
        return true;
    }

    std::size_t size() const {
        return m_data.size();
    }

    bool empty() const override {
        return m_data.empty();
    }

    void release() override {
        m_bReleased = true;
        m_condQueueNotEmpty.notify_all();
    }

   void clear() override {
        FifoCleaner<T>::cleanup(m_data);
        m_nbNotified = 0;
        m_data.clear();
    }

    inline void for_each(const std::function<void(T)> & f) {
        // CAUTION : no synchro for now : to study
        for (auto element: m_data) {
            f(element);
        }
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

#ifndef SWIG
template <template<typename, typename> class FIFO, class T, class NS = StdThreadedNamespace, typename ... Args2> SRef<IFifo<T>> createFifo(Args2&&... args) {
    return utils::make_shared<FIFO<T,NS>>(std::forward< Args2 >(args)...);
}
#endif

}}}

#endif
