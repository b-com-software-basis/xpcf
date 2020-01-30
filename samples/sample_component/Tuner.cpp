#include "Tuner.h"

namespace xpcf = org::bcom::xpcf;

template<> component::Tuner * xpcf::ComponentFactory::createInstance<component::Tuner>();


namespace component {
Tuner::Tuner():xpcf::ConfigurableBase(xpcf::toMap<Tuner>())
{
    declareInterface<ITuner>(this);
    //  Inject declarations come here : declare any component that must be injected to your component through its interface /////////////////////////..///// declareInjectable<IFilter>(m_filter);
    //
    // Inject declaration can have a name :
    // declareInjectable<IFilter>(m_blurFilter, "blurFilter");
    //
    // Inject declaration can be optional i.e. not finding a binding component for the interface is not an error :
    // declareInjectable<IImageFilter>(m_imageFilter, false);

    // wrap any component member variable to expose as properties with declareProperty<T>() with T matching the variable type
    // For example : declareProperty<float>("blurFactor",m_blurFactor);
    // declareProperty("name",m_memberVariable) also works with template type deduction when m_memberVariable is a supported type of IProperty
    declareProperty("TuneType",m_tuneType);
    std::cout<<"Constructor Tuner::Tuner () called!"<<std::endl;
}


Tuner::~Tuner()
{
    std::cout<<"Destructor Tuner::Tuner () called!"<<std::endl;
}

void Tuner::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    delete this;
    return;
}

xpcf::XPCFErrorCode Tuner::onConfigured()
{
    // Add custom onConfigured code
    return xpcf::XPCFErrorCode::_SUCCESS;
}

void Tuner::onInjected()
{
}

}

