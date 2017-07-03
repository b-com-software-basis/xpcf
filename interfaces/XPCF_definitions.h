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
 * @date 2017-04-28
 */

#ifndef _XPCF_ComDefine_HeaderFile__
#define _XPCF_ComDefine_HeaderFile__

#if _WIN32
#define XPCF_EXPORT_API __declspec(dllexport)
#else
#define XPCF_EXPORT_API
#endif

#ifdef _WIN32
#define XPCF_IMPORT_API __declspec(dllimport)
#else
#define IMPORT_API
#endif

enum class XPCFErrorCode : long {
    _SUCCESS = 0, /**< Operation successfull  */
    _FAIL = -1, /**< Operation failed  */
    _ERROR_INVALID_ARGUMENT = -10, /**<   */
    _ERROR_DOMAIN = -11, /**<   */
    _ERROR_NOT_IMPLEMENTED = -12, /**<  */
    _ERROR_NULL_POINTER = -13, /**<  */
    _ERROR_OUT_OF_RANGE = -14, /**<   */
    _ERROR_OVERFLOW = -20, /**<   */
    _ERROR_UNDERFLOW = -21, /**<   */
    _ERROR_OUT_OF_MEMORY = -22, /**<  */
    _ERROR_RANGE = -23, /**<  */
    _ERROR_SYSTEM  = -24 /**<  */
};


#define XPCF_FAIL        0x00000000L
#define XPCF_OK          0x00000001L

#define XPCF_ERROR       0x00000002L

#define XPCF_NOINTERFACE 0x00000002L

#define XPCF_NOGETCOMPONENT XPCF_FAIL+1

#define XPCF_NAME_LENGTH		 128
#define XPCF_PATH_LENGTH		 256
#define XPCF_ERROR_MSG_LENGTH 128


#ifndef XPCF_MANAGED_CPP_ENABLED
#ifndef XPCF_STD_SHAREDPTR
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/move/unique_ptr.hpp>
#include <boost/function.hpp>
#include <boost/type_traits.hpp>
#include <boost/any.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
namespace sptrnms = boost;
namespace org {namespace bcom { namespace xpcf {
namespace utils = boost;
namespace uuids = boost::uuids;
namespace unixpcf = boost::movelib;
}}}
#else
#include <memory>
#include <functional>
#include <type_traits>
namespace utils = std;
namespace unixpcf = std;
#endif
#else
//#define SPtr(N) N *
#define SPtr(N) SPtr_##N
#define SRef(N) N *
#endif

template<class T>
using SRef = org::bcom::xpcf::utils::shared_ptr<T>;
template<class T>
using SPtr = org::bcom::xpcf::utils::shared_ptr<T>;

template<class T>
using UniqueRef = org::bcom::xpcf::unixpcf::unique_ptr<T>;

#endif //_XPCF_ComDefine_HeaderFile__
