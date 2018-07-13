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
#include "C1.h"
#include <iostream>
#include "xpcfSampleComponentAPI.h"

namespace xpcf = org::bcom::xpcf;
namespace org { namespace bcom { namespace xpcf {
template<> XPCFSAMPLECOMPONENT_EXPORT_API component::C1* ComponentFactory::createInstance() {
    return new component::C1();
}
}}}

namespace component {


C1::C1 ():ConfigurableBase(xpcf::toMap<C1>()),m_name("C1")
{
   addInterface<I1>(this);
   addInterface<I2>(this);
   m_blurMatrixRepartition = xpcf::getPropertyMapInstance();
   m_blurScales.resize(4);
   m_matrixValues.resize(4);

   SRef<xpcf::IPropertyMap> params = getPropertyRootNode();
   params->wrapStructure("blurMatrixRepartition",m_blurMatrixRepartition);
   params->wrapDouble("blurFactor",m_blurFactor);
   params->wrapDoubleVector("blurScale",m_blurScales);
   m_blurMatrixRepartition->wrapLong("cols",m_cols);
   m_blurMatrixRepartition->wrapLong("rows",m_rows);
   m_blurMatrixRepartition->wrapDoubleVector("values",m_matrixValues);
   std::cout<<m_name<<"::"<<"Constructor C1::C1 () called!"<<std::endl;
}

C1::~C1 ()
{
    std::cout<<m_name<<"::"<<"Destructor C1::C1 () called!"<<std::endl;
}

void C1::unloadComponent ()
{
  std::cout<<m_name<<"::"<<"C1::unload () called!"<<std::endl;

  delete this;
  return;
}

void C1::I2_opname ()
{
    //m_blurFactor*m_blurScale[1];
    std::cout<<m_name<<"::"<<" C1::I2_opname ()  called!"<<std::endl;

}

void C1::I2_opname2 ()
{

    std::cout<<m_name<<"::"<<" C1::I2_opname2 ()  called!"<<std::endl;

}

void C1::I1_opname ()
{

    std::cout<<m_name<<"::"<<" C1::I1_opname ()  called!"<<std::endl;

}

void C1::I1_opname2 ()
{

    std::cout<<m_name<<"::"<<" C1::I1_opname2 ()  called!"<<std::endl;

}

void C1::I1_opname3 ()
{

    std::cout<<m_name<<"::"<<" C1::I1_opname3 ()  called!"<<std::endl;

}

void C1::I1_opname4 ()
{

    std::cout<<m_name<<"::"<<" C1::I1_opname4 ()  called!"<<std::endl;

}

void C1::I1_opname5 ()
{

    std::cout<<m_name<<"::"<<" C1::I1_opname5 ()  called!"<<std::endl;

}
}
