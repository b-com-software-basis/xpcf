// Copyright (C) 2017-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <iostream>

#include <cxxopts.hpp>

#include <xpcf/api/IComponentManager.h>
#include <xpcf/core/helpers.h>
#include "GrpcServerManager.h"
#include <cstdlib>
#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

namespace fs = boost::filesystem;

namespace xpcf = org::bcom::xpcf;

// print help options
void print_help(const cxxopts::Options& options)
{
    std::cout << options.help({""}) << '\n';
}

// print error message
void print_error(const std::string& msg)
{
    std::cerr << msg << '\n';
}

void tryConfigureServer(SRef<xpcf::IGrpcServerManager> server, const std::string & propName, const std::string & envVarName)
{
    char * envValue = getenv(envVarName.c_str());
    if (envValue != nullptr) {
        xpcf::IProperty::PropertyType type = server->bindTo<xpcf::IConfigurable>()->getProperty(propName.c_str())->getType();
        switch (type) {
        case xpcf::IProperty::PropertyType::IProperty_CHARSTR:
            server->bindTo<xpcf::IConfigurable>()->getProperty(propName.c_str())->setStringValue(envValue);
            break;

        case xpcf::IProperty::PropertyType::IProperty_UINTEGER:
            server->bindTo<xpcf::IConfigurable>()->getProperty(propName.c_str())->setUnsignedIntegerValue(std::atoi(envValue));
            break;

        case xpcf::IProperty::PropertyType::IProperty_LONG:
            server->bindTo<xpcf::IConfigurable>()->getProperty(propName.c_str())->setLongValue(std::atol(envValue));
        break;
        default:
            std::cout<<"GrpcServerManager Property type not handled"<<std::endl;
            break;
        }
    }
    else {
        std::cout<<"No '"<<envVarName<<"' environment variable found";
    }
}

int main(int argc, char* argv[])
{
    fs::detail::utf8_codecvt_facet utf8;
    SRef<xpcf::IComponentManager> cmpMgr = xpcf::getComponentManagerInstance();
    cmpMgr->bindLocal<xpcf::IGrpcServerManager,xpcf::GrpcServerManager>();
    std::string configSrc;
    fs::path currentPath(boost::filesystem::initial_path().generic_string(utf8));
    configSrc = currentPath.generic_string(utf8);

    cxxopts::Options option_list("xpcf_grpc_server",
                                 "xpcf_grpc_server - The commandline interface to the xpcf grpc server application.\n");
    option_list.add_options()
            ("h,help", "display this help and exit")
            ("v,version", "display version information and exit")
            ("f,file", "xpcf grpc server configuration file",
             cxxopts::value<std::string>())
            ("d,folder", "xpcf grpc server configuration folder (every xpcf xml file in the folder is loaded)",
             cxxopts::value<std::string>());

    auto options = option_list.parse(argc, argv);
    if (options.count("help"))
        print_help(option_list);
    else if (options.count("version"))
    {
        std::cout << "xpcf_grpc_server version MYVERSION \n";
        std::cout << '\n';
    }
    else if ((!options.count("file") || options["file"].as<std::string>().empty())
             && (!options.count("folder") || options["folder"].as<std::string>().empty())) {
        print_error("missing one of file or folder argument, using " + configSrc + " folder as default");
        cmpMgr->load(configSrc.c_str(),false);
    }

    if (options.count("file") && !options["file"].as<std::string>().empty()) {
        configSrc = options["file"].as<std::string>();
        cmpMgr->load(configSrc.c_str());
    }
    if (options.count("folder") && !options["folder"].as<std::string>().empty()) {
        configSrc = options["folder"].as<std::string>();
        cmpMgr->load(configSrc.c_str(),false);
    }
    auto serverMgr = cmpMgr->resolve<xpcf::IGrpcServerManager>();
    tryConfigureServer(serverMgr, "server_address", "XPCF_GRPC_SERVER_URL");
    tryConfigureServer(serverMgr, "server_credentials", "XPCF_GRPC_CREDENTIALS");
    tryConfigureServer(serverMgr, "max_receive_message_size", "XPCF_GRPC_MAX_RECV_MSG_SIZE");
    tryConfigureServer(serverMgr, "max_send_message_size", "XPCF_GRPC_MAX_SEND_MSG_SIZE");

    std::cout<<"xpcf_grpc_server listens on: "<<serverMgr->bindTo<xpcf::IConfigurable>()->getProperty("server_address")->getStringValue()<<std::endl;
    serverMgr->runServer();
    return 0;
}
