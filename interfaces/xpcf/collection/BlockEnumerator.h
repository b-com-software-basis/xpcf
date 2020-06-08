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

#ifndef ORG_BCOM_XPCF_BlockEnumerator_H
#define ORG_BCOM_XPCF_BlockEnumerator_H

#include "Enumerator.h"

namespace org { namespace bcom { namespace xpcf {

template <class T, template<typename, typename> class S>
class BlockEnumerator : public Enumerator<T,S> {
public:
    static_assert(std::is_same<S<T, ::std::allocator<T>>, std::vector<T, ::std::allocator<T>>>::value ||
                  std::is_same<S<T, ::std::allocator<T>>, std::deque<T, ::std::allocator<T>>>::value,
                  "Sequence type not allowed for BlockEnumerator");
    BlockEnumerator() = delete;
    BlockEnumerator(S<T, ::std::allocator<T>> & seq, uint32_t offset, uint32_t size);
    ~BlockEnumerator() override = default;
    void reset() override;

private:
    int64_t computeSize() override;
    using Enumerator<T,S>::m_sequence;
    using Enumerator<T,S>::m_index;
    using Enumerator<T,S>::m_endReached;
    uint32_t m_offset = 0;
    uint32_t m_size;
};

template <class T, template<typename, typename> class S>
BlockEnumerator<T, S>::BlockEnumerator(S<T, ::std::allocator<T>> & seq, uint32_t offset, uint32_t chunkSize):Enumerator<T,S>(seq),m_offset(offset)
{
    m_size = m_offset + chunkSize;
    if (m_sequence.size() < m_size) {
        throw std::out_of_range("offset + size > sequence size");
    }
    m_index = m_offset - static_cast<int64_t>(1);
}

template <class T, template<typename, typename> class S>
inline int64_t BlockEnumerator<T, S>::computeSize()
{
    return static_cast<int64_t>(m_size);
}


template <class T, template<typename, typename> class S>
inline void BlockEnumerator<T, S>::reset() {
    m_endReached = false;
    m_index = m_offset - static_cast<int64_t>(1);
}


template <template<typename, typename> class S>
class BlockEnumerator<const char *, S> : public Enumerator<const char *,S> {
public:
    static_assert(std::is_same<S<std::string, ::std::allocator<std::string>>, std::vector<std::string, ::std::allocator<std::string>>>::value ||
                  std::is_same<S<std::string, ::std::allocator<std::string>>, std::deque<std::string, ::std::allocator<std::string>>>::value,
                  "Sequence type not allowed for BlockEnumerator");
    BlockEnumerator() = delete;
    BlockEnumerator(S<std::string, ::std::allocator<std::string>> & seq, uint32_t offset, uint32_t size);
    ~BlockEnumerator() override = default;
    void reset() override;

private:
    int64_t computeSize() override;
    using Enumerator<const char *,S>::m_sequence;
    using Enumerator<const char *,S>::m_index;
    using Enumerator<const char *,S>::m_endReached;
    uint32_t m_offset = 0;
    uint32_t m_size;
};

template <template<typename, typename> class S>
BlockEnumerator<const char *, S>::BlockEnumerator(S<std::string, ::std::allocator<std::string>> & seq, uint32_t offset, uint32_t chunkSize):Enumerator<const char *,S>(seq),m_offset(offset)
{
    m_size = m_offset + chunkSize;
    if (m_sequence.size() < m_size) {
        throw std::out_of_range("offset + size > sequence size");
    }
    m_index = m_offset - static_cast<int64_t>(1);
}

template <template<typename, typename> class S>
inline int64_t BlockEnumerator<const char *, S>::computeSize()
{
    return static_cast<int64_t>(m_size);
}

template <template<typename, typename> class S>
inline void BlockEnumerator<const char *, S>::reset() {
    m_endReached = false;
    m_index = m_offset - static_cast<int64_t>(1);
}

}}}

#endif
