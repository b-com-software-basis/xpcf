/**
 * @copyright Copyright (c) 2017 B-com http://www.b-com.com/
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
 * @brief description of file
 * @date 2017-12-11
 */


#ifndef ORG_BCOM_XPCF_IENUMERABLE_H
#define ORG_BCOM_XPCF_IENUMERABLE_H
#include "xpcf/collection/IEnumerator.h"
#include "xpcf/core/refs.h"
#include "xpcf/core/uuid.h"

namespace org { namespace bcom { namespace xpcf {

template <typename T>
class IEnumerable
{
public:
    virtual ~IEnumerable() = default;
    virtual SRef<IEnumerator<T>> getEnumerator() const  = 0;
    virtual SRef<IEnumerator<T>> getEnumerator(uint32_t offset, uint32_t chunkSize) const = 0;
    virtual uint32_t size() const = 0;
};


template <typename T>
const Iterator<T> begin(const IEnumerable<T> & ref)
{
    return begin(ref.getEnumerator());
}

template <typename T>
const Iterator<T> end(const IEnumerable<T> & ref)
{
    return end(ref.getEnumerator());
}

template <typename T>
Iterator<T> begin(IEnumerable<T> & ref)
{
    return begin(ref.getEnumerator());
}

template <typename T>
Iterator<T> end(IEnumerable<T> & ref)
{
    return end(ref.getEnumerator());
}

extern template class IEnumerable<uuids::uuid>;

}}}

#endif
