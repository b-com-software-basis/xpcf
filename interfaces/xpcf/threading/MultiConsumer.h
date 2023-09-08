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

#ifndef ORG_BCOM_XPCF_MULTICONSUMER_HPP
#define ORG_BCOM_XPCF_MULTICONSUMER_HPP

#include <deque>
#include <condition_variable>
#include <mutex>
#include "xpcf/core/refs.h"
#include "xpcf/core/BaseException.h"
#include "xpcf/threading/SharedBuffer.h"

namespace org { namespace bcom { namespace xpcf {

/**
 * @class MultiConsumer
 * @brief Specifies the MultiConsumer class.
 *
 * This class  allows several consumers to get data from a unique synchronized queue (i.e. any class derived from IFifo).
 * Each item in the source queue is popped once all consumers have popped its value.
 */
template <typename T, typename NS = StdThreadedNamespace>
class MultiConsumer : public IFinalize {
public:
    MultiConsumer(size_t nbConsumers, const SRef<IFifo<T>> & fifo): m_fifo(fifo),m_nbConsumers(nbConsumers) {
        m_consumersBuffers.resize(nbConsumers);
    }

    void release() override {
        for (auto & buffer : m_consumersBuffers) {
            buffer.release();
        }
    }

    uint64_t subscribe() {
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        if (m_currentSubscriberID< m_nbConsumers) {
            return m_currentSubscriberID++;
        }
        throw xpcf::Exception("all consumers already subscribed !!!");
    }

    void init()
    {
        T value;
        m_nbConsumed = 0;
        m_fifo->pop(value);
        for (auto & buffer : m_consumersBuffers) {
            buffer.push(value);
        }
    }

    T pop(uint64_t consumerID)
    {
        T value;
        if (consumerID >= m_nbConsumers) {
            throw xpcf::Exception("Invalid consumer ID");
        }

        {
            std::unique_lock<typename NS::MutexType> lock(m_mutex);
            if (m_bFirstInit) {
                init();
                m_bFirstInit = false;
            }
        }

        value = m_consumersBuffers.at(consumerID).pop();
        std::unique_lock<typename NS::MutexType> lock(m_mutex);
        m_nbConsumed++;
        if (m_nbConsumed == m_nbConsumers) {
            init();
        }
        return value;
    }

private:
    typename NS::MutexType m_mutex;
    uint64_t m_currentSubscriberID = 0;
    size_t m_nbConsumers;
    SRef<IFifo<T>> m_fifo;
    std::vector<SharedFifo<T>> m_consumersBuffers;
    std::atomic<uint64_t> m_nbConsumed = 0;
    std::atomic<bool> m_bFirstInit = true;

};

}}}

#endif
