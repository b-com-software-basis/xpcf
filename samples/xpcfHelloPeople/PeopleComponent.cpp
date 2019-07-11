#include "PeopleComponent.h"
#include "xpcfHelloPeopleAPI.h"
#include <iostream>

namespace xpcf = org::bcom::xpcf;

template<> PeopleComponent* xpcf::ComponentFactory::createInstance<PeopleComponent>();

PeopleComponent::PeopleComponent():xpcf::ComponentBase(xpcf::toMap<PeopleComponent>())
{
    declareInterface<IMessage>(this);
}


PeopleComponent::~PeopleComponent()
{

}

void PeopleComponent::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    delete this;
    return;
}


void PeopleComponent::print()
{
    std::cout<<"Hello People"<<std::endl;
}
