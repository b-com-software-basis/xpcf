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


#ifndef ORG_BCOM_XPCF_ICOLLECTION_H
#define ORG_BCOM_XPCF_ICOLLECTION_H

#include "xpcf/collection/IEnumerable.h"

namespace org { namespace bcom { namespace xpcf {

template <typename T>
class ICollection : virtual public IEnumerable<T> {
public:
    virtual ~ICollection() = default;
    virtual void add(T value) = 0;
    virtual void clear() = 0;
    virtual bool contains(T value) = 0;
    virtual bool remove(T value) = 0;
};

extern template class ICollection<uuids::uuid>;

}}}

#endif
