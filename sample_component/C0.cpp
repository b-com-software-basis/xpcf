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
#include "C0.h"
#include <iostream>
#include "xpcfSampleComponentAPI.h"

namespace xpcf = org::bcom::xpcf;
namespace org { namespace bcom { namespace xpcf {
template<> XPCFSAMPLECOMPONENT_EXPORT_API component::C0* ComponentFactory::createInstance() {
    return new component::C0();
}
}}}

namespace component {


C0::C0 ():ComponentBase(xpcf::toMap<C0>()),m_name("C0")
{
   addInterface<I0>(this);
   addInterface<I1>(this);
   std::cout<<m_name<<"::"<<"Constructor C0::C0 () called!"<<std::endl;
}

C0::~C0 ()
{
    std::cout<<m_name<<"::"<<"Destructor C0::C0 () called!"<<std::endl;
}

void C0::unloadComponent ()
{
  std::cout<<m_name<<"::"<<"C0::unload () called!"<<std::endl;

  delete this;
  return;
}

void C0::I0_opname ()
{
    //m_blurFactor*m_blurScale[1];
    std::cout<<m_name<<"::"<<" C0::I0_opname ()  called!"<<std::endl;

}

void C0::I0_opname2 ()
{

    std::cout<<m_name<<"::"<<" C0::I0_opname2 ()  called!"<<std::endl;

}

void C0::I0_opname3 ()
{

    std::cout<<m_name<<"::"<<" C0::I0_opname3 ()  called!"<<std::endl;


}

void C0::I0_opname4 ()
{


    std::cout<<m_name<<"::"<<" C0::I0_opname4 ()  called!"<<std::endl;


}

void C0::I0_opname5 ()
{

    std::cout<<m_name<<"::"<<" C0::I0_opname5 ()  called!"<<std::endl;

}

void C0::I1_opname ()
{

    std::cout<<m_name<<"::"<<" C0::I1_opname ()  called!"<<std::endl;

}

void C0::I1_opname2 ()
{

    std::cout<<m_name<<"::"<<" C0::I1_opname2 ()  called!"<<std::endl;

}

void C0::I1_opname3 ()
{

    std::cout<<m_name<<"::"<<" C0::I1_opname3 ()  called!"<<std::endl;

}

void C0::I1_opname4 ()
{

    std::cout<<m_name<<"::"<<" C0::I1_opname4 ()  called!"<<std::endl;

}

void C0::I1_opname5 ()
{

    std::cout<<m_name<<"::"<<" C0::I1_opname5 ()  called!"<<std::endl;

}
}
