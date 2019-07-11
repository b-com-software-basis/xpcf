
#include <xpcf/module/ModuleFactory.h>
#include <iostream>
#include "WorldComponent.h"
#include "PeopleComponent.h"
#include "ConfigurableComponent.h"
#include "InjectableAdapterComponent.h"

namespace xpcf=org::bcom::xpcf;

/**
 *  @ingroup xpcfmodule
 */
/**
  * Declare module.
  */
XPCF_DECLARE_MODULE("81B74564-CCE7-4266-867D-9772DFA7F9B0","xpcfHelloWithAllComponents","xpcfHelloWithAllComponents module description");

/**
 * This method is the module entry point.
 * XPCF uses this method to create components available in the module.
 *
 * Each component exposed must be declared inside a xpcf::tryCreateComponent<ComponentType>() call.
 */
extern "C" XPCF_MODULEHOOKS_API xpcf::XPCFErrorCode XPCF_getComponent(const xpcf::uuids::uuid& componentUUID,SRef<xpcf::IComponentIntrospect>& interfaceRef)
{
    xpcf::XPCFErrorCode errCode = xpcf::XPCFErrorCode::_FAIL;
    errCode = xpcf::tryCreateComponent<WorldComponent>(componentUUID,interfaceRef);
    if (errCode != xpcf::XPCFErrorCode::_SUCCESS) {
        errCode = xpcf::tryCreateComponent<PeopleComponent>(componentUUID,interfaceRef);
    }
    if (errCode != xpcf::XPCFErrorCode::_SUCCESS) {
        errCode = xpcf::tryCreateComponent<ConfigurableComponent>(componentUUID,interfaceRef);
    }
    if (errCode != xpcf::XPCFErrorCode::_SUCCESS) {
        errCode = xpcf::tryCreateComponent<InjectableAdapterComponent>(componentUUID,interfaceRef);
    }
    return errCode;
}

/**
  * The declarations below populate list of the components available in the module (it represents the module index).
  * XPCF uses this index to introspect the components available in a module, providing the ability to generate the configuration file skeleton from the code.
  */
XPCF_BEGIN_COMPONENTS_DECLARATION
XPCF_ADD_COMPONENT(WorldComponent)
XPCF_ADD_COMPONENT(PeopleComponent)
XPCF_ADD_COMPONENT(ConfigurableComponent)
XPCF_ADD_COMPONENT(InjectableAdapterComponent)
XPCF_END_COMPONENTS_DECLARATION
