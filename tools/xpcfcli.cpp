/**
 * @copyright Copyright (c) 2015 All Right Reserved, B-com http://www.b-com.com/
 *
 * This file is subject to the B<>Com License.
 * All other rights reserved.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * @author Loïc Touraine
 *
 * @file
 * @brief description of file
 * @date 2015-09-18
 */

#include <xpcf/xpcf.h>
#include <xpcf/api/IModuleManager.h>
#include <xpcf/core/refs.h>
#include <boost/log/core.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

using namespace std;

namespace fs = boost::filesystem;
namespace xpcf  = org::bcom::xpcf;


static void usage(const char * appName)
{
    cout<<"Usage :"<<appName<<" -help : displays this help"<<endl;
    cout<<"\t-v : display version"<<endl;
    cout<<"\t-name [moduleName] : the module name to introspect"<<endl;
    cout<<"\t-path [modulePath]  : the module folder path      => "<<endl;
    cout<<"\t-o [xmlfilepath] : the output xml filename"<<endl;
}

void displayParameter(SRef<xpcf::IProperty> p)
{
    std::cout<<"Property name ="<<p->getName()<<std::endl;
    for (uint32_t i=0 ; i < p->size() ; i++ ) {
        switch (p->getType()) {
        case xpcf::IProperty::IProperty_NONE :
            break;
        case xpcf::IProperty::IProperty_INTEGER :
            std::cout<<"=> Int property = "<<p->getIntegerValue(i)<<std::endl;
            break;
        case xpcf::IProperty::IProperty_UINTEGER :
            std::cout<<"=> Uint property = "<<p->getUnsignedIntegerValue(i)<<std::endl;
            break;
        case xpcf::IProperty::IProperty_LONG :
            std::cout<<"=> Long property = "<<p->getLongValue(i)<<std::endl;
            break;
        case xpcf::IProperty::IProperty_ULONG :
            std::cout<<"=> ULong property = "<<p->getUnsignedLongValue(i)<<std::endl;
            break;
        case xpcf::IProperty::IProperty_CHARSTR :
            std::cout<<"=> String property = "<<p->getStringValue(i)<<std::endl;
            break;
        case xpcf::IProperty::IProperty_UNICODESTR :
            break;
        case xpcf::IProperty::IProperty_FLOAT :
            std::cout<<"=> Float property = "<<p->getFloatingValue(i)<<std::endl;
            break;
        case xpcf::IProperty::IProperty_DOUBLE :
            std::cout<<"=> Double property = "<<p->getDoubleValue(i)<<std::endl;
            break;
        case xpcf::IProperty::IProperty_STRUCTURE :
        {
            SRef<xpcf::IPropertyMap> propertyMap = p->getStructureValue(i);
            std::cout<<"Accessing class values for C0 from IProperty/IPropertyMap interfaces"<<std::endl;
            for (auto property : propertyMap->getProperties()) {
                displayParameter(property);
            }
            break;
        }
        default:
            break;
        }
    }
}


int main(int argc, char **argv) {
    int optionIndex = argc-2;
    map<string,string> options;
    options["-name"]="";
    options["-path"]="";
    options["-o"]="";
    std::string name,path,outputFilePath;

    while (optionIndex >= 1) {
        if (strcmp(argv[optionIndex],"-help")==0) {
            usage(argv[0]);
            return 0;
        }
        if (strcmp(argv[optionIndex],"-v")==0) {
            std::string version = MYVERSIONSTRING;
            cout<<version<<endl;
            return 0;
        }
        if (options.find(argv[optionIndex]) == options.end()) {
            cout<<"Unknown option :"<<argv[optionIndex]<<". "<<argv[0]<<" not started :"<<endl;
            usage(argv[0]);
            return -1;
        }
        options[argv[optionIndex]]=argv[optionIndex+1];
        optionIndex -= 2;
    }

    if (optionIndex == 0) {
        if (strcmp(argv[1],"-help")==0) {
            usage(argv[0]);
            return 0;
        }
        if (strcmp(argv[1],"-v")==0) {
            std::string version = MYVERSIONSTRING;
            cout<<version<<endl;
            return 0;
        }
        if (options.find(argv[1]) != options.end()) {
            cout<<"Missing parameter for option :"<<argv[1]<<". "<<argv[0]<<" not started :"<<endl;
            return -1;
        }
        cout<<"Unknown option :"<<argv[1]<<". "<<argv[0]<<" not started :"<<endl;
        usage(argv[0]);
        return -1;
    }
    xpcf::uuids::uuid iid_IComponentIntrospect= xpcf::toUUID<xpcf::IComponentIntrospect>();

    //Initialize default ssl key dir to root configuration dir
    name = options["-name"];
    path = options["-path"];
    outputFilePath = options["-o"];
    if (name.length() == 0 || path.length() == 0 || outputFilePath.length() == 0) {
        cout<<"Empty option. "<<argv[0]<<" not started :"<<endl;
        usage(argv[0]);
        return -1;
    }
    boost::log::core::get()->set_logging_enabled(false);
    SRef<xpcf::IModuleManager> xpcfModuleManager = xpcf::getModuleManagerInstance();
    SPtr<xpcf::ModuleMetadata> modMdata = xpcfModuleManager->introspectModule(name.c_str(), path.c_str());
    SRef<xpcf::IComponentIntrospect> rIComponentIntrospect;
    std::cout<<"Module name = '"<<name<<"'"<<std::endl;
    std::cout<<"            -> UUID = '"<<modMdata->getUUID()<<"'"<<std::endl;
    std::cout<<"            -> path = '"<<modMdata->getPath()<<"'"<<std::endl;
    std::cout<<"            -> fullpath = '"<<modMdata->getFullPath()<<"'"<<std::endl;

    std::cout<<"Trying to save module informations'"<<std::endl;

    xpcfModuleManager->saveModuleInformations(outputFilePath.c_str(),modMdata);

    std::cout<<"=> Component list "<<std::endl;
    for (xpcf::uuids::uuid componentUUID : modMdata->getComponents()) {
        xpcf::XPCFErrorCode res=xpcf::XPCFErrorCode::_SUCCESS;
        try {
            rIComponentIntrospect = xpcfModuleManager->createComponent(modMdata,componentUUID);
            std::cout<<"===> Component UUID='"<<componentUUID<<"'"<<std::endl;
            int nbInterfaces = rIComponentIntrospect->getNbInterfaces();
            for (xpcf::uuids::uuid interfaceUUID : rIComponentIntrospect->getInterfaces()) {
                std::cout<<"===> interface UUID='"<<interfaceUUID<<"' Description='"<<rIComponentIntrospect->getDescription(interfaceUUID)<<"'"<<std::endl;
            }

            if (rIComponentIntrospect->implements<xpcf::IConfigurable>()) {
                SRef<xpcf::IConfigurable> rIConfigurable = rIComponentIntrospect->bindTo<xpcf::IConfigurable>();
                map<xpcf::uuids::uuid,SRef<xpcf::IPropertyMap>> paramsMap;
                if (rIConfigurable->hasProperties()) {
                    paramsMap[componentUUID] = rIConfigurable->getPropertyRootNode();
                    rIConfigurable->serialize(outputFilePath.c_str(),0);
                }

                for (auto property : rIConfigurable->getProperties()) {
                    if (rIConfigurable->getProperties().size() == 0) {
                        std::cout<<"ERROR : no movenext "<<std::endl;
                        break;
                    }
                    else {
                        displayParameter(property);
                    }
                }
            }
            SRef<xpcf::IInjectable> rInject = rIComponentIntrospect->bindTo<xpcf::IInjectable>();
            for (auto inj : rInject->getInjectables()) {

            }
        }
        catch (std::invalid_argument & e) {
            std::cout<<"ERROR : invalid argument "<<e.what()<<std::endl;
        }
        /*        int nbInterfaces = rIComponentIntrospect->getNbInterfaces();
        std::vector<xpcf::uuids::uuid>  uuidArray;
        uuidArray.resize(nbInterfaces);
        rIComponentIntrospect->getInterfaces(uuidArray.data());
        for (uint32_t j = 0 ; j<nbInterfaces ; j++) {
            std::cout<<"===> interface["<<i<<"] UUID='"<<uuidArray[j]<<"' Description='"<<rIComponentIntrospect->getDescription(uuidArray[j])<<"'"<<std::endl;
        }*/
    }

    return 0;
}


