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
 * @date 2020-09-12
 */

#ifndef BASEBUFFER_H
#define BASEBUFFER_H

#include <vector>

namespace org { namespace bcom { namespace xpcf {

/**
 * @class BaseBuffer
 * @brief <B>A Buffer used to store any data such as descriptors.</B>
 */


class BaseBuffer {
public:
    BaseBuffer () = default;

    BaseBuffer (uint32_t size) {
        setSize(size);
    }

    /**
     * @brief BaseBuffer
     * copy initialise the buffer
     * @param buffer
     */
    /// TODO: add a owned/non-owned parameter/other constructor with no copy, just reference holding
    BaseBuffer (const std::vector<uint8_t> & buffer) {
        setSize(buffer.size());
        m_storageData.insert(m_storageData.begin(), buffer.data(), buffer.data() + m_bufferSize);
    }

    BaseBuffer (void* data, uint32_t size) {
       setData(data,size);
    }

    ~BaseBuffer () = default;

    void setSize (uint32_t size) {
        m_bufferSize = size;
        if (m_bufferSize == 0) { // invalid size
            return;
        }
        // The reserve + memset do no really create the elements.
        m_storageData.resize(m_bufferSize);
        /*
        m_storageData.reserve(m_bufferSize);
        memset(m_storageData.data(),0,m_bufferSize);
        */
    }

    uint32_t getSize () { return m_bufferSize; }

    void setData (void * data, uint32_t size) {
        if (m_bufferSize < size) {
            setSize(size);
        }
        m_storageData.insert(m_storageData.begin(), static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + m_bufferSize);
    }

    void appendData (const void * data, uint32_t size) {
        uint32_t endOffset = m_bufferSize;
        setSize(m_bufferSize + size);
        auto startIt = std::next(m_storageData.begin(), endOffset);
        m_storageData.insert(startIt, static_cast<const uint8_t *>(data), static_cast<const uint8_t *>(data) + size);
    }

    void* data () { return m_storageData.data(); }
    const void* data () const { return m_storageData.data(); }

    std::string encode();
    void decode(const std::string & str);

private:
    std::vector<uint8_t> m_storageData;
    uint32_t m_bufferSize = 0;

};

}}}

#endif // BASEBUFFER_H
