#include "TypeDescriptor.h"

#include <iostream>
#include <cppast/cpp_template.hpp>
#include <cppast/cpp_array_type.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <boost/algorithm/string.hpp>
#include <regex>

constexpr const char * TMPLGROUPPREFIX="XPCFGRPCGEN_TMPL_GROUP";
namespace xpcf = org::bcom::xpcf;

TypeDescriptor::TypeDescriptor(const cppast::cpp_type & p)
{
    m_descriptorInfo = xpcf::unixpcf::make_unique<TypeDescriptorInfo>(p);
}

TypeDescriptor::TypeDescriptor(UniqueRef<TypeDescriptorInfo> info)
{
    m_descriptorInfo = std::move(info);
}

TypeDescriptor::TypeDescriptor(const std::string & typeName, template_type tmplType, const cppast::cpp_type & p)
{
    m_descriptorInfo = xpcf::unixpcf::make_unique<TypeDescriptorInfo>(typeName,tmplType,p);
    m_descriptorInfo->m_kind = type_kind::template_t;
    m_descriptorInfo->m_isBuiltin = false;
}

bool isCppFixedWidthType(const cppast::cpp_type & p)
{
    const std::string cppFixedWidthTypeList="int8_t|int16_t|int32_t|int64_t|"
                                            "uint8_t|uint16_t|uint32_t|uint64_t|"
                                            "int_least8_t|int_least16_t|int_least32_t|int_least64_t|"
                                            "uint_least8_t|uint_least16_t|uint_least32_t|uint_least64_t|"
                                            "int_fast8_t|int_fast16_t|int_fast32_t|int_fast64_t|"
                                            "uint_fast8_t|uint_fast16_t|uint_fast32_t|uint_fast64_t|"
                                            "intptr_t|uintptr_t|intmax_t|uintmax_t|";
    return (cppFixedWidthTypeList.find(to_string(p) + '|') != std::string::npos);
}

bool isStdString(const cppast::cpp_type & p)
{
    const std::string typeStr="std::string|std::basic_string<char>|";
    return (typeStr.find(to_string(p) + '|') != std::string::npos);
}


const std::map<std::string,enum cpp_builtin_type> builtinStrToEnumMap = {
{"int8_t",cpp_builtin_type::cpp_int8_t},
{"int16_t",cpp_builtin_type::cpp_int16_t},
{"int32_t",cpp_builtin_type::cpp_int32_t},
{"int64_t",cpp_builtin_type::cpp_int64_t},
{"uint8_t",cpp_builtin_type::cpp_uint8_t},
{"uint16_t",cpp_builtin_type::cpp_uint16_t},
{"uint32_t",cpp_builtin_type::cpp_uint32_t},
{"uint64_t",cpp_builtin_type::cpp_uint64_t},
{"int_least8_t",cpp_builtin_type::cpp_int_least8_t},
{"int_least16_t",cpp_builtin_type::cpp_int_least16_t},
{"int_least32_t",cpp_builtin_type::cpp_int_least32_t},
{"int_least64_t",cpp_builtin_type::cpp_int_least64_t},
{"uint_least8_t",cpp_builtin_type::cpp_uint_least8_t},
{"uint_least16_t",cpp_builtin_type::cpp_uint_least16_t},
{"uint_least32_t",cpp_builtin_type::cpp_uint_least32_t},
{"uint_least64_t",cpp_builtin_type::cpp_uint_least64_t},
{"int_fast8_t",cpp_builtin_type::cpp_int_fast8_t},
{"int_fast16_t",cpp_builtin_type::cpp_int_fast16_t},
{"int_fast32_t",cpp_builtin_type::cpp_int_fast32_t},
{"int_fast64_t",cpp_builtin_type::cpp_int_fast64_t},
{"uint_fast8_t",cpp_builtin_type::cpp_uint_fast8_t},
{"uint_fast16_t",cpp_builtin_type::cpp_uint_fast16_t},
{"uint_fast32_t",cpp_builtin_type::cpp_uint_fast32_t},
{"uint_fast64_t",cpp_builtin_type::cpp_uint_fast64_t},
{"uint_fast64_t",cpp_builtin_type::cpp_uint_fast64_t},
{"intptr_t",cpp_builtin_type::cpp_intptr_t},
{"uintptr_t",cpp_builtin_type::cpp_uintptr_t},
{"intmax_t",cpp_builtin_type::cpp_intmax_t},
{"uintmax_t",cpp_builtin_type::cpp_uintmax_t},
};

const std::map<std::string,enum template_type> templateToContainerMap = {
{"std::vector",template_type::vector_t},
{"std::map",template_type::map_t},
{"std::unordered_map",template_type::map_t},
{"std::tuple",template_type::tuple_t},
{"std::shared_ptr",template_type::sharedptr_t},
{"SRef",template_type::sharedptr_t},
};

enum cpp_builtin_type cppFixedWidthType(const cppast::cpp_type & p)
{
    return builtinStrToEnumMap.at(cppast::to_string(p));
}

bool isContainer(const cppast::cpp_template_instantiation_type& p)
{
    const std::string containerList="std::vector|std::map|std::unordered_map";
    return (containerList.find( p.primary_template().name()) != std::string::npos);
}


std::string TypeDescriptor::getFullTypeDescription() const
{
    std::string typeStr = m_descriptorInfo->m_typename;

    std::string nSpace = m_descriptorInfo->m_nameSpace;
    if (!nSpace.empty() && nSpace.length() > 0) {
        nSpace += "::";
        typeStr = nSpace + m_descriptorInfo->m_typename;
    }

    bool severalArgs = false;
    if (m_descriptorInfo->m_kind == type_kind::template_t) {
        typeStr += "<";
    }
    if (m_descriptorInfo->m_tmplArgsVector.size() > 0) {
        for (auto & value : m_descriptorInfo->m_tmplArgsVector) {
            if (severalArgs) {
                typeStr += ",";
            }
            if (value->kind() == type_kind::template_t) {
                typeStr += value->getFullTypeDescription();
            }
            else {
                std::string nSpace = value->getNamespace();
                if (!nSpace.empty() && nSpace.length() > 0) {
                    nSpace += "::";
                }
                typeStr += nSpace + value->getTypename();
            }
            severalArgs = true;
        }
    }
#ifdef CPPAST_TPLARGS_NOTEXPOSED
    else {
        for (auto & [key,value] : m_tmplArgs) {
            if (severalArgs) {
                typeStr += ",";
            }
            if (value->kind() == type_kind::template_t) {
                typeStr += value->getFullTypeDescription();
            }
            else {
                std::string nSpace = value->getNamespace();
                if (!nSpace.empty() && nSpace.length() > 0) {
                    nSpace += "::";
                }
                typeStr += nSpace + value->getTypename();
            }
            severalArgs = true;
        }
    }
#endif
    if (m_descriptorInfo->m_kind == type_kind::template_t) {
        typeStr += ">";
    }
    return typeStr;
}

#ifdef CPPAST_TPLARGS_NOTEXPOSED

const std::map<std::string,enum template_type> leafTmplRegexpMap = {
{"(std::)*vector\s*<\s*([^<> ]*)\s*>", template_type::vector_t},
{"(std::)*(map|unordered_map)\s*<\s*([^<>\s]*)\s*>",template_type::map_t},
{"(std::)*(SRef|SPtr|shared_ptr)+\s*<\s*([^<>]*)\s*>",template_type::sharedptr_t},
{"(std::)*tuple\s*<\s*([^<>\s]*)\s*>",template_type::tuple_t}
};


// Note : SRef<T> or shared_ptr<T> can be flattened to T *, hence remove the SRef template level
// std::vector<SRef<std::vector<int>>> is equivalent to std::vector<std::vector<int>*>,
// there is just the need to know the pointer type is not rough but shared_ptr one
std::shared_ptr<TypeDescriptor> TypeDescriptor::deduceTemplateType(const std::string & leafTemplate)
{
    std::string combinedVectorRegex = "(std::)*vector\s*<\s*([^<> ]*|SRef<[^<>]*>)\s*>";
    //std::string leafSrefRegex = "(std::)*(SRef|SPtr|shared_ptr)+\s*<\s*([^<>\s]*)\s*>\s*";
    std::string tupleRegex = "(std::)*tuple\s*<\s*([^<>\s]*)\s*>";

    std::smatch sm;
    for (auto & [regexStr, tmplType] : leafTmplRegexpMap) {
        std::regex tmplRegex(regexStr, std::regex_constants::extended);
        if (std::regex_search(leafTemplate, sm, tmplRegex, std::regex_constants::match_flag_type::match_any)) {
            std::string matchStr = sm.str(0);
            std::string tmplTypeName = matchStr.substr(0,matchStr.find_first_of('<'));
            std::cout<<"Found "<< tmplTypeName<<" template "<<matchStr<<std::endl;
            return std::make_shared<TypeDescriptor>(tmplTypeName ,tmplType);
        }
    }
    return nullptr;
}


void TypeDescriptor::addTemplateArgument(const std::string & arg, const std::shared_ptr<TypeDescriptor> & d)
{
    m_tmplArgs[arg]=d;
}

void TypeDescriptor::linkArgumentsType(const std::string & args, const std::map<std::string,std::pair<std::string,std::shared_ptr<TypeDescriptor>>> & templateGroups)
{
    std::string argStrRegex = TMPLGROUPPREFIX;
    argStrRegex += "[0-9]*";
    std::regex tmplRegex(argStrRegex, std::regex_constants::extended);
    std::smatch sm;
    std::string argStr = args;
    while (std::regex_search(argStr, sm, tmplRegex, std::regex_constants::match_flag_type::match_any)) {
        std::string matchStr = sm.str(0);
        std::cout<<"Add template argument to "<<matchStr<<std::endl;
        argStr = sm.suffix();
        addTemplateArgument(matchStr, templateGroups.at(matchStr).second);
    }
}

// this method parses template arguments for unexposed arguments from cppast
// argStr is the full argument string parsed
// It finally build the arguments AST, the tree is composed of TypeDescriptor nodes
// Note : final types are not added.
std::string TypeDescriptor::parseTemplateArguments(const std::string & argStr, uint32_t groupNumber)
{
    std::string argumentStr = argStr;
    std::string regexArgumentStr = argStr;
    std::vector<std::string> tmplArgs;
    std::string leafTemplateGroupRegex = "[^<,]*<[^<>]*>";
    std::regex tmplRegex(leafTemplateGroupRegex, std::regex_constants::extended);
    std::smatch sm;
    // parsing finds first leaf template instanciation, replace leaf templates with groups identifiers then search for "new"(up level) leaf templates
    // until there is no more leaf
    while (std::regex_search(regexArgumentStr, sm, tmplRegex, std::regex_constants::match_flag_type::match_any)) {
        std::string matchStr = sm.str(0);
        std::cout<<"Found leaf template "<<matchStr<<std::endl;
        regexArgumentStr = sm.suffix();
        std::string groupNumberStr = TMPLGROUPPREFIX + std::to_string(groupNumber++);
        argumentStr = std::regex_replace(argumentStr, tmplRegex, groupNumberStr, std::regex_constants::format_first_only);
        m_templateGroups[groupNumberStr] = std::make_pair(matchStr,deduceTemplateType(matchStr));
        if (m_templateGroups[groupNumberStr].second) {
            m_templateGroups[groupNumberStr].second->linkArgumentsType(matchStr, m_templateGroups);
        }
    }

    // NOTE :doesn't provide the template level ex: T<G,U<K,V<Z,Y>>,K<L,M>,G<M>>
    // matches are V<Z,Y> , K<L,M> , G<M> in the sample, V<Z,Y> is an inner template !
    // first search for inner template instantiation
    if (argumentStr.find_first_of('<') != argumentStr.npos) {
        std::string subTemplateName = argumentStr.substr(0,argumentStr.find_first_of('<'));
        std::string subArgs = argumentStr.substr(argumentStr.find_first_of('<')+1,argumentStr.find_last_of('>')-2);
        std::cout<<"Inner template name = "<<subTemplateName<<" subArgs = "<<subArgs<<std::endl;
        argumentStr = parseTemplateArguments(argumentStr, groupNumber);
    } else {
        // if none were found : leaf template args : split template arguments
        // NOTE : this behavior ignores preprocessing issues : I assume preprocessing took place before !
        std::cout<<"Remaining arguments "<<argumentStr<<std::endl;
        boost::split(tmplArgs, argumentStr, [](char c){return c == ',';});
        std::cout<<"TemplateGroups:"<<std::endl;

        for (auto & [key,value] : m_templateGroups) {
            std::cout<<"===> key="<<key<<" value="<<value.first<<" type="<<value.second->getTemplateType()<<std::endl;
            // search for group in map
            // identify template type
            // inference ??
        }
    }
    return argumentStr;
}
#endif

enum template_type getContainerType(const cppast::cpp_template_instantiation_type& p)
{
    return templateToContainerMap.at(p.primary_template().name());
}

void TypeDescriptor::parseTemplateInstanciation(const cppast::cpp_entity_index& index, const cppast::cpp_type & p, TypeDescriptorInfo & info)
{
    // to fully parse a template, all headers (std::vector, others...) must be parsed before to have the template types indexed with cppast
    // to avoid such parsing, pattern recognition or index fill function should be implemented
    std::string typeKind;
    std::string derefTypeKind;
    bool exposed = false;
    std::reference_wrapper<const cppast::cpp_type> innerType = (p);
    // howto handle recursive template inst ? Parameter in parameter maybe or templateDescriptor ?
    // will support container<T,R> or container<T> where T/R can be one of a std::tuple, an SRef or an SRef<std::tuple>
    // composed containers will not be supported, as it involves performance question to serialize vectors of vectors or map of vectors and so on ...
    // Meeting such use cases could mean a need for dedicated storage service
    std::cout<<"Parsing template instanciation"<<std::endl;
    auto& templateInstType = static_cast<const cppast::cpp_template_instantiation_type&>(p);
    auto& templateType = static_cast<const cppast::cpp_dependent_type&>(p);
    typeKind = "Template instanciation defined type";

    derefTypeKind = templateInstType.primary_template().name();
    innerType = templateType.dependee();
    auto ids = templateInstType.primary_template().id();
    exposed = templateInstType.arguments_exposed();

    for (auto & id : ids) {
        if (index.lookup(id).has_value()) {
            auto & entity = index.lookup(id).value();
            if (entity.scope_name().has_value()) {
                std::cout<<"Scope name "<<entity.scope_name().value().name()<<std::endl;
            }
        }
    }

    // bIsLeaf = false;
    //derefTypeKind = templateType.name();
    std::cout<<" => type [ " << to_string(p) << " ]\n";
    std::cout<<" => type kind [ " << typeKind << " ]\n";
    std::cout<<" => derefed type [ " <<derefTypeKind<<" ]\n";
    std::cout<<" => arguments are ";
    info.m_kind = type_kind::template_t;
    info.m_typename = templateInstType.primary_template().name();
    if (isContainer(templateInstType)) {
        info.m_templateType = getContainerType(templateInstType);
    }
    if (!exposed) {
#ifdef CPPAST_TPLARGS_NOTEXPOSED
        const std::string & args = templateInstType.unexposed_arguments();
        std::cout<<args<<std::endl;
        // I wrote a template argument parser to fix current miss in cppast
        std::string tmplArgs = parseTemplateArguments(args);
        linkArgumentsType(tmplArgs, m_templateGroups);
#endif
    } else {
        std::cout<<" exposed"<<std::endl;
        // NOTE : template arguments were not exposed as of current cppast version, hence the following code can't work for now !
        // my cppast fork contains template arguments parsing, hence this code works only with the forked version
        auto tmplArgs =  templateInstType.arguments();
        if (tmplArgs.has_value()) {
            for (auto & argument : tmplArgs.value()) {
                // parse each argument
                std::cout<<"=> "<<cppast::to_string(argument.type().value())<<std::endl;
                info.m_tmplArgsVector.push_back(std::make_shared<TypeDescriptor>(parseType(index, argument.type().value())));
            }
        }
    }
}

UniqueRef<TypeDescriptor::TypeDescriptorInfo> TypeDescriptor::parseType(const cppast::cpp_entity_index& index, const cppast::cpp_type & p)
{
    std::string constParam = "not const";
    std::string typeKind;
    std::string derefTypeKind;
    std::reference_wrapper<const cppast::cpp_type> innerType = (p);
    std::reference_wrapper<const cppast::cpp_type> currentType = (p);
    UniqueRef<TypeDescriptorInfo> info = xpcf::unixpcf::make_unique<TypeDescriptorInfo>(p);
    bool bIsLeaf = true;
    do {
        if (isCppFixedWidthType(currentType)) {
            info->m_typename = cppast::to_string(currentType);
            info->m_builtinType = cppFixedWidthType(currentType);
            info->m_kind = type_kind::builtin_t;
            info->m_isBuiltin = true;
        }
        if (isStdString(currentType)) {
            bIsLeaf = true;
            info->m_typename = "std::string";
            info->m_kind = type_kind::std_string_t;
        }
        if (currentType.get().kind() == cppast::cpp_type_kind::pointer_t) {
            if (m_foundConst.size() == 0) {
                info->m_const = false;
            }
            m_foundConst.clear();
            info->m_isReference = true;
            info->m_isPointer = true;
            typeKind += " pointer";
            // cast to cpp_pointer_type
            auto& pointerType = static_cast<const cppast::cpp_pointer_type&>(currentType.get());
            innerType = pointerType.pointee();
            derefTypeKind = to_string(innerType);
            bIsLeaf = false;
        }
        if (currentType.get().kind() == cppast::cpp_type_kind::array_t) {
            typeKind += " array";
            // cast to cpp_array_type
            auto& arrayType = static_cast<const cppast::cpp_array_type&>(currentType.get());
            innerType = arrayType.value_type();
            derefTypeKind = to_string(innerType);
            bIsLeaf = false;
        }
        if (currentType.get().kind() ==  cppast::cpp_type_kind::reference_t) {
            // cast to cpp_reference_type
            auto& refType = static_cast<const cppast::cpp_reference_type&>(currentType.get());
            if (refType.reference_kind() == cppast::cpp_reference::cpp_ref_lvalue) {
                info->m_isReference = true;
                typeKind += "lvalue ref";
                m_foundRef.push_back(true);
            }
            else if (refType.reference_kind() == cppast::cpp_reference::cpp_ref_rvalue) {
                typeKind += "rvalue ref";
            }
            innerType = refType.referee();
            derefTypeKind = to_string(innerType);
            bIsLeaf = false;
        }
        if (currentType.get().kind() ==  cppast::cpp_type_kind::cv_qualified_t) {
            typeKind += " cv_qualified";
            // cast to cpp_cv_qualified_type
            auto& cvType = static_cast<const cppast::cpp_cv_qualified_type&>(currentType.get());
            innerType = cvType.type();
            auto cvQual = cvType.cv_qualifier();
            derefTypeKind = to_string(innerType);
            bIsLeaf = false;
            if (is_const(cvQual)) {
                constParam = "const";
                m_foundConst.push_front(true);
            }
        }
        if (currentType.get().kind() ==  cppast::cpp_type_kind::builtin_t) {
            auto& builtinType = static_cast<const cppast::cpp_builtin_type&>(currentType.get());
            if (builtinType.builtin_type_kind() != cppast::cpp_builtin_type_kind::cpp_void) {
                info->m_void = false;
            }
            derefTypeKind = to_string(builtinType.builtin_type_kind());
            info->m_typename = cppast::to_string(currentType.get());
            info->m_builtinType = static_cast<cpp_builtin_type>(builtinType.builtin_type_kind());
            info->m_kind = type_kind::builtin_t;
            info->m_isBuiltin = true;
            bIsLeaf = true;
        }
        else {
            info->m_void = false;
        }
        if (currentType.get().kind() ==  cppast::cpp_type_kind::user_defined_t &&
                !isCppFixedWidthType(currentType) &&
                !isStdString(currentType)) {
            bIsLeaf = true;
            info->m_kind = type_kind::user_defined_t;
            auto & userType = static_cast<const cppast::cpp_user_defined_type&>(currentType.get());
            derefTypeKind = "User defined type";
            info->m_typename = userType.entity().name();
            if (userType.entity().no_overloaded().get() > 0) {
                auto ids = userType.entity().id();
                for (auto & id : ids) {
                    if (index.lookup(id).has_value()) {
                        auto & entity = index.lookup(id).value();
                        if (entity.kind() == cppast::cpp_entity_kind::type_alias_t) {
                            std::cout<<"Entity "<<entity.name()<<" is a type alias"<<std::endl;
                        }
                    }
                }

                auto vec =  userType.entity().get(index);
                if (vec.size() >0) {
                    auto&  indexedEntity = userType.entity().get(index)[0u];
                    indexedEntity->kind();
                    const cppast::cpp_entity&  entity = userType.entity().get(index)[0u].get();
                    cppast::cpp_entity_kind knd = entity.kind();
                    if (entity.kind() == cppast::cpp_entity_kind::enum_t) {
                        derefTypeKind = "Enum type";
                        info->m_kind = type_kind::enum_t;
                    }
                }
            }
        }
        std::cout<< "=> constness [ " <<constParam << " ]\n";

        if (currentType.get().kind() ==  cppast::cpp_type_kind::template_instantiation_t) {
            bIsLeaf = true;
            parseTemplateInstanciation(index,currentType, *info);
        }
        else {
            std::cout<<" => type [ " << cppast::to_string(currentType) << " ]\n";
            std::cout<<" => type kind [ " << typeKind << " ]\n";
            std::cout<<" => derefed type [ " <<derefTypeKind<<" ]\n";
            if (cppast::to_string(currentType) == derefTypeKind) {
                bIsLeaf = true;
            }
        }
        currentType = innerType;
    }
    while(!bIsLeaf);
    if (m_foundConst.size() < m_foundRef.size()) {
        // found less const than ref : type is not fully const
        info->m_const = false;
    }
    else {
        info->m_const = true;
    }
    if (info->m_isBuiltin && info->m_const && info->m_isPointer && info->m_builtinType == cpp_char) {
        info->m_isBuiltin = false;
        info->m_kind = type_kind::c_string_t;
    }
    return info;
    /*if (!bIsLeaf) {
        parseType(index,innerType);
    }*/
}

void TypeDescriptor::parse(const cppast::cpp_entity_index& index, const cppast::cpp_type & p)
{
    m_descriptorInfo = parseType(index,p);
    std::cout<<getFullTypeDescription()<<std::endl;
}
