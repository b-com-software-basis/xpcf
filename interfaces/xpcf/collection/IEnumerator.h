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

namespace org { namespace bcom { namespace xpcf {

template <class T>
class IEnumerator;
// FOR range-based for usage ONLY
template <class T>
class Iterator {
public:
    Iterator(IEnumerator<T> * enumerator):m_enumerator(enumerator) {}
    Iterator(const Iterator<T> & it) {
        this->m_enumerator = it.m_enumerator;
    }

    Iterator(const Iterator<T> && it) {
        this->m_enumerator = it.m_enumerator;
    }

    inline bool operator !=(Iterator<T> & it) {
        //check underlying enumerator is the same, i.e. begin and end iterators come from the same enumerator
        if (!(*this == it)) {
            return true;
        }
        return !m_enumerator->endReached();
    }

    inline bool operator ==(Iterator<T> & it) {
        //check underlying enumerator is the same, i.e. begin and end iterators come from the same enumerator
        if (it.m_enumerator == m_enumerator) {
            return true;
        }
        return false;
    }

    inline T operator*() {
        return m_enumerator->current();
    }

    inline void operator++() {
        m_enumerator->moveNext();
    }

    inline void operator = (const Iterator<T> & it) {
        this->m_endReached = it.m_endReached;
        this->m_enumerator = it.m_enumerator;
    }

private:
    IEnumerator<T> * m_enumerator;
};

template <class T>
class IEnumerator { //: public virtual IComponentIntrospect {
public:
    virtual ~IEnumerator() = default;
    virtual bool moveNext() = 0;
    virtual void reset() = 0;
    virtual T current() = 0;
    virtual bool endReached() = 0;
    virtual Iterator<T> begin() = 0;
    virtual Iterator<T> end() = 0;

};

}}}

#endif
