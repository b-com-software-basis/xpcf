/**
 * @copyright Copyright (c) 2015 All Right Reserved, B-com http://www.b-com.com/
 *
 * This file is subject to the B<>Com License.
 * All other rights reserved.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * @author Loïc Touraine
 *
 * @file
 * @brief description of file
 * @date 2017-09-18
 */

#ifndef XPCF_MANAGEDCPPASPECTS_H
#define XPCF_MANAGEDCPPASPECTS_H

#ifdef XPCF_MANAGED_CPP_ENABLED
#define interface public __gc __interface
#define gc_class public __gc class
#define gc_struct public __gc struct
#define unmanaged_const_method
#using<mscorlib.dll>
#else
#define interface class
#define gc_class class
#define gc_struct struct
#define __nogc
#define __gc
#define unmanaged_const_method const
#endif

#endif
