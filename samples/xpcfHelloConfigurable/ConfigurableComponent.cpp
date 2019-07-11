#include "ConfigurableComponent.h"
#include "xpcfHelloConfigurableAPI.h"
#include <iostream>

namespace xpcf = org::bcom::xpcf;

template<> ConfigurableComponent* xpcf::ComponentFactory::createInstance<ConfigurableComponent>();

ConfigurableComponent::ConfigurableComponent():xpcf::ConfigurableBase(xpcf::toMap<ConfigurableComponent>())
{
    declareInterface<IMessage>(this);
    declareProperty("message",m_message);
}


ConfigurableComponent::~ConfigurableComponent()
{

}

void ConfigurableComponent::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    delete this;
    return;
}


void ConfigurableComponent::print()
{
    std::cout<<m_message<<std::endl;
}
