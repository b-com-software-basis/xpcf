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

#ifndef I1_h
#define I1_h 1

#include "IComponentIntrospect.h"

class I1 : public virtual org::bcom::xpcf::IComponentIntrospect
{
  public:
      virtual ~I1() {}
      virtual void I1_opname () = 0;
      virtual void I1_opname2 () = 0;
      virtual void I1_opname3 () = 0;
      virtual void I1_opname4 () = 0;
      virtual void I1_opname5 () = 0;

    static constexpr const char * UUID = "3bc8f5ea-ee24-473e-8afd-4f5b1b21c018";


};

#endif
