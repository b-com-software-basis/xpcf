%{Cpp:LicenseTemplate}\

#include <xpcf/xpcf.h>
#include <iostream>

namespace xpcf=org::bcom::xpcf;


/**
  * Declare module.
  */
int main(int argc, char ** argv)
{
    SRef<xpcf::IComponentManager> xpcfComponentManager = xpcf::getComponentManagerInstance();
    XPCFErrorCode xpcfComponentManager->loadModuleMetadata(%{moduleName},"./");
    SRef<%{interfaceName}> component = xpcfComponentManager->create<>(%{componentName})->bindTo<%{interfaceName}>();
    // call %{interfaceName} methods below
    //...
    return 0;
}
