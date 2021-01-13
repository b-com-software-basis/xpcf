// Copyright (C) 2017-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <iostream>

#include <cxxopts.hpp>

#include <xpcf/api/IComponentManager.h>
#include <xpcf/core/helpers.h>
#include "GrpcServerManager.h"
#include <cstdlib>

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

int main(int argc, char* argv[])
{
    SRef<xpcf::IComponentManager> cmpMgr = xpcf::getComponentManagerInstance();
    cmpMgr->bindLocal<xpcf::IGrpcServerManager,xpcf::GrpcServerManager>();
    //cmpMgr->bindLocal<IRPCGenerator,GRPCFlatBufferGenerator>();



    cxxopts::Options option_list("xpcf_grpc_server",
                                 "xpcf_grpc_server - The commandline interface to the xpcf grpc server application.\n");
    option_list.add_options()
            ("h,help", "display this help and exit")
            ("v,version", "display version information and exit")
            ("f,file", "xpcf grpc server configuration file",
             cxxopts::value<std::string>());

    auto options = option_list.parse(argc, argv);
    if (options.count("help"))
        print_help(option_list);
    else if (options.count("version"))
    {
        std::cout << "xpcf_grpc_server version MYVERSION \n";
        std::cout << '\n';
    }
    else if (!options.count("file") || options["file"].as<std::string>().empty()) {
        print_error("missing one of file or database dir argument");
        return 1;
    }
    std::string file = options["file"].as<std::string>();
    cmpMgr->load(file.c_str());
    auto serverMgr = cmpMgr->resolve<xpcf::IGrpcServerManager>();
    serverMgr->bindTo<xpcf::IConfigurable>()->configure(file.c_str());
    char * serverURL = getenv("XPCF_GRPC_SERVER_URL");
    if (serverURL != nullptr) {
        serverMgr->bindTo<xpcf::IConfigurable>()->getProperty("server_address")->setStringValue(serverURL);
    }
    serverMgr->runServer();
    return 0;
}
