#include "XpcfConfigGenerator.h"
#include <boost/process.hpp>


namespace bp = boost::process;
namespace xpcf = org::bcom::xpcf;

template<> XpcfConfigGenerator * xpcf::ComponentFactory::createInstance<XpcfConfigGenerator>();

XpcfConfigGenerator::XpcfConfigGenerator():AbstractGenerator(xpcf::toMap<XpcfConfigGenerator>())
{
}


XpcfConfigGenerator::~XpcfConfigGenerator()
{
}

std::unique_ptr<XmlFileManager> XpcfConfigGenerator::createXmlFileManager(const std::string & fileName)
{
    fs::detail::utf8_codecvt_facet utf8;
    fs::path xmlFilePath(fileName,utf8);
    xmlFilePath = m_folder/xmlFilePath;
    return XmlFileManager::createXmlFileManager(xmlFilePath);
}

void XpcfConfigGenerator::initializeImpl(std::map<MetadataType,std::string> metadata)
{
    if (m_mode == GenerateMode::STD_COUT) {
        m_xmlClientMgr = std::make_unique<XmlBlockManager>(std::cout);
        m_xmlServerMgr = std::make_unique<XmlBlockManager>(std::cout);
        m_xmlClientPropsMgr = std::make_unique<XmlBlockManager>(std::cout);
        m_xmlServerPropsMgr = std::make_unique<XmlBlockManager>(std::cout);
    }
    else {
        m_xmlClientMgr = createXmlFileManager("xpcfGrpcRemoting" + metadata[MetadataType::PROJECT_NAME] + "Client.xml");
        m_xmlServerMgr = createXmlFileManager("xpcfGrpcRemoting" + metadata[MetadataType::PROJECT_NAME] + "Server.xml");
        m_xmlClientPropsMgr = createXmlFileManager("xpcfGrpcRemoting" + metadata[MetadataType::PROJECT_NAME] + "Client_properties.xml");
        m_xmlServerPropsMgr = createXmlFileManager("xpcfGrpcRemoting" + metadata[MetadataType::PROJECT_NAME] + "Server_properties.xml");
    }
    m_xmlClientMgr->xmlOpenRootNode();
    m_xmlClientMgr->enter<XML::MODULE>({{"uuid",metadata[MetadataType::MODULE_UUID]},
                                        {"name",metadata[MetadataType::MODULE_NAME]},
                                        {"description",metadata[MetadataType::MODULE_DESCRIPTION]},
                                        {"path","$REMAKENROOT/modules"}});
    m_xmlServerMgr->xmlOpenRootNode();
    m_xmlServerMgr->enter<XML::MODULE>({{"uuid",metadata[MetadataType::MODULE_UUID]},
                                        {"name",metadata[MetadataType::MODULE_NAME]},
                                        {"description",metadata[MetadataType::MODULE_DESCRIPTION]},
                                        {"path","$REMAKENROOT/modules"}});
    m_xmlClientPropsMgr->xmlOpenRootNode("xpcf-configuration");
    m_xmlServerPropsMgr->xmlOpenRootNode("xpcf-configuration");
}

std::map<IRPCGenerator::MetadataType,std::string> XpcfConfigGenerator::generateImpl(SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata)
{
    {
        xml_block_guard<XML::COMPONENT> componentBlk(*m_xmlClientMgr, {{"uuid",xpcf::uuids::to_string(c->getClientUUID())},
                                                                       {"name",metadata[MetadataType::PROXY_XPCFGRPCCOMPONENTNAME]},
                                                                       {"description",metadata[MetadataType::PROXY_XPCFGRPCCOMPONENTNAME]}});
        m_xmlClientMgr->node<XML::INTERFACE>({{"uuid","125f2007-1bf9-421d-9367-fbdc1210d006"},
                                              {"name","XPCF::IComponentIntrospect"},
                                              {"description","Component introspection interface"}});
        m_xmlClientMgr->node<XML::INTERFACE>({{"uuid", c->getXpcfTrait().uuid},
                                              {"name", c->getXpcfTrait().name},
                                              {"description", c->getXpcfTrait().description}});
    }
    {
        xml_block_guard<XML::COMPONENT> componentBlk(*m_xmlServerMgr,{{"uuid",xpcf::uuids::to_string(c->getServerUUID())},
                                                                      {"name",metadata[MetadataType::SERVER_XPCFGRPCCOMPONENTNAME]},
                                                                      {"description",metadata[MetadataType::SERVER_XPCFGRPCCOMPONENTNAME]}});
        m_xmlServerMgr->node<XML::INTERFACE>({{"uuid","125f2007-1bf9-421d-9367-fbdc1210d006"},
                                              {"name","XPCF::IComponentIntrospect"},
                                              {"description","Component introspection interface"}});
        m_xmlServerMgr->node<XML::INTERFACE>({{"uuid", "baab5e42-2c24-48de-953b-07237af419e4"},
                                              {"name", "org::bcom::xpcf::IGrpcService"},
                                              {"description", "xpcf GrpcService interface"}});
    }
    m_grpcComponentBinds.push_back(metadata.at(MetadataType::SERVER_XPCFGRPCCOMPONENTNAME));
    return metadata;
}

void XpcfConfigGenerator::finalizeImpl(std::map<MetadataType,std::string> metadata)
{
    m_xmlClientMgr->leave();
    m_xmlServerMgr->leave();
    {
        xml_block_guard<XML::FACTORY> factoryBlk(*m_xmlServerMgr);
        {
            xml_block_guard<XML::BINDINGS> factoryBlk(*m_xmlServerMgr);
            {
                xml_block_guard<XML::BIND> factoryBlk(*m_xmlServerMgr, {{"interface","org::bcom::xpcf::IGrpcService"}});
                for (auto & bind : m_grpcComponentBinds) {
                    m_xmlServerMgr->node<XML::COMPONENT>({{"to", bind}});
                }
            }
        }
    }
    m_xmlClientMgr->release();
    m_xmlServerMgr->release();
    m_xmlClientPropsMgr->release();
    m_xmlServerPropsMgr->release();
    //to put version in pkgDepsFile for protobuf
    //pkg-config --modversion protobuf
    // protobuf|3.15.6|protobuf|brew@system
}
