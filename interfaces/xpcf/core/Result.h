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
 * @date 2017-12-05
 */

#ifndef ORG_BCOM_XPCF_RESULT_H
#define ORG_BCOM_XPCF_RESULT_H


#include "xpcf/core/XPCFErrorCode.h"

#include <string>

namespace org { namespace bcom { namespace xpcf {

template <class E> bool hasError(E e){};

template <> inline bool hasError(XPCFErrorCode e)
{
    if (static_cast<long>(e) < 0) {
        return true;
    }
    return false;
}

/**
 * @brief Specifies the result template class
 * @tparam T : the type value the result class will handle
 */
template<class T, class E>
class result
{
public:
    result(std::string contextName, E errCode,
           std::string message = "FAILED"): m_context(contextName) { setResult(errCode, message); }
    result(T value, std::string contextName, E errCode,
           std::string message = "FAILED"):m_context(contextName), m_value(value) { setResult(errCode, message); }
    ~result() {}
    inline bool hasError() const { if (static_cast<long>(m_result.first) < 0) { return true; } return false;}
    inline void setValue(T value) { m_value = value; }
    inline T getValue() const { return m_value; }
    inline E errCode() const { return m_result.first; }
    inline void errCode(E err) { m_result.first = err; }
    inline std::string errMessage() const { return m_context+ " " +m_result.second; }
    inline std::string errContext() const { return m_context; }
    inline void errMessage(const std::string & msg) { m_result.second = msg; }
    inline void setResult(E err, const std::string & msg) { m_result.second = msg; m_result.first = err; }
    inline void setResult(E err, const std::string & msg, T value) { m_result.second = msg; m_result.first = err; m_value = value; }

private:
    std::pair<E, std::string> m_result;
    std::string m_context;
    T m_value;
};

}}}


#endif
