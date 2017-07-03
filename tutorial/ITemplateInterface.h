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

#ifndef __ITemplateInterface_h__
#define __ITemplateInterface_h__

#include "IComponentIntrospect.h"

namespace client {
class ITemplateInterface : public virtual org::bcom::xpcf::IComponentIntrospect
{

  public:
      virtual ~ITemplateInterface() {}

        // Add interface methods here
        // ...
        //

        // a new UUID for each new interface must be set using uuidgen
      XPCF_DECLARE_UUID("AC4D2F09-0DF1-412E-874D-9245718F4753");

};
}
#endif
