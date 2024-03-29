#include "ASTParser.h"
#include "RemakenPathHelper.h"
#include <cppast/visitor.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cppast/cpp_class_template.hpp>
#include <cppast/cpp_variable.hpp>
#include <cppast/cpp_expression.hpp>
#include <xpcf/api/IComponentManager.h>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

namespace xpcf = org::bcom::xpcf;

template<> ASTParser * xpcf::ComponentFactory::createInstance<ASTParser>();

ASTParser::ASTParser():xpcf::ComponentBase(xpcf::toMap<ASTParser>())
{
    declareInterface<ITypeParser>(this);

    m_simpleParser = xpcf::unixpcf::make_unique<cppast::simple_file_parser<cppast::libclang_parser>>(type_safe::ref(m_index));

    m_fileParser = xpcf::unixpcf::make_unique<cppast::libclang_parser>(type_safe::ref(m_logger));
}


ASTParser::~ASTParser()
{

}

void ASTParser::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    delete this;
    return;
}

int ASTParser::set_compile_options(const cxxopts::ParseResult & options, cppast::libclang_compile_config & config)
{
    if (options.count("verbose"))
        config.write_preprocessed(true);

    if (options.count("fast_preprocessing"))
        config.fast_preprocessing(true);

    if (options.count("remove_comments_in_macro"))
        config.remove_comments_in_macro(true);

    if (options.count("include_directory"))
        for (auto& include : options["include_directory"].as<std::vector<std::string>>())
            config.add_include_dir(include);
    if (options.count("macro_definition"))
        for (auto& macro : options["macro_definition"].as<std::vector<std::string>>())
        {
            auto equal = macro.find('=');
            auto name  = macro.substr(0, equal);
            if (equal == std::string::npos)
                config.define_macro(std::move(name), "");
            else
            {
                auto def = macro.substr(equal + 1u);
                config.define_macro(std::move(name), std::move(def));
            }
        }
    if (options.count("macro_undefinition"))
        for (auto& name : options["macro_undefinition"].as<std::vector<std::string>>())
            config.undefine_macro(name);
    if (options.count("feature"))
        for (auto& name : options["feature"].as<std::vector<std::string>>())
            config.enable_feature(name);

    // the compile_flags are generic flags
    cppast::compile_flags flags;
    if (options.count("gnu_extensions"))
        flags |= cppast::compile_flag::gnu_extensions;
    if (options.count("msvc_extensions"))
        flags |= cppast::compile_flag::ms_extensions;
    if (options.count("msvc_compatibility"))
        flags |= cppast::compile_flag::ms_compatibility;

    if (options["std"].as<std::string>() == "c++98")
        config.set_flags(cppast::cpp_standard::cpp_98, flags);
    else if (options["std"].as<std::string>() == "c++03")
        config.set_flags(cppast::cpp_standard::cpp_03, flags);
    else if (options["std"].as<std::string>() == "c++11")
        config.set_flags(cppast::cpp_standard::cpp_11, flags);
    else if (options["std"].as<std::string>() == "c++14")
        config.set_flags(cppast::cpp_standard::cpp_14, flags);
    else if (options["std"].as<std::string>() == "c++1z")
        config.set_flags(cppast::cpp_standard::cpp_1z, flags);
    else
    {
        std::cerr << "invalid value '" + options["std"].as<std::string>() + "' for std flag" << '\n';
        return 1;
    }
    return 0;
}

int ASTParser::initOptions(const cxxopts::ParseResult & options)
{
    fs::detail::utf8_codecvt_facet utf8;
    if (options.count("database_dir") && !options["database_dir"].as<std::string>().empty())
    {
        cppast::libclang_compilation_database database(options["database_dir"].as<std::string>());

        if (options.count("database_file")) {
            m_config = cppast::libclang_compile_config(database, options["database_file"].as<std::string>());
        }
        else if (options.count("file") && !options["file"].as<std::string>().empty()) {
            m_config = cppast::libclang_compile_config(database, options["file"].as<std::string>());
        }
    }

    if (options.count("verbose")) {
        m_logger.set_verbose(true);
    }

    if (options.count("name")) {
        m_metadata[IRPCGenerator::MetadataType::PROJECT_NAME] = options["name"].as<std::string>();
    }
    xpcf::uuids::random_generator gen;
    xpcf::uuids::uuid moduleUUID = gen();
    std::string moduleUUIDStr = xpcf::uuids::to_string(moduleUUID);
    if (options.count("module_uuid")) {
        moduleUUIDStr = options["module_uuid"].as<std::string>();
    }
    m_metadata[IRPCGenerator::MetadataType::MODULE_UUID] = moduleUUIDStr;
    m_metadata[IRPCGenerator::MetadataType::MODULE_NAME] = "xpcfGrpcRemoting" +  m_metadata[IRPCGenerator::MetadataType::PROJECT_NAME];
    m_metadata[IRPCGenerator::MetadataType::MODULE_DESCRIPTION] = "xpcf remoting module for project " + m_metadata[IRPCGenerator::MetadataType::PROJECT_NAME];

    if (options.count("project_version")) {
        m_metadata[IRPCGenerator::MetadataType::PROJECT_VERSION] = options["project_version"].as<std::string>();
    }

    if (options.count("name") && options.count("project_version") && options.count("repository") && options.count("url")) {
        m_metadata[IRPCGenerator::MetadataType::PROJECT_DEPENDENCY_URL] = options["name"].as<std::string>() + "|"
                + options["project_version"].as<std::string>() + "|"+ options["name"].as<std::string>() + "|"
                + options["repository"].as<std::string>()+ "|"+  options["url"].as<std::string>();
    }

    fs::path interfacesAbsolutePath = RemakenPathHelper::getHomePath();
    if (options.count("interfaces_folder")) {
        interfacesAbsolutePath /= options["interfaces_folder"].as<std::string>();
    }
    m_interfacesAbsolutePath = interfacesAbsolutePath.generic_string(utf8);
    if (m_interfacesAbsolutePath.back() != fs::path::separator) {
        m_interfacesAbsolutePath += fs::path::separator;
    }
    return ASTParser::set_compile_options(options,m_config);
}

std::string ASTParser::computeNamespace()
{
    std::string nspace;
    uint32_t nspaceIdx = 0;
    for (auto & n : m_currentNamespace) {
        if (nspaceIdx > 0) {
            nspace += "::";
        }
        nspace += n;
        nspaceIdx ++;
    }
    return nspace;
}

bool isXpcfTrait(const cppast::cpp_class_template_specialization& c)
{
    return (c.name() == "InterfaceTraits") || (c.name() == "ComponentTraits");
}

std::string retrieveXpcfTraitType(const cppast::cpp_class_template_specialization& c)
{
    return c.unexposed_arguments().as_string();
}


void ASTParser::parseXpcfTrait(std::ostream& out, const cppast::cpp_class_template_specialization& c)
{
    out << "Found class_template_specialization_t defined type = "<< c.name()<<'\n';

    // There is only one argument for this template class, and it's the interface typename
    std::string xpcfTraitTypeName = retrieveXpcfTraitType(c);
    out<< "Found xpcf trait type = " << xpcfTraitTypeName<<'\n';
    if (xpcfTraitTypeName.find("::") == 0) {
        xpcfTraitTypeName.erase(0,2);
    }
    const cppast::cpp_class & cppastClass = c.class_();
    std::string traitsClassName = cppastClass.name();
    out << "Class specialized = "<< traitsClassName <<'\n';
    for (const cppast::cpp_entity & ec: cppastClass) {
        if (ec.kind() == cppast::cpp_entity_kind::variable_t) { // static variable are not considered members of the class ...
            auto & varEntity = static_cast<const cppast::cpp_variable &>(ec);
            out<< "Found variable " << ec.name()<<'\n';
            std::string exprValue;
            if (varEntity.default_value()) {
                const cppast::cpp_expression& expr = varEntity.default_value().value();

                switch (expr.kind())
                {
                case cppast::cpp_expression_kind::literal_t:
                    exprValue = static_cast<const cppast::cpp_literal_expression&>(varEntity.default_value().value()).value();
                    break;
                case cppast::cpp_expression_kind::unexposed_t:
                    auto & unexpExpr = static_cast<const cppast::cpp_unexposed_expression&>(varEntity.default_value().value());
                    exprValue = unexpExpr.expression().as_string();
                    break;
                }
                out<< " value = " << exprValue <<'\n';
                boost::algorithm::erase_all(exprValue,"\"");
                if (ec.name() == "UUID") {
                    m_xpcfTraits[xpcfTraitTypeName].uuid = exprValue;
                }
                if (ec.name() == "NAME") {
                    m_xpcfTraits[xpcfTraitTypeName].name = exprValue;
                }
                if (ec.name() == "DESCRIPTION") {
                    m_xpcfTraits[xpcfTraitTypeName].description = exprValue;
                }
            }
        }
    }
}

void ASTParser::parseEntity(std::ostream& out, const cppast::cpp_entity& e, const std::string & filePath)
{
    // print name and the kind of the entity
    if (!e.name().empty())
        out << e.name();
    else
        out << "<anonymous>";
    out << " (" << cppast::to_string(e.kind()) << ")\n";

    // print whether or not it is a definition
    if (cppast::is_definition(e))
        out << " [definition]\n";

    // print number of attributes
    if (!e.attributes().empty())
        out << " [" << e.attributes().size() << " attribute(s)]\n";

    if (e.kind() == cppast::cpp_entity_kind::class_t) {
        out << " [" << e.name() << "] is class"<< '\n';
        std::string entityNspace = computeNamespace();
        std::string includePath = filePath;
        if (boost::algorithm::starts_with(includePath, m_interfacesAbsolutePath)) {
            boost::algorithm::erase_first(includePath, m_interfacesAbsolutePath);
        }
        SRef<ClassDescriptor> c = xpcf::utils::make_shared<ClassDescriptor>(e, entityNspace, includePath);
        c->parse(m_index);
        if (c->isInterface() && !c->ignored()) {
            m_interfaces.insert(std::make_pair(c->getFullName(),c));
        }
        else {
            if (!xpcf::mapContains(m_classes, c->getName())) {
                m_classes.insert(std::make_pair(c->getFullName(),c));
                out << "Found user defined type = "<<c->getFullName()<<'\n';
            }
        }
    }

    if (e.kind() == cppast::cpp_entity_kind::class_template_specialization_t) {
        auto & c = static_cast<const cppast::cpp_class_template_specialization&>(e);
        if (isXpcfTrait(c)) {
            parseXpcfTrait(out, c);
            /* std::string traitTypeName = retrieveXpcfTraitType(c);
            m_xpcfTraits[traitTypeName] = xpcfTraitInfo; // to handle later on : must be independent of parsing order
            if (xpcf::mapContains(m_interfaces,traitTypeName)) {
                auto desc = m_interfaces.at(traitTypeName);
                desc->setXpcfTrait(xpcfTraitInfo);
            }*/
        }
    }

    if (e.kind() == cppast::cpp_entity_kind::language_linkage_t)
        // no need to print additional information for language linkages
        out << '\n';
    else if (e.kind() == cppast::cpp_entity_kind::namespace_t)
    {
        // cast to cpp_namespace
        auto& ns = static_cast<const cppast::cpp_namespace&>(e);
        // print whether or not it is inline
        if (ns.is_inline())
            out << " [inline]";
        out << '\n';
    }
    else
    {
        // print the declaration of the entity
        // it will only use a single line
        // derive from code_generator and implement various callbacks for printing
        // it will print into a std::string
        class code_generator : public cppast::code_generator
        {
            std::string str_;                 // the result
            bool        was_newline_ = false; // whether or not the last token was a newline
            // needed for lazily printing them

        public:
            code_generator(const cppast::cpp_entity& e)
            {
                // kickoff code generation here
                cppast::generate_code(*this, e);
            }

            // return the result
            const std::string& str() const noexcept
            {
                return str_;
            }

        private:
            // called to retrieve the generation options of an entity
            generation_options do_get_options(const cppast::cpp_entity&,
                                              cppast::cpp_access_specifier_kind) override
            {
                // generate declaration only
                return code_generator::declaration;
            }

            // no need to handle indentation, as only a single line is used
            void do_indent() override {}
            void do_unindent() override {}

            // called when a generic token sequence should be generated
            // there are specialized callbacks for various token kinds,
            // to e.g. implement syntax highlighting
            void do_write_token_seq(cppast::string_view tokens) override
            {
                if (was_newline_)
                {
                    // lazily append newline as space
                    str_ += ' ';
                    was_newline_ = false;
                }
                // append tokens
                str_ += tokens.c_str();
            }

            // called when a newline should be generated
            // we're lazy as it will always generate a trailing newline,
            // we don't want
            void do_write_newline() override
            {
                was_newline_ = true;
            }

        } generator(e);
        // print generated code
        out << ": `" << generator.str() << '`' << '\n';
    }
}

void ASTParser::processPostAstParsingActions()
{
    // perform actions depending on full parsing result
    for (auto & [name,c] : getParsedInterfaces()) {
        if (xpcf::mapContains(m_xpcfTraits, name)) {
            c->setXpcfTrait(m_xpcfTraits.at(name));
        }
    }
}

void ASTParser::parseAst(std::ostream& out, const cppast::cpp_file& file)
{
    // print file name
    out << "AST for '" << file.name() << "':\n";
    std::string prefix; // the current prefix string
    std::vector<std::string> currentNamespace;
    // recursively visit file and all children
    cppast::visit(file, [&](const cppast::cpp_entity& e, cppast::visitor_info info) {
        if (e.kind() == cppast::cpp_entity_kind::file_t || cppast::is_templated(e)
                || cppast::is_friended(e))
            // no need to do anything for a file,
            // templated and friended entities are just proxies, so skip those as well
            // return true to continue visit for children
            return true;
        else if (info.event == cppast::visitor_info::container_entity_exit)
        {
            // we have visited all children of a container,
            // remove prefix
            prefix.pop_back();
            prefix.pop_back();
            if (e.kind() == cppast::cpp_entity_kind::namespace_t) {
                out << "leaving namespace "<< m_currentNamespace.back() <<"\n";
                m_currentNamespace.pop_back();
            }
        }
        else
        {
            out << prefix; // print prefix for previous entities
            // calculate next prefix
            if (info.last_child)
            {
                if (info.event == cppast::visitor_info::container_entity_enter)
                    prefix += "  ";
                out << "+-";

            }
            else
            {
                if (info.event == cppast::visitor_info::container_entity_enter)
                    prefix += "| ";
                out << "|-";
            }
            if (e.kind() == cppast::cpp_entity_kind::namespace_t) {
                m_currentNamespace.push_back(e.name());
            }
            parseEntity(out, e, file.name());
        }

        return true;
    });
}

// parse a file
int ASTParser::parse_file(const std::string& filename, bool fatal_error)
{
    // the parser is used to parse the entity
    // there can be multiple parser implementations
    // parse the file
    try
    {
        m_fileParsed = m_fileParser->parse(m_index, filename, m_config);
    }
    catch (cppast::libclang_error& ex)
    {
        std::cerr << "fatal libclang error: " << ex.what() << '\n';
        return 1;
    }

    if (fatal_error && m_fileParser->error()) {
        // a non-fatal parse error
        // error has been logged to stderr
        return 1;
    }

    if (!m_fileParsed) {
        return 2;
    }
    parseAst(std::cout, *m_fileParsed);
    processPostAstParsingActions();
    return 0;
}



void ASTParser::parse_database(const cppast::libclang_compilation_database& database, const cxxopts::ParseResult & options)
{
    struct data_t
    {
        UniqueRef<cppast::simple_file_parser<cppast::libclang_parser>>& parser;
        const cppast::libclang_compilation_database& database;
        const cxxopts::ParseResult options;
    } data {m_simpleParser, database, options};

    cppast::detail::for_each_file(database, &data, [](void* ptr, std::string file) {
        auto& data = *static_cast<data_t*>(ptr);

        cppast::libclang_compile_config config(data.database, file);
        ASTParser::set_compile_options(data.options,config);
        data.parser->parse(std::move(file), std::move(config));
    });
}


int ASTParser::parse_database(const std::string & databasePath, const cxxopts::ParseResult & options)
try
{

    cppast::libclang_compilation_database database(databasePath); // the compilation database

    // simple_file_parser allows parsing multiple files and stores the results for us
    try
    {
        parse_database(database,options); // parse all files in the database
    }
    catch (cppast::libclang_error& ex)
    {
        std::cerr << "fatal libclang error: " << ex.what() << '\n';
        return 1;
    }

    if (m_simpleParser->error()) {
        // a non-fatal parse error
        // error has been logged to stderr
        return 1;
    }
    for (auto& file : m_simpleParser->files()) {
        parseAst(std::cout,file);
    }
    processPostAstParsingActions();
    return 0;
}
catch (std::exception& ex)
{
    std::cerr << ex.what() << '\n';
    return 1;
}

