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
 * @date 2015-09-18
 */

#ifndef I0_h
#define I0_h 1

#include "IComponentIntrospect.h"

class I0 : public virtual org::bcom::xpcf::IComponentIntrospect
{

  public:
      virtual ~I0() {}
      virtual void I0_opname () = 0;
      virtual void I0_opname2 () = 0;
      virtual void I0_opname3 () = 0;
      virtual void I0_opname4 () = 0;
      virtual void I0_opname5 () = 0;

      static constexpr const char * UUID = "46333fd5-9eeb-4c9a-9912-b7df96ccf5fc";

};
#endif
