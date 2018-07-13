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

#ifndef ORG_BCOM_XPCF_ENUMERATOR_H
#define ORG_BCOM_XPCF_ENUMERATOR_H

#include "xpcf/collection/IEnumerator.h"
#include <vector>
#include <deque>

namespace org { namespace bcom { namespace xpcf {

template <class T, template<typename, typename> class S>
class Enumerator : public IEnumerator<T> {
public:
    static_assert(std::is_same<S<T, ::std::allocator<T>>, std::vector<T, ::std::allocator<T>>>::value ||
                  std::is_same<S<T, ::std::allocator<T>>, std::deque<T, ::std::allocator<T>>>::value,
                  "Sequence type not allowed for Enumerator");
    Enumerator() = delete;
    Enumerator(S<T, ::std::allocator<T>> & seq);
    virtual ~Enumerator() override = default;
    bool moveNext() override;
    void reset() override;
    T current() override;
    bool endReached() override;

    Iterator<T> begin() override;
    Iterator<T>  end() override;
    //    void unloadComponent() override final;

protected:
    virtual int64_t computeSize();
    S<T, ::std::allocator<T>> & m_sequence;
    int64_t m_index = -1;
    bool m_endReached = false;
};

template <class T, template<typename, typename> class S>
Enumerator<T, S>::Enumerator(S<T, ::std::allocator<T>> & seq):m_sequence(seq)
{
}

template <class T, template<typename, typename> class S>
inline int64_t Enumerator<T, S>::computeSize()
{
    return static_cast<int64_t>(m_sequence.size());
}

template <class T, template<typename, typename> class S>
inline bool Enumerator<T, S>::moveNext() {
    if (m_index < (computeSize() - 1)) {
        m_index++;
        return true;
    }
    m_endReached = true;
    return false;
}

template <class T, template<typename, typename> class S>
inline void Enumerator<T, S>::reset() {
    m_endReached = false;
    m_index =  -1;
}

template <class T, template<typename, typename> class S>
T Enumerator<T, S>::current() { return m_sequence.at(m_index) ; }

template <class T, template<typename, typename> class S>
bool Enumerator<T, S>::endReached() {
    return m_endReached;
}

template <class T, template<typename, typename> class S>
inline Iterator<T> Enumerator<T, S>::begin()
{
    reset();
    moveNext();
    return Iterator<T>(this);
}

template <class T, template<typename, typename> class S>
inline Iterator<T> Enumerator<T, S>::end()
{
    return Iterator<T>(this);
}

template <template<typename, typename> class S>
class Enumerator<const char *, S> : public IEnumerator<const char *> {
public:
    static_assert(std::is_same<S<std::string, ::std::allocator<std::string>>, std::vector<std::string, ::std::allocator<std::string>>>::value ||
                  std::is_same<S<std::string, ::std::allocator<std::string>>, std::deque<std::string, ::std::allocator<std::string>>>::value,
                  "Sequence type not allowed for Enumerator");
    Enumerator() = delete;
    Enumerator(S<std::string, ::std::allocator<std::string>> & seq);
    ~Enumerator() = default;
    bool moveNext() override;
    void reset() override;
    const char * current() override;
    bool endReached() override;

    Iterator<const char*> begin() override;
    Iterator<const char*> end() override;
    //    void unloadComponent() override final;

protected:
    int64_t computeSize();
    S<std::string, ::std::allocator<std::string>> & m_sequence;
    int64_t m_index = -1;
    bool m_endReached = false;
};

/*template <class T>
void Enumerator<T>::unloadComponent() { delete this;};*/
template <template<typename, typename> class S>
Enumerator<const char *, S>::Enumerator(S<std::string, ::std::allocator<std::string>> & seq):m_sequence(seq)
{
}

template <template<typename, typename> class S>
inline int64_t Enumerator<const char *, S>::computeSize()
{
    return static_cast<int64_t>(m_sequence.size());
}

template <template<typename, typename> class S>
inline bool Enumerator<const char *, S>::moveNext() {
    if (m_index < (computeSize() - 1)) {
        m_index++;
        return true;
    }
    m_endReached = true;
    return false;
}

template <template<typename, typename> class S>
inline void Enumerator<const char *, S>::reset() {
    m_endReached = false;
    m_index = -1;
}

template <template<typename, typename> class S>
const char * Enumerator<const char *, S>::current() { return m_sequence.at(m_index).c_str() ; }

template <template<typename, typename> class S>
bool Enumerator<const char *, S>::endReached() {
    return m_endReached;
}

template < template<typename, typename> class S>
inline Iterator<const char*> Enumerator<const char *, S>::begin()
{
    reset();
    moveNext();
    return Iterator<const char*>(this);
}

template <template<typename, typename> class S>
inline Iterator<const char*> Enumerator<const char *, S>::end()
{
    return Iterator<const char*>(this);
}

}}}

extern template class org::bcom::xpcf::Enumerator<org::bcom::xpcf::uuids::uuid, std::vector>;
#endif
