/**
 * @copyright Copyright (c) 2020 B-com http://www.b-com.com/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author Loïc Touraine
 *
 * @file
 * @brief description of file
 * @date 2020-02-10
 */
#include <iostream>

#include <cxxopts.hpp>

#include <cppast/code_generator.hpp>         // for generate_code()
#include <cppast/cpp_entity_kind.hpp>        // for the cpp_entity_kind definition
#include <cppast/cpp_forward_declarable.hpp> // for is_definition()
#include <cppast/cpp_namespace.hpp>          // for cpp_namespace
#include <cppast/libclang_parser.hpp> // for libclang_parser, libclang_compile_config, cpp_entity,...
#include <cppast/visitor.hpp>         // for visit()
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_variable.hpp>
#include <cppast/cpp_template.hpp>
#include <cppast/cpp_array_type.hpp>
#include "ClassDescriptor.h"
#include <xpcf/api/IComponentManager.h>
#include <xpcf/core/helpers.h>
#include "RemoteServiceGenerator.h"
#include "GRPCProtoGenerator.h"
#include "GRPCFlatBufferGenerator.h"
#include "ProxyGenerator.h"
#include "ServerGenerator.h"
#include "ProjectGenerator.h"
#include "XpcfConfigGenerator.h"
#include "ASTParser.h"
#include <boost/log/core/core.hpp>
namespace xpcf = org::bcom::xpcf;


/// Usage samples:
/// --database_file /path/to/database_dir/compile_commands.json --database_dir /path/to/database_dir/ --remove_comments_in_macro --std c++1z

//$ ./xpcf_grpc_gen -n xpcfSampleComponent -v 2.6.3 -r @github -u https://github.com/b-com-software-basis/xpcf/releases/releases/download --std c++1z --database_dir ../../../xpcf/samples/build-xpcfSampleComponent-Desktop_Qt_5_12_5_clang_64bit-Debug/ --remove_comments_in_macro -o ~/tmp/grpc_gen_folder_sampleComponent -g protobuf

//$ ./xpcf_grpc_server -f ~/tmp/grpc_gen_folder_sampleComponent/xpcfGrpcRemotingxpcfSampleComponent.xml
// note server URL should be available either in xpcf conf or in XPCF_GRPC_SERVER_URL env var : to add to grpc_server for env var
//default url is 0.0.0.0:50051
//$ ./xpcf_grpc_client -f ../../../xpcfGrpcSampleComponentClient.xml

//$ ./xpcf_grpc_gen -n nom_projet -v version_projet -r @github -u url_projet –std c++1z –database_dir chemin/vers/compilation_database/ –remove_comments_in_macro -o ~/tmp/grpc_gen_folder_sampleComponent -g protobuf

//$ ./xpcf_grpc_gen -n SolARFramework -v 0.9.0 -r SolARBuild@github -u https://github.com/SolarFramework/SolARFramework —database_dir /home/ltouraine/builds/build-SolARFramework-Desktop_Qt_5_12_6_GCC_64bit-Debug/ –std c++1z —remove_comments_in_macro -g protobuf -o ~/grpcGenSolar


//Linux sample grpc process:
//server side
//. ~/workspace/linux-github/linux-xpcf/scripts/set_linux_env.sh
//export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/ltouraine/.conan/data/boost/1.74.0/_/_/package/eeef031d45a4b3fccb233a8660e3255a183c617c/lib:~/.remaken/packages/linux-gcc/xpcf/2.6.3/lib/x86_64/shared/debug/
//export XPCF_GRPC_SERVER_URL=0.0.0.0:34123
//./xpcf_grpc_server -f xpcfGrpcSampleComponentServer.xml

//client side
//. ~/workspace/linux-github/linux-xpcf/scripts/set_linux_env.sh
//./xpcf_grpc_client -f xpcfGrpcSampleComponentClient.xml
// print help options
void print_help(const cxxopts::Options& options)
{
    std::cout << options.help({"", "parsing", "generation"}) << '\n';
}

// print error message
void print_error(const std::string& msg)
{
    std::cerr << msg << '\n';
}

SRef<xpcf::IComponentManager> bindXpcfComponents() {
    SRef<xpcf::IComponentManager> cmpMgr = xpcf::getComponentManagerInstance();
#ifdef XPCF_NAMEDINJECTIONAPPROACH
    // global component holding all sub components approach with named injections:
    cmpMgr->bindLocal<IRPCGenerator, RemoteServiceGenerator, xpcf::IComponentManager::Singleton>();
    cmpMgr->bindLocal<IRPCGenerator, ProxyGenerator>("proxy");
    cmpMgr->bindLocal<IRPCGenerator, ServerGenerator>("server");
    cmpMgr->bindLocal<IRPCGenerator, ProjectGenerator>("project");
#else
    // chained injection through base class composite approach :
    cmpMgr->bindLocal<IRPCGenerator, RemoteServiceGenerator, xpcf::BindingScope::Singleton, xpcf::BindingRange::Named|xpcf::BindingRange::Explicit>("service");
    cmpMgr->bindLocal<GRPCProtoGenerator, IRPCGenerator, ProxyGenerator, xpcf::BindingScope::Transient, xpcf::BindingRange::Explicit|xpcf::BindingRange::Default>();
    cmpMgr->bindLocal<GRPCFlatBufferGenerator, IRPCGenerator, ProxyGenerator, xpcf::BindingScope::Transient, xpcf::BindingRange::Explicit|xpcf::BindingRange::Default>();
    cmpMgr->bindLocal<ProxyGenerator, IRPCGenerator, ServerGenerator, xpcf::BindingScope::Transient, xpcf::BindingRange::Explicit|xpcf::BindingRange::Default>();
    cmpMgr->bindLocal<ServerGenerator, IRPCGenerator, ProjectGenerator, xpcf::BindingScope::Transient, xpcf::BindingRange::Explicit|xpcf::BindingRange::Default>();
    cmpMgr->bindLocal<ProjectGenerator, IRPCGenerator, XpcfConfigGenerator, xpcf::BindingScope::Transient, xpcf::BindingRange::Explicit|xpcf::BindingRange::Default>();
#endif
    cmpMgr->bindLocal<ITypeParser, ASTParser, xpcf::BindingScope::Singleton>("astParser");
    return cmpMgr;
}

int main(int argc, char* argv[])
try
{
    boost::log::core::get()->set_logging_enabled(false);
    SRef<xpcf::IComponentManager> cmpMgr = xpcf::getComponentManagerInstance();

    cxxopts::Options option_list("xpcf_grpc_gen",
                                 "xpcf_grpc_gen - The commandline interface to the grpc generators for xpcf.\n"
"Sample use:\n"
"1/ generate the compilation database from the framework project hosting the xpcf interfaces that will be remoted.\n"
"Within QT Creator for instance, use \"Build->Generate Compilation Database for [project]\"\n\n"
"2/ launch xpcf_grpc_gen with:\n"
"xpcf_grpc_gen -n [framework name] -v [framework version] -r @github -u [framework binary release github url] --std c++1z --database_dir [compilation database folder path] --remove_comments_in_macro -o [code generation destination folder path] -g protobuf -i [relative framework interfaces path beneath the project starting at $HOME - this path will be removed from #include directives]\n\n"
"Example: to generate grpc code from xpcf SampleComponent interfaces the command is (if the xpcf folder is located in the $HOME user folder):\n"
"xpcf_grpc_gen -n xpcfSampleComponent -v 2.6.3 -r @github -u https://github.com/b-com-software-basis/xpcf/releases/download --std c++1z --database_dir xpcf/samples/build-xpcfSampleComponent-Desktop_Qt_5_12_10_clang_64bit-Debug/ --remove_comments_in_macro -o ~/tmp/grpc_gen_sampleComponent -g protobuf -i xpcf/samples/sample_component");
        // clang-format off
    option_list.add_options()
            ("h,help", "display this help and exit")
            ("version", "display version information and exit")
            ("d,verbose", "be verbose when parsing")
            ("n,name", "the parsed project/framework name",
             cxxopts::value<std::string>())
            ("v,project_version", "the parsed project/framework version",
             cxxopts::value<std::string>())
            ("r,repository", "the project/framework host repository type [github,conan,system...] ",
             cxxopts::value<std::string>())
            ("u,url", "the project/framework host repository url",
             cxxopts::value<std::string>())
            ("fatal_errors", "abort program when a parser error occurs, instead of doing error correction")
            ("file", "the file that is being parsed",
             cxxopts::value<std::string>());
    option_list.add_options("parsing")
            ("database_dir", "set the directory where a 'compile_commands.json' file is located containing build information",
             cxxopts::value<std::string>())
            ("database_file", "set the file name whose configuration will be used regardless of the current file name",
             cxxopts::value<std::string>())
            ("std", "set the C++ standard (c++98, c++03, c++11, c++14, c++1z (experimental))",
             cxxopts::value<std::string>()->default_value(cppast::to_string(cppast::cpp_standard::cpp_latest)))
            ("I,include_directory", "add directory to include search path",
             cxxopts::value<std::vector<std::string>>())
            ("D,macro_definition", "define a macro on the command line",
             cxxopts::value<std::vector<std::string>>())
            ("U,macro_undefinition", "undefine a macro on the command line",
             cxxopts::value<std::vector<std::string>>())
            ("f,feature", "enable a custom feature (-fXX flag)",
             cxxopts::value<std::vector<std::string>>())
            ("gnu_extensions", "enable GNU extensions (equivalent to -std=gnu++XX)")
            ("msvc_extensions", "enable MSVC extensions (equivalent to -fms-extensions)")
            ("msvc_compatibility", "enable MSVC compatibility (equivalent to -fms-compatibility)")
            ("fast_preprocessing", "enable fast preprocessing, be careful, this breaks if you e.g. redefine macros in the same file!")
            ("remove_comments_in_macro", "whether or not comments generated by macro are kept, enable if you run into errors");
    option_list.add_options("generation")
            ("g,generator", "the message format to use : either 'flatbuffers' or 'protobuf'",
             cxxopts::value<std::string>()->default_value("flatbuffers"))
            ("m,module_uuid", "the UUID to use for the XPCF module generated",
             cxxopts::value<std::string>())
            ("i,interfaces_folder", "the relative interfaces root folder (starting from $HOME folder) for the project (this path will be removed from #include directives)",
             cxxopts::value<std::string>())
            ("o,output", "set the destination folder where the generated grpc files will be created. The folder is created if it doesn't already exists",
             cxxopts::value<std::string>());


    // clang-format on
    // waitForUserInput();
    auto options = option_list.parse(argc, argv);
    if (options.count("help"))
        print_help(option_list);
    else if (options.count("version"))
    {
        std::string version = MYVERSIONSTRING;
        std::cout << "xpcf_grpc_gen version "<< version <<std::endl;
        std::cout << "Copyright (C) Loïc Touraine 2019-2020\n";
        std::cout << '\n';
    }
    else {
        auto cmpMgr = bindXpcfComponents();

        if (options["generator"].as<std::string>() == "protobuf") {
#ifdef XPCF_NAMEDINJECTIONAPPROACH
            // global component holding all sub components approach with named injections:
            cmpMgr->bindLocal<IRPCGenerator, GRPCProtoGenerator>("grpc");
#else
            cmpMgr->bindLocal<RemoteServiceGenerator, IRPCGenerator, GRPCProtoGenerator, xpcf::BindingScope::Transient, xpcf::BindingRange::Explicit|xpcf::BindingRange::Default>();
#endif
        }
        else if (options["generator"].as<std::string>() != "flatbuffers") {
            print_error("invalid value " + options["generator"].as<std::string>() + " for generator option. See usage :");
            print_help(option_list);
            return 1;
        }
        else {
#ifdef XPCF_NAMEDINJECTIONAPPROACH
            // global component holding all sub components approach with named injections:
            cmpMgr->bindLocal<IRPCGenerator, GRPCFlatBufferGenerator>("grpc");
#else
            cmpMgr->bindLocal<RemoteServiceGenerator, IRPCGenerator, GRPCFlatBufferGenerator, xpcf::BindingScope::Transient, xpcf::BindingRange::Explicit|xpcf::BindingRange::Default>();
#endif
        }

#ifdef XPCF_NAMEDINJECTIONAPPROACH

        // every injectable is bound : able to resolve the serviceGenerator
        auto serviceGenerator = cmpMgr->resolve<IRPCGenerator>();
#else
        // every injectable is bound : able to resolve the serviceGenerator
        auto serviceGenerator = cmpMgr->resolve<IRPCGenerator>("service");
#endif
        if (options.count("output")) {
            serviceGenerator->setGenerateMode(IRPCGenerator::GenerateMode::FILE);
            serviceGenerator->setDestinationFolder(options["output"].as<std::string>());
        }

        auto astParser = cmpMgr->resolve<ITypeParser>("astParser");

        int result = astParser->initOptions(options);
        if (result !=0) {
            return result;
        }

        cppast::cpp_entity_index idx; // the entity index is used to resolve cross references in the AST

        if (!options.count("file") || options["file"].as<std::string>().empty()) {
            if (options.count("database_dir") && !options["database_dir"].as<std::string>().empty()) {
                std::cout<<"File argument is missing : parsing every file listed in database"<<std::endl;
                astParser->parse_database(options["database_dir"].as<std::string>(),options);
                //parse_database(options["database_dir"].as<std::string>(),idx,options, [&](const cppast::cpp_entity_index& idx, std::ostream& out, const cppast::cpp_file& file) { astParser->parseAst(idx,out,file); });
            }
            else {
                print_error("missing one of file or database dir argument");
                return 1;
            }
        }
        else {
            result = astParser->parse_file(options["file"].as<std::string>(), options.count("fatal_errors") == 1);
            if (result != 0)
                return result;
        }
        //update types : try to qualify non fqdn types in parameters ... from classes found during parsing
        for (auto & [name,c] : astParser->getParsedInterfaces()) {
            for (auto & m: c->methods()) {
                for (auto & p : m->m_params) {
                    std::string knownType = cppast::to_string(p->getCppastType());
                    std::string base = knownType.substr(0,knownType.find_first_of("::"));
                    auto typeFound = astParser->getClassInfo(base);
                    if (typeFound) {
                        p->setNamespace(typeFound->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE));
                    } else {
                        typeFound = astParser->getInterfaceInfo(base);
                        if (typeFound) {
                            p->setNamespace(typeFound->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE));
                        }
                    }
                }
                auto & r = m->returnType();
                std::string knownType = cppast::to_string(r.getCppastType());
                std::string base = knownType.substr(0,knownType.find_first_of("::"));
                auto typeFound = astParser->getClassInfo(base);
                if (typeFound) {
                    r.setNamespace(typeFound->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE));
                } else {
                    typeFound = astParser->getInterfaceInfo(base);
                    if (typeFound) {
                        r.setNamespace(typeFound->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE));
                    }
                }
            }
        }
        serviceGenerator->initialize(astParser->metadata());
        for (auto & [name,c] : astParser->getParsedInterfaces()) {
            // check every typedescriptor in each method of the interface is known in classes map or is a builtin type??
            // Note : we must find a message/serialized buffer for each type in each interface
            astParser->metadata() = serviceGenerator->generate(c, astParser->metadata());
            astParser->metadata() = serviceGenerator->validate(c,astParser->metadata());
        }
        serviceGenerator->finalize(astParser->metadata());
    }


}
catch (const cppast::libclang_error& ex)
{
    print_error(std::string("[fatal parsing error] ") + ex.what());
    return 2;
}
