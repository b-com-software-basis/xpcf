#include "InjectableAdapterComponent.h"
#include "xpcfHelloInjectableAdapterAPI.h"
#include <iostream>
#include <xpcf/api/IComponentManager.h>

namespace xpcf = org::bcom::xpcf;

template<> InjectableAdapterComponent* xpcf::ComponentFactory::createInstance<InjectableAdapterComponent>();

InjectableAdapterComponent::InjectableAdapterComponent():xpcf::ComponentBase(xpcf::toMap<InjectableAdapterComponent>())
{
    declareInterface<IMessage>(this);
    declareInjectable<IMessage>(m_concreteMessageHandler,"adapterMessageHandler");
}


InjectableAdapterComponent::~InjectableAdapterComponent()
{
}

void InjectableAdapterComponent::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    delete this;
    return;
}

void InjectableAdapterComponent::print()
{
    m_concreteMessageHandler->print();
}
