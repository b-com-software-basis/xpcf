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
 * @date 2017-03-07
 */

#include <stdio.h>
#include "HumanMusician.h"
#include <iostream>
#include "xpcfSampleComponentAPI.h"

namespace xpcf = org::bcom::xpcf;

template<> component::HumanMusician* xpcf::ComponentFactory::createInstance<component::HumanMusician>();

namespace component {
HumanMusician::HumanMusician ():xpcf::ComponentBase(xpcf::toMap<HumanMusician>()),m_name("HumanMusician")
{
   declareInterface<IHuman>(this);
   declareInterface<IMusician>(this);
   std::cout<<m_name<<"::"<<"Constructor HumanMusician::HumanMusician () called!"<<std::endl;
}

HumanMusician::~HumanMusician ()
{
    std::cout<<m_name<<"::"<<"Destructor HumanMusician::HumanMusician () called!"<<std::endl;
}

void HumanMusician::unloadComponent ()
{
  std::cout<<m_name<<"::"<<"HumanMusician::unload () called!"<<std::endl;

  delete this;
  return;
}

void HumanMusician::play ()
{
    //m_blurFactor*m_blurScale[1];
    std::cout<<m_name<<"::"<<" HumanMusician::play ()  called!"<<std::endl;

}

void HumanMusician::eat ()
{

    std::cout<<m_name<<"::"<<" HumanMusician::eat ()  called!"<<std::endl;

}

void HumanMusician::sleep ()
{

    std::cout<<m_name<<"::"<<" HumanMusician::sleep ()  called!"<<std::endl;


}

void HumanMusician::kiss ()
{


    std::cout<<m_name<<"::"<<" HumanMusician::kiss ()  called!"<<std::endl;


}

void HumanMusician::work ()
{

    std::cout<<m_name<<"::"<<" HumanMusician::work ()  called!"<<std::endl;

}

void HumanMusician::learn ()
{

    std::cout<<m_name<<"::"<<" HumanMusician::learn ()  called!"<<std::endl;

}

void HumanMusician::playMusic ()
{

    std::cout<<m_name<<"::"<<" HumanMusician::playMusic ()  called!"<<std::endl;

}

void HumanMusician::listen ()
{

    std::cout<<m_name<<"::"<<" HumanMusician::listen ()  called!"<<std::endl;

}

void HumanMusician::practice ()
{

    std::cout<<m_name<<"::"<<" HumanMusician::practice ()  called!"<<std::endl;

}

void HumanMusician::party ()
{

    std::cout<<m_name<<"::"<<" HumanMusician::party ()  called!"<<std::endl;

}
}
