// Copyright (C) 2017-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <iostream>

#include <cxxopts.hpp>

#include <xpcf/api/IComponentManager.h>
#include <xpcf/core/helpers.h>
#include "IGuitarist.h"
#include "IGuitar.h"
#include "IHuman.h"
#include "IMusician.h"


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
    cxxopts::Options option_list("xpcf_grpc_client",
                                 "xpcf_grpc_client - The commandline interface to the xpcf grpc client test application.\n");
    option_list.add_options()
            ("h,help", "display this help and exit")
            ("v,version", "display version information and exit")
            ("f,file", "xpcf grpc client configuration file",
             cxxopts::value<std::string>());

    auto options = option_list.parse(argc, argv);
    if (options.count("help"))
        print_help(option_list);
    else if (options.count("version"))
    {
        std::cout << "xpcf_grpc_client version MYVERSION \n";
        std::cout << '\n';
    }
    else if (!options.count("file") || options["file"].as<std::string>().empty()) {
        print_error("missing one of file or database dir argument");
        return 1;
    }
    MusicScore m;
    m.add("A4",0.4);
    m.add("C5",0.2);
    m.add("E5",0.4);

    SRef<xpcf::IComponentManager> cmpMgr = xpcf::getComponentManagerInstance();
    std::string file = options["file"].as<std::string>();
    cmpMgr->load(file.c_str());
    auto guitarist = cmpMgr->resolve<IGuitarist>();
   /* auto guitar = cmpMgr->resolve<IGuitar>();
    auto electricGuitar = cmpMgr->resolve<IElectricGuitar>();
    auto human = cmpMgr->resolve<IHuman>();
    auto musician = cmpMgr->resolve<IMusician>();*/
    guitarist->playSolo(m);
  /*  human->sleep();
    musician->playMusic(m);
    std::cout<<"Guitar brand "<<guitar->getGuitarBrand()<<std::endl;
    std::cout<<"Guitar nb strings "<<guitar->getNbStrings()<<std::endl;
    std::cout<<"ElectricGuitar brand "<<electricGuitar->bindTo<IGuitar>()->getGuitarBrand()<<std::endl;
    std::cout<<"ElectricGuitar nb strings "<<electricGuitar->bindTo<IGuitar>()->getNbStrings()<<std::endl;*/

    return 0;
}
