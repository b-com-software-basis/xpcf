#ifndef ASTPARSER_H
#define ASTPARSER_H
#include <xpcf/component/ComponentBase.h>
#include <xpcf/core/helpers.h>

#include "interfaces/ITypeParser.h"
#include <cppast/libclang_parser.hpp> // for libclang_parser, libclang_compile_config, cpp_entity,...
#include <cppast/cpp_class_template.hpp>

class ASTParser : public org::bcom::xpcf::ComponentBase, virtual public ITypeParser
{
public:
    ASTParser();
    ~ASTParser() override;
    void unloadComponent () override;
    std::map<std::string,SRef<ClassDescriptor>> & getParsedInterfaces() override { return m_interfaces; }
    std::map<std::string,SRef<ClassDescriptor>> & getParsedClasses() override { return m_classes; }

    //SRef<ClassDescriptor> getInterfaceInfo(const std::string & name)  override;
    std::map<IRPCGenerator::MetadataType,std::string> & metadata() override { return m_metadata; }
    SRef<ClassDescriptor> getClassInfo(const std::string & name)  override { if (org::bcom::xpcf::mapContains(m_classes, name)) { return m_classes.at(name); } return nullptr; }
    SRef<ClassDescriptor> getInterfaceInfo(const std::string & name)  override { if (org::bcom::xpcf::mapContains(m_interfaces, name)) { return m_interfaces.at(name); } return nullptr; }
    void parseAst(std::ostream& out, const cppast::cpp_file& file)  override;
    int initOptions(const cxxopts::ParseResult & options) override;

    int parse_database(const std::string & databasePath, const cxxopts::ParseResult & options) override;
    int parse_file(const std::string& filename, bool fatal_error) override;

private:
    static int set_compile_options(const cxxopts::ParseResult & options, cppast::libclang_compile_config & config);
    void parse_database(const cppast::libclang_compilation_database& database, const cxxopts::ParseResult & options);
    void parseEntity(std::ostream& out, const cppast::cpp_entity& e, const std::string & filePath);
    void parseXpcfTrait(std::ostream& out, const cppast::cpp_class_template_specialization& c);
    void processPostAstParsingActions();
    std::string computeNamespace();
    std::map<IRPCGenerator::MetadataType,std::string> m_metadata;
    //std::vector<SRef<ClassDescriptor>> m_interfaces;
    std::map<std::string,SRef<ClassDescriptor>> m_interfaces;
    std::map<std::string,SRef<ClassDescriptor>> m_classes;
    std::map<std::string,org::bcom::xpcf::Traits> m_xpcfTraits;
    std::vector<std::string> m_currentNamespace;
    std::string m_currentFileName;
    cppast::cpp_entity_index  m_index; // the entity index is used to resolve cross references in the AST
    UniqueRef<cppast::simple_file_parser<cppast::libclang_parser>> m_simpleParser;
    UniqueRef<cppast::libclang_parser> m_fileParser;
    cppast::stderr_diagnostic_logger m_logger;
    cppast::libclang_compile_config m_config;
    std::unique_ptr<cppast::cpp_file> m_fileParsed;

};




template <> struct org::bcom::xpcf::ComponentTraits<ASTParser>
{
    static constexpr const char * UUID = "{b82122a7-379d-40c6-8473-e34e4f4fc51d}";
    static constexpr const char * NAME = "ASTParser";
    static constexpr const char * DESCRIPTION = "ASTParser implements ITypeParser interface";
};

#endif // ASTPARSER_H
