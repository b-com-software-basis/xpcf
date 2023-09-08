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

#ifndef ORG_BCOM_XPCF_FIBERFIFOS_H
#define ORG_BCOM_XPCF_FIBERFIFOS_H

#ifdef XPCF_USE_BOOST
#include "xpcf/threading/SharedCircularBuffer.h"
#include "xpcf/core/refs.h"
#include "boost/fiber/mutex.hpp"
#include "boost/fiber/condition_variable.hpp"

namespace org { namespace bcom { namespace xpcf {

//template <template<typename, typename, typename> class FIFO, class T> SRef<IFifo<T>> createFiberFifo() {
//    return utils::make_shared<FIFO<T,utils::fibers::mutex,utils::fibers::condition_variable>>();
//}
#ifdef XPCF_USE_BOOST
using FiberThreadedNamespace = TemplatedThreadedNamespace<utils::fibers::mutex, utils::fibers::condition_variable, utils::fibers::cv_status>;
#else
using FiberThreadedNamespace = TemplatedThreadedNamespace<std::mutex, std::condition_variable, std::cv_status>;
#endif
template <template<typename, typename> class FIFO, class T> SRef<IFifo<T>> createFiberFifo() {
    return utils::make_shared<FIFO<T,FiberThreadedNamespace>>();
}
/*
template <class T> SRef<IFifo<T>> createFiberFifo() {
    return utils::make_shared<SharedFifo<T,utils::fibers::mutex>>();
}*/

}}}

#endif

#endif
