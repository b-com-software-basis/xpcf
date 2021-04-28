/**
 * @copyright Copyright (c) 2017 B-com http://www.b-com.com/
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
 * @date 2018-06-25
 */

#include <stdio.h>
#include "VirtualGuitarist.h"
#include <iostream>
#include "xpcfSampleComponentAPI.h"
#include <xpcf/xpcf.h>

namespace xpcf = org::bcom::xpcf;

template<> component::VirtualGuitarist* xpcf::ComponentFactory::createInstance<component::VirtualGuitarist>();

namespace component {

VirtualGuitarist::VirtualGuitarist ():xpcf::ConfigurableBase(xpcf::toMap<VirtualGuitarist>()),m_name("VirtualGuitarist")
{
    declareInterface<IMusician>(this);
    declareInterface<IGuitarist>(this);
    declareInjectable<IGuitar>(m_guitar);
    declareInjectable<IElectricGuitar>(m_electricGuitar,"electricGuitar");
    declareInjectable<IElectricGuitar>(m_guitars);
    declareProperty("blurFactor",m_blurFactor);
    m_blurScales.resize(4);
    declarePropertySequence("blurScale",m_blurScales);
    declarePropertyNode("blurMatrixRepartition",m_blurMatrixRepartition);
    declareProperty(m_blurMatrixRepartition,"cols",m_cols);
    declareProperty(m_blurMatrixRepartition,"rows",m_rows);
    m_matrixValues.resize(4);
    declarePropertySequence(m_blurMatrixRepartition,"values",m_matrixValues);
    std::cout<<m_name<<"::"<<"Constructor VirtualGuitarist::VirtualGuitarist () called!"<<std::endl;
}

VirtualGuitarist::~VirtualGuitarist ()
{
    std::cout<<m_name<<"::"<<"Destructor VirtualGuitarist::VirtualGuitarist () called!"<<std::endl;
}

void VirtualGuitarist::onInjected()
{
    declareInjectable<IGuitar>(m_folkGuitar,"folkGuitar",true);
}

void VirtualGuitarist::unloadComponent ()
{
    std::cout<<m_name<<"::"<<"VirtualGuitarist::unload () called!"<<std::endl;

    delete this;
    return;
}

void VirtualGuitarist::playSolo(MusicScore & score)
{
    std::cout<<m_name<<"::"<<" VirtualGuitarist::playSolo ()  called!"<<std::endl;
    for (auto & [key,duration] : score.m_score) {
        std::cout<<"Playing key = "<< key << " duration = " << duration<<std::endl;
    }
    /*std::cout<<"Adding key = G4"<<std::endl;
    score.add("G4",0.4);*/
}

void VirtualGuitarist::playRhythm()
{

    std::cout<<m_name<<"::"<<"playRhythm ()  called!"<<std::endl;

}
void VirtualGuitarist::learn ()
{

    std::cout<<m_name<<"::"<<"learn ()  called!"<<std::endl;

}

void VirtualGuitarist::playMusic (const MusicScore & score)
{

    std::cout<<m_name<<"::"<<"playMusic ()  called!"<<std::endl;
    for (auto & [key,duration] : score.m_score) {
        std::cout<<"Playing key = "<< key << " duration = " << duration<<std::endl;
    }
}

void VirtualGuitarist::playScore (std::vector<std::pair<std::string, float>> & score)
{

    std::cout<<m_name<<"::"<<"playScore ()  called!"<<std::endl;
    for (auto & [key,duration] : score) {
        std::cout<<"Playing key = "<< key << " duration = " << duration<<std::endl;
    }
}

void VirtualGuitarist::playModifyScore (std::vector<std::pair<std::string, float>> & score)
{

    std::cout<<m_name<<"::"<<"playModifyScore ()  called!"<<std::endl;
    for (auto & [key,duration] : score) {
        std::cout<<"Playing key = "<< key << " duration = " << duration<<std::endl;
    }
    std::cout<<"Adding key = G4"<<std::endl;
    score.push_back(std::make_pair("G4",0.4));
}

void VirtualGuitarist::listen ()
{

    std::cout<<m_name<<"::"<<"listen ()  called!"<<std::endl;

}

void VirtualGuitarist::practice ()
{

    std::cout<<m_name<<"::"<<"practice ()  called!"<<std::endl;

}

void VirtualGuitarist::party ()
{

    std::cout<<m_name<<"::"<<"party ()  called!"<<std::endl;

}

SRef<IGuitar> VirtualGuitarist::getGuitar(IGuitar::GuitarType type)
{
    switch(type) {
    case IGuitar::GuitarType::Electric :
        return m_electricGuitar->bindTo<IGuitar>();
        break;
    case IGuitar::GuitarType::Folk :
        return m_folkGuitar;
        break;
    default:
        return m_guitar;
        break;
    }
    return m_guitar;
}

std::vector<std::string>  VirtualGuitarist::getGuitarCollection()
{
    std::vector<std::string> guitars;
    for (auto g : *m_guitars) {
        guitars.push_back(g->bindTo<IGuitar>()->getGuitarBrand());
    }
    return guitars;
}

}
