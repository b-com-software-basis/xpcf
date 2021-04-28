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


    SRef<xpcf::IComponentManager> cmpMgr = xpcf::getComponentManagerInstance();
    std::string file = options["file"].as<std::string>();
    cmpMgr->load(file.c_str());
    auto guitarist = cmpMgr->resolve<IGuitarist>();
    auto guitar = cmpMgr->resolve<IGuitar>();
    auto electricGuitar = cmpMgr->resolve<IElectricGuitar>();
    auto human = cmpMgr->resolve<IHuman>();
    auto musician = cmpMgr->resolve<IMusician>();
    std::cout<<"Calling playsolo on empty score"<<std::endl;
    guitarist->playSolo(m);
    std::cout<<"Calling playScore on empty vector"<<std::endl;
    guitarist->playScore(m.m_score);
    m.add("A4",0.4);
    m.add("C5",0.2);
    m.add("E5",0.4);
    std::cout<<"Calling playsolo on score with notes"<<std::endl;
    guitarist->playSolo(m);
    std::cout<<"Calling playModifyScore on score with notes"<<std::endl;
    guitarist->playModifyScore(m.m_score);
    std::cout<<"Score after playModifyScore called"<<std::endl;
    for (auto & [key, duration] :  m.m_score) {
        std::cout<<"key="<<key<<" duration="<<duration<<std::endl;
    }
    human->sleep();
    musician->playMusic(m);
    std::cout<<"Guitar brand "<<guitar->getGuitarBrand()<<std::endl;
    std::cout<<"Guitar nb strings "<<guitar->getNbStrings()<<std::endl;
     std::cout<<"Guitar tune type "<<guitar->getTuneType()<<std::endl;

    return 0;
}
