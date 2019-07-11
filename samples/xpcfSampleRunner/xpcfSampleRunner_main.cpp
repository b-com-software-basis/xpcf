
#include "IMessage.h"
#include <xpcf/xpcf.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>

namespace po = boost::program_options;
namespace xpcf=org::bcom::xpcf;

using namespace std;
/**
  * Declare module.
  */
int main(int argc, char ** argv)
{
    po::options_description optionsDesc{"Usage"};
    po::variables_map optionsVars;
    std::string componentUuid, xmlFilePath;
    optionsDesc.add_options()
            ("help,h", "produce help message")
            ("file,f", po::value<string>(&xmlFilePath), "xpcf xml configuration file to load")
            ;
    po::store(po::command_line_parser(argc, argv).
              options(optionsDesc).run(), optionsVars);

    po::notify(optionsVars);

    if (optionsVars.count("help")) {
        cout << optionsDesc <<endl;
        return 0;
    }
    SRef<xpcf::IComponentManager> xpcfComponentManager = xpcf::getComponentManagerInstance();
    xpcf::XPCFErrorCode err = xpcfComponentManager->load(xmlFilePath.c_str());
    if (err != xpcf::XPCFErrorCode::_SUCCESS) {
        return -1;
    }
    SRef<IMessage> component = xpcfComponentManager->resolve<IMessage>();
    if (!component) {
         cout<<"Error : unable to resolve component for IMessage interface : check your xml file, it should contain at least one component implementing IMessage !"<<endl;
        return -1;
    }
    component->print();
    return 0;
}
