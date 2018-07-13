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

#ifndef ORG_BCOM_XPCF_XPCFERRORCODE_H
#define ORG_BCOM_XPCF_XPCFERRORCODE_H

namespace org {namespace bcom { namespace xpcf {

typedef enum {
    _SUCCESS = 0, /**< Operation successfull  */
    _FAIL = -1, /**< Operation failed  */
    _ERROR_INVALID_ARGUMENT = -10, /**<   */
    _ERROR_DOMAIN = -11, /**<   */
    _ERROR_NOT_IMPLEMENTED = -12, /**<  */
    _ERROR_NULL_POINTER = -13, /**<  */
    _ERROR_OUT_OF_RANGE = -14, /**<   */
    _ERROR_TYPE = -15, /**<   */
    _ERROR_OVERFLOW = -20, /**<   */
    _ERROR_UNDERFLOW = -21, /**<   */
    _ERROR_OUT_OF_MEMORY = -22, /**<  */
    _ERROR_RANGE = -23, /**<  */
    _ERROR_SYSTEM  = -24, /**<  */
    _ERROR_UNKNOWN  = -25, /**<  */
    _ERROR_ACCESS_DENIED  = -26, /**<  */
    _ERROR_COMPONENT_UNKNOWN = -30,
    _ERROR_INTERFACE_UNKNOWN = -40,
    _ERROR_MODULE_UNKNOWN = -50,
    _ERROR_MODULE_NOGETCOMPONENT = -51
} XPCFErrorCode;

}}}

#endif //_XPCF_ComDefine_HeaderFile__
