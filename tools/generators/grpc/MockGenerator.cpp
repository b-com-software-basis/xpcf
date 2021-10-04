#include "MockGenerator.h"
#include "CppHelpers.h"

#include <fstream>

namespace xpcf = org::bcom::xpcf;

template<> MockGenerator * xpcf::ComponentFactory::createInstance<MockGenerator>();


MockGenerator::MockGenerator():AbstractGenerator(xpcf::toMap<MockGenerator>())
{
}


MockGenerator::~MockGenerator()
{

}

void MockGenerator::processHeaderMethods(const SRef<ClassDescriptor> c, CppBlockManager & blockMgr)
{
    for (auto m : c->methods()) {
        blockMgr.out() <<  m->getFullDeclaration();
        if (m->isConst()) {
            blockMgr.out() <<  " const";
        }
        blockMgr.out() << " override;\n";
        if (m->returnType().kind() == type_kind::c_string_t) {
            m_proxyMembersVariables["m_" + m->getName()] = std::make_pair("std::string", false);
        }
        else if (m->returnType().isConst() && m->returnType().isReference()) {
            m_proxyMembersVariables["m_" + m->getName()] = std::make_pair(m->returnType().getFullTypeDescription(),m->isConst());
        }
    }
    blockMgr.newline();
}

void MockGenerator::generateHeader(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata, std::ostream& out)
{
    CppBlockManager blockMgr(out);
    //NOTE : mock is configurable to set any needed property
    xpcf::uuids::uuid mockUUID = c->getClientUUID();
    if (mockUUID.is_nil()) {
        mockUUID = c->createClientUUID();
        m_serviceUuidMap[m_className] = xpcf::uuids::to_string(mockUUID);
    }
    blockMgr.out() << "// Mock xpcf component Class Header generated with xpcf_grpc_gen\n\n";
    blockMgr.newline();
    blockMgr.includeGuardsStart(m_className);
    blockMgr.include(c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACEFILEPATH)); // relative or absolute path?? TODO:  retrieve filepath from metadata
    blockMgr.include("xpcf/component/ConfigurableBase.h",false);
    blockMgr.include("memory",false);
    blockMgr.include("string",false);

    blockMgr.newline();
    blockMgr.out() << "namespace " + m_nameSpace + " ";
    {
        block_guard<CPP::NSPACE> nspaceBlk(blockMgr);
        std::string baseInterface;
        if (!c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE).empty()) {
            baseInterface = c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE) + "::";
        }
        baseInterface += c->getName();
        blockMgr.newline();
        blockMgr.out() << "class " + m_className + ":  public org::bcom::xpcf::ConfigurableBase, virtual public " + baseInterface + " ";
        {
            std::map<std::string,std::pair<std::string,bool>> proxyMembersVariables; // name, {type, isConstMethod}
            block_guard<CPP::CLASS> classBlk(blockMgr);
            {
                block_guard<CPP::PUBLIC> publicBlk(blockMgr);

                blockMgr.out() << m_className +"();\n";
                blockMgr.out() << "~"+ m_className +"() override = default;\n";
                blockMgr.out() << "void unloadComponent () override final;\n";
                blockMgr.out() << "org::bcom::xpcf::XPCFErrorCode onConfigured() override;\n";
                blockMgr.newline();
                // foreach method
                processHeaderMethods(c, blockMgr);
            }
            {
                block_guard<CPP::PRIVATE> privateBlk(blockMgr);
                for (auto & [name, value ] : m_proxyMembersVariables) {
                    auto & [type, isConst] = value;
                            if (isConst) {
                        blockMgr.out() << "mutable ";
                    }
                    blockMgr.out() << type << " " << name << ";\n";
                }
            }
        }
    }
    blockMgr.newline();

    m_proxyMembersVariables.clear();

    blockMgr.out() << "template <> struct org::bcom::xpcf::ComponentTraits<" + m_nameSpace + "::" + m_className +">\n";
    {
        block_guard<CPP::CLASS> classBlk(blockMgr);
        blockMgr.out() << "static constexpr const char * UUID = \"" + xpcf::uuids::to_string(mockUUID) + "\";\n";
        blockMgr.out() << "static constexpr const char * NAME = \"" + m_className + "\";\n";
        blockMgr.out() << "static constexpr const char * DESCRIPTION = \"" + m_className + " grpc client proxy component\";\n";
    }
    blockMgr.newline();
    blockMgr.includeGuardsEnd();
}

void MockGenerator::bindInput(const ParameterDescriptor & p, CppBlockManager & blockMgr)
{
    if (p.type().kind() == type_kind::builtin_t
            || p.type().kind() == type_kind::enum_t
            || p.type().kind() == type_kind::std_string_t
            || p.type().kind() == type_kind::c_string_t) {
        if (p.type().needsStaticCast()) {
            blockMgr.out() << "reqIn.set_"<< boost::to_lower_copy(p.getName()) << "(static_cast<" << p.type().getRPCType() << ">(" << p.getName() << "));\n";
        }
        else {
            blockMgr.out() << "reqIn.set_"<< boost::to_lower_copy(p.getName()) << "(" << p.getName() << ");\n";
        }

    }
    else if ((p.type().kind() == type_kind::user_defined_t) || (p.type().kind() == type_kind::template_t)) {
        blockMgr.out() << "reqIn.set_"<< boost::to_lower_copy(p.getName()) << "(xpcf::serialize<" << p.type().getFullTypeDescription() <<">(" << p.getName() << "));\n";
    }
}

void MockGenerator::bindOutput(const ParameterDescriptor & p, CppBlockManager & blockMgr)
{
    if (p.type().kind() == type_kind::builtin_t
            || p.type().kind() == type_kind::enum_t
            || p.type().kind() == type_kind::std_string_t) {
        if (p.type().needsStaticCast()) {
            blockMgr.out() << p.getName() << " = static_cast<" << p.type().getFullTypeDescription() << ">(respOut." << boost::to_lower_copy(p.getName()) <<"());\n";
        }
        else {
            blockMgr.out() <<p.getName()<<" = respOut."<< boost::to_lower_copy(p.getName()) <<"();\n";
        }
    }
    else if ((p.type().kind() == type_kind::user_defined_t)|| (p.type().kind() == type_kind::template_t)) {
        blockMgr.out() <<p.getName()<<" = xpcf::deserialize<" << p.type().getFullTypeDescription() << ">(respOut."<<boost::to_lower_copy(p.getName()) <<"());\n";
    }
}

void MockGenerator::processBodyMethods(const SRef<ClassDescriptor> c, CppBlockManager & blockMgr)
{
    for (auto m : c->methods()) {
        blockMgr.out() << m->getReturnType() + "  "+ m_className + "::" + m->getDeclaration();
        if (m->isConst()) {
            blockMgr.out() <<  " const";
        }
        blockMgr.out() << "\n";
        {
            block_guard methodBlk(blockMgr);
            if (m->hasInputs()) {
                for (ParameterDescriptor * p: m->m_inParams) {
                    bindInput(*p, blockMgr);
                }
                for (ParameterDescriptor * p: m->m_inoutParams) {
                    bindInput(*p, blockMgr);
                }
            }

            blockMgr.out() << "// put your mock code here\n";

            if (m->hasOutputs())  {
                for (ParameterDescriptor * p: m->m_inoutParams) {
                    bindOutput(*p, blockMgr);
                }
                for (ParameterDescriptor * p: m->m_outParams) {
                    bindOutput(*p, blockMgr);
                }
            }
            if (!m->returnType().isVoid()) {
                if (m->returnType().kind() == type_kind::builtin_t) {
                    blockMgr.out() << m->returnType().getFullTypeDescription() << "xpcfMockReturnValue;\n";
                    blockMgr.out() << "return xpcfMockReturnValue;\n";
                }
                else if (m->returnType().kind() == type_kind::user_defined_t) {
                    if (m->returnType().isConst() && m->returnType().isReference()) {
                        blockMgr.out() << "return m_" << m->getName() << ";\n";
                    }
                    else {
                        blockMgr.out() << m->returnType().getFullTypeDescription() << "xpcfMockReturnValue;\n";
                        blockMgr.out() << "return xpcfMockReturnValue;\n";
                    }
                }
                else if (m->returnType().kind() == type_kind::enum_t) {
                    blockMgr.out() << m->returnType().getFullTypeDescription() << "xpcfMockReturnValue;\n";
                    blockMgr.out() << "return xpcfMockReturnValue;\n";
                }
                else if (m->returnType().kind() == type_kind::c_string_t ) {
                    blockMgr.out() << "return m_" << m->getName() << ".c_str();\n";
                }
            }
        }
        blockMgr.newline();
    }
}

void MockGenerator::generateBody(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata, std::ostream& out)
{
    CppBlockManager blockMgr(out);
    blockMgr.out() << "// XPCF Mock Class implementation generated with xpcf_grpc_gen\n";
    blockMgr.include(m_headerFileName);
    blockMgr.include("cstddef",false);
    blockMgr.include("xpcf/core/Exception.h",false);
    blockMgr.out() << "namespace xpcf = org::bcom::xpcf;\n";


    blockMgr.newline();

    blockMgr.out() << "template<> " + m_nameSpace + "::" + m_className + "* xpcf::ComponentFactory::createInstance<"+ m_nameSpace + "::" + m_className + ">();\n";
    blockMgr.newline();
    blockMgr.out() << "namespace " + m_nameSpace + " ";
    {
        block_guard<CPP::NSPACE> nspaceBlk(blockMgr);
        blockMgr.newline();

        blockMgr.out() << m_className + "::" + m_className + "():xpcf::ConfigurableBase(xpcf::toMap<"+ m_className + ">())\n";
        {
            block_guard methodBlk(blockMgr);
            std::string baseInterface;
            if (!c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE).empty()) {
                baseInterface = c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE) + "::";
            }
            baseInterface += c->getName();
            blockMgr.out() << "declareInterface<" + baseInterface + ">(this);\n";
            blockMgr.out() << "declareProperty(\"channelUrl\",m_channelUrl);\n";
            blockMgr.out() << "declareProperty(\"channelCredentials\",m_channelCredentials);\n";
        }
        blockMgr.newline();
        blockMgr.out() << "void " + m_className + "::unloadComponent ()\n";
        {
            block_guard methodBlk(blockMgr);
            blockMgr.out() << "delete this;\n";
            blockMgr.out() << "return;\n";
        }
        blockMgr.newline();
        blockMgr.out() << "XPCFErrorCode " + m_className +"::onConfigured()\n";
        {
            block_guard methodBlk(blockMgr);
            blockMgr.out() << "return xpcf::XPCFErrorCode::_SUCCESS;\n";
        }
        blockMgr.newline();
        processBodyMethods(c, blockMgr);
    }
}

std::map<IRPCGenerator::MetadataType,std::string> MockGenerator::generateImpl(SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata)

{
    m_nameSpace =  "org::bcom::xpcf::mock" + c->getName();
    m_className = c->getName() + "_mockComponent";
    m_headerFileName = m_className + ".h";
    m_cppFileName = m_className + ".cpp";
    if (m_mode == GenerateMode::STD_COUT) {
        generateHeader(c, metadata, std::cout);
        generateBody(c, metadata, std::cout);
        std::cout<<std::endl;
    }
    else {
        fs::detail::utf8_codecvt_facet utf8;
        fs::path headerFilePath(m_headerFileName,utf8);
        fs::path cppFilePath(m_cppFileName,utf8);
        headerFilePath = m_folder/headerFilePath;
        cppFilePath = m_folder/cppFilePath;
        std::ofstream headerFile(headerFilePath.generic_string(utf8).c_str(), std::ios::out);
        generateHeader(c, metadata, headerFile);
        headerFile.close();
        std::ofstream cppFile(cppFilePath.generic_string(utf8).c_str(), std::ios::out);
        generateBody(c, metadata, cppFile);
        cppFile.close();
    }
    metadata[PROXY_XPCFGRPCCOMPONENTNAME] = m_className;
    metadata[PROXY_XPCFGRPCNAMESPACE] = m_nameSpace;
    metadata[MetadataType::PROXY_HEADERFILENAME] = m_headerFileName;
    metadata[MetadataType::PROXY_CPPFILENAME] = m_cppFileName;
    return metadata;
}

void MockGenerator::finalizeImpl(std::map<MetadataType,std::string> metadata)
{
    for (auto [name, uuid] : m_serviceUuidMap) {
        std::cout << "Component " << name << " generated uuid = [[xpcf::clientUUID(" << uuid <<")]]" << std::endl;
    }
    m_serviceUuidMap.clear();
}

