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

#include "ComponentTemplate.h"

#include "ComponentBase.h"
#include "ContainerFactory.h"
#include <iostream>

namespace xpcf=org::bcom::xpcf;

XPCF_DECLARE_CONTAINER("3F975C08-3AD1-4361-8B7B-214C17F19DE9","TemplateContainer")

extern "C" XPCF_EXPORT_API void XPCF_getComponent(const boost::uuids::uuid& componentUUID,SRef<xpcf::IComponentIntrospect>& interfaceRef)
{
    boost::uuids::uuid uuidOf_XPCF_CID_ComponentTemplate  = xpcf::toUUID( client::ComponentTemplate::UUID );
	// Test the UUID to be sure that this library implements this component.
    if (componentUUID == uuidOf_XPCF_CID_ComponentTemplate )
    {
        xpcf::ComponentFactory::createComponent<client::ComponentTemplate>(interfaceRef);
	}
    /*else if (aClsid == uuidOf_XPCF_CID_OtherComponent  )
	{
        xpcf::ComponentFactory::createComponent<OtherComponent>(interfaceRef);
	}*/

}

XPCF_BEGIN_COMPONENTS_DECLARATION
XPCF_ADD_COMPONENT(client::ComponentTemplate::UUID,"Component Template")
XPCF_END_COMPONENTS_DECLARATION
