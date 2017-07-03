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

#include <stdio.h>

#include "ComponentTemplate.h"
// XPCF_ComponentBase
#include "ComponentBase.h"

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(client::ComponentTemplate);

namespace client {
ComponentTemplate::ComponentTemplate ()
{
   setUUID(ComponentTemplate::UUID);
   addInterface<ITemplateInterface>(this,ITemplateInterface::UUID, "interface ITemplateInterface");
}

ComponentTemplate::~ComponentTemplate ()
{
}

}
