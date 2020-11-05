#ifndef ITYPEPARSER_H
#define ITYPEPARSER_H


#include <xpcf/api/IComponentIntrospect.h>
#include "ClassDescriptor.h"
#include "interfaces/IRPCGenerator.h"
#include <cxxopts.hpp>

class ITypeParser: virtual public org::bcom::xpcf::IComponentIntrospect
{
public:
    virtual ~ITypeParser() override = default;
    virtual std::vector<SRef<ClassDescriptor>> & getParsedInterfaces() = 0 ;
    //virtual SRef<ClassDescriptor> getInterfaceInfo(const std::string & name) = 0;
    virtual SRef<ClassDescriptor> getClassInfo(const std::string & name) = 0;
    virtual std::map<IRPCGenerator::MetadataType,std::string> & metadata() = 0;
    virtual int parse_database(const std::string & databasePath, const cxxopts::ParseResult & options) = 0;
    virtual const cppast::cpp_file* parse_file(const std::string& filename, bool fatal_error) = 0;
    virtual void parseAst(std::ostream& out, const cppast::cpp_file& file) = 0;
    virtual int initOptions(const cxxopts::ParseResult & options) = 0;


};



template <> struct org::bcom::xpcf::InterfaceTraits<ITypeParser>
{
    static constexpr const char * UUID = "{20be2759-44ea-44ae-8d52-15a5ff9945c1}";
    static constexpr const char * NAME = "ITypeParser";
    static constexpr const char * DESCRIPTION = "ITypeParser interface description";
};

#endif // ITYPEPARSER_H
