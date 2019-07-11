#include "WorldComponent.h"
#include "xpcfHelloWorldAPI.h"
#include <iostream>

namespace xpcf = org::bcom::xpcf;

template<> WorldComponent* xpcf::ComponentFactory::createInstance<WorldComponent>();

WorldComponent::WorldComponent():xpcf::ComponentBase(xpcf::toMap<WorldComponent>())
{
    declareInterface<IMessage>(this);
}


WorldComponent::~WorldComponent()
{

}

void WorldComponent::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    delete this;
    return;
}


void WorldComponent::print()
{
    std::cout<<"Hello World"<<std::endl;
}
