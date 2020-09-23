#include "AbstractGenerator.h"

namespace xpcf = org::bcom::xpcf;

template<> AbstractGenerator * xpcf::ComponentFactory::createInstance<AbstractGenerator>();



AbstractGenerator::AbstractGenerator(std::map<std::string,std::string> componentInfosMap):xpcf::ComponentBase(componentInfosMap)
{
    declareInterface<IRPCGenerator>(this);
    //  Inject declarations come here : declare any component that must be injected to your component through its interface /////////////////////////..///// declareInjectable<IFilter>(m_filter);
    //
    // Inject declaration can have a name :
    // declareInjectable<IFilter>(m_blurFilter, "blurFilter");
    //
    // Inject declaration can be optional i.e. not finding a binding component for the interface is not an error :
    // declareInjectable<IImageFilter>(m_imageFilter, false);

}


AbstractGenerator::~AbstractGenerator()
{

}

void AbstractGenerator::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    delete this;
    return;
}

void AbstractGenerator::setDestinationFolder(const std::string & folder)
{
    fs::detail::utf8_codecvt_facet utf8;
    fs::path folderPath(folder,utf8);
    if (!fs::exists(folderPath)) {
        fs::create_directories(folderPath); // TODO : manage error
    }
    m_folder = folderPath;
}

void AbstractGenerator::setGenerateMode(const GenerateMode & mode)
{
    m_mode = mode;
}

