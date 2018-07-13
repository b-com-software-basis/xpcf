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

#ifndef ORG_BCOM_XPCF_COLLECTION_H
#define ORG_BCOM_XPCF_COLLECTION_H

#include "xpcf/collection/ICollection.h"
#include "Enumerator.h"
#include <algorithm>

namespace org { namespace bcom { namespace xpcf {

template <class T, template<typename, typename> class S>
class Collection : public ICollection<T> {
public:
    static_assert(std::is_same<S<T, ::std::allocator<T>>, std::vector<T, ::std::allocator<T>>>::value ||
                  std::is_same<S<T, ::std::allocator<T>>, std::deque<T, ::std::allocator<T>>>::value,
                  "Sequence type not allowed for Collection");
    Collection() { m_enumerator = unixpcf::make_unique<Enumerator<T,S>>(m_sequence); }
    ~Collection() override = default;

    //ICollection
    void add(T value) override;
    void clear() override;
    bool contains(T value) override;
    bool remove(T value) override;

    // IEnumerable
    Iterator<T> begin() override;
    Iterator<T>  end() override;
    const Iterator<T>  begin() const override;
    const Iterator<T>  end() const override;

    uint32_t size() const override;

private:
    S<T, ::std::allocator<T>> m_sequence;
    UniqueRef<IEnumerator<T>> m_enumerator;
};

template <class T, template<typename, typename> class S>
inline void Collection<T, S>::add(T value) {
    m_sequence.push_back(value);
}

template <class T, template<typename, typename> class S>
inline void Collection<T, S>::clear() { m_sequence.clear(); }

template <class T, template<typename, typename> class S>
inline bool Collection<T, S>::contains(T value)
{
    return std::find(m_sequence.begin(), m_sequence.end(), value) != m_sequence.end();
}

template <class T, template<typename, typename> class S>
inline bool Collection<T, S>::remove(T value)
{
    if ( !contains(value) ) {
        return false;
    }

    m_sequence.erase(std::remove(m_sequence.begin(), m_sequence.end(), value), m_sequence.end());
    return true;
}

template <class T, template<typename, typename> class S>
inline Iterator<T>  Collection<T, S>::begin()
{
    m_enumerator->reset();
    m_enumerator->moveNext();
    return Iterator<T>(m_enumerator.get());
}

template <class T, template<typename, typename> class S>
inline Iterator<T>  Collection<T, S>::end()
{
    return Iterator<T>(m_enumerator.get());
}

template <class T, template<typename, typename> class S>
inline const Iterator<T>  Collection<T, S>::begin() const
{
    m_enumerator->reset();
    m_enumerator->moveNext();
    return Iterator<T>(m_enumerator.get());
}

template <class T, template<typename, typename> class S>
inline const Iterator<T>  Collection<T, S>::end() const
{
    return Iterator<T>(m_enumerator.get());
}


template <class T, template<typename, typename> class S>
inline uint32_t Collection<T, S>::size() const
{
    return m_sequence.size();
}

template <template<typename, typename> class S>
class Collection<const char *, S> : public ICollection<const char *> {
public:
    static_assert(std::is_same<S<std::string, ::std::allocator<std::string>>, std::vector<std::string, ::std::allocator<std::string>>>::value ||
                  std::is_same<S<std::string, ::std::allocator<std::string>>, std::deque<std::string, ::std::allocator<std::string>>>::value,
                  "Sequence type not allowed for Collection");
    Collection() { m_enumerator = unixpcf::make_unique<Enumerator<const char *,S>>(m_sequence); }
    ~Collection() override = default;

    //ICollection
    void add(const char * value) override;
    void clear() override;
    bool contains(const char * value) override;
    bool remove(const char * value) override;

    // IEnumerable
    Iterator<const char*> begin() override;
    Iterator<const char*> end() override;
    const Iterator<const char*> begin() const override;
    const Iterator<const char*> end() const override;

    uint32_t size() const override;

private:
    S<std::string, ::std::allocator<std::string>> m_sequence;
    UniqueRef<IEnumerator<const char *>> m_enumerator;
};

template <template<typename, typename> class S>
inline void Collection<const char *, S>::add(const char * value) {
    m_sequence.push_back(value);
}

template <template<typename, typename> class S>
inline void Collection<const char *, S>::clear() { m_sequence.clear(); }

template <template<typename, typename> class S>
inline bool Collection<const char *, S>::contains(const char * value)
{
    std::string strValue = value;
    return std::find(m_sequence.begin(), m_sequence.end(), strValue) != m_sequence.end();
}

template <template<typename, typename> class S>
inline bool Collection<const char *, S>::remove(const char * value)
{
    if ( !contains(value) ) {
        return false;
    }
    std::string strValue = value;
    m_sequence.erase(std::remove(m_sequence.begin(), m_sequence.end(), strValue), m_sequence.end());
    return true;
}

template < template<typename, typename> class S>
inline Iterator<const char*> Collection<const char *, S>::begin()
{
    m_enumerator->reset();
    m_enumerator->moveNext();
    return Iterator<const char*>(m_enumerator.get());;
}

template <template<typename, typename> class S>
inline Iterator<const char*> Collection<const char *, S>::end()
{
    return Iterator<const char*>(m_enumerator.get());
}

template < template<typename, typename> class S>
inline const Iterator<const char*> Collection<const char *, S>::begin() const
{
    m_enumerator->reset();
    m_enumerator->moveNext();
    return Iterator<const char*>(m_enumerator.get());
}

template <template<typename, typename> class S>
inline  const Iterator<const char*> Collection<const char *, S>::end() const
{
    return Iterator<const char*>(m_enumerator.get());
}

template <template<typename, typename> class S>
inline uint32_t Collection<const char *, S>::size() const
{
    return m_sequence.size();
}

extern template class Collection<uuids::uuid, std::vector>;
}}}

#endif
