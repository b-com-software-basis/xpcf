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


#ifndef ORG_BCOM_XPCF_IENUMERATOR_H
#define ORG_BCOM_XPCF_IENUMERATOR_H
#include <iostream>

#include "xpcf/core/refs.h"

namespace org { namespace bcom { namespace xpcf {

template <typename T>
class IEnumerator {
public:
    virtual ~IEnumerator() = default;
    virtual bool moveNext() = 0;
    virtual void reset() = 0;
    virtual T current() = 0;
    virtual bool endReached() = 0;
    virtual bool operator ==(IEnumerator<T> & it) const = 0;
};

template <typename T>
class Iterator {
public:
    Iterator(SRef<IEnumerator<T>> enumerator):m_enumerator(enumerator) {}
    Iterator(const Iterator<T> & it) = default;
    Iterator(Iterator<T> && it) = default;
    Iterator & operator=(const Iterator & other) = default;
    Iterator & operator=(Iterator && other) = default;

    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag ;
    using pointer = T*;
    using reference = T&;

       difference_type operator- (Iterator const &rhs) const
       {
         return 0;//sm_it - rhs.m_it;
       }
    inline bool operator !=(const Iterator<T> & it) {
      return !m_enumerator->endReached();
    }

    inline bool operator ==(const Iterator<T> & it) {
      return m_enumerator == it.m_enumerator;
    }

    inline T operator*() {
        return m_enumerator->current();
    }

    inline void operator++() {
        m_enumerator->moveNext();
    }

private:
    SRef<IEnumerator<T>> m_enumerator;
};

template <typename T>
Iterator<T> begin(SRef<IEnumerator<T>> ref)
{
    ref->reset();
    ref->moveNext();
    return Iterator<T>(ref);
}

template <typename T>
Iterator<T> end(SRef<IEnumerator<T>> ref)
{
    return Iterator<T>(ref);
}

}}}

#endif
