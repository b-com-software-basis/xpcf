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

#include "C0.h"

#include "ComponentBase.h"
#include "ContainerFactory.h"
#include <iostream>

namespace xpcf=org::bcom::xpcf;

XPCF_DECLARE_CONTAINER("3b899ff0-e098-4218-bdaa-82abdec22960","Feature XXX container")

extern "C" XPCF_EXPORT_API void XPCF_getComponent(const xpcf::uuids::uuid& componentUUID,SRef<xpcf::IComponentIntrospect>& interfaceRef)
{
    xpcf::uuids::uuid uuidOf_XPCF_CID_C0  = xpcf::toUUID( C0::UUID );
    // Test the UUID to be sure that this library implements this component.
    if (componentUUID == uuidOf_XPCF_CID_C0 )
    {
        xpcf::ComponentFactory::createComponent<C0>(interfaceRef);
	}
}

XPCF_BEGIN_COMPONENTS_DECLARATION
XPCF_ADD_COMPONENT(C0::UUID,"Class C0")
XPCF_END_COMPONENTS_DECLARATION


