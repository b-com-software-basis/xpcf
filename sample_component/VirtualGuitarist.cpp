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

namespace xpcf = org::bcom::xpcf;
namespace org { namespace bcom { namespace xpcf {
template<> XPCFSAMPLECOMPONENT_EXPORT_API component::VirtualGuitarist* ComponentFactory::createInstance() {
    return new component::VirtualGuitarist();
}
}}}

namespace component {


VirtualGuitarist::VirtualGuitarist ():ConfigurableBase(xpcf::toMap<VirtualGuitarist>()),m_name("VirtualGuitarist")
{
   addInterface<IMusician>(this);
   addInterface<IGuitarist>(this);
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
   std::cout<<m_name<<"::"<<"Constructor VirtualGuitarist::VirtualGuitarist () called!"<<std::endl;
}

xpcf::XPCFErrorCode VirtualGuitarist::onConfigured()
{
    std::cout<<m_name<<"::"<<"VirtualGuitarist::onConfigured() called!"<<std::endl;
    return this->xpcf::ConfigurableBase::onConfigured();
}

VirtualGuitarist::~VirtualGuitarist ()
{
    std::cout<<m_name<<"::"<<"Destructor VirtualGuitarist::VirtualGuitarist () called!"<<std::endl;
}

void VirtualGuitarist::unloadComponent ()
{
  std::cout<<m_name<<"::"<<"VirtualGuitarist::unload () called!"<<std::endl;

  delete this;
  return;
}

void VirtualGuitarist::playSolo()
{
    std::cout<<m_name<<"::"<<" VirtualGuitarist::playSolo ()  called!"<<std::endl;
}

void VirtualGuitarist::playRhythm()
{

    std::cout<<m_name<<"::"<<" VirtualGuitarist::playRhythm ()  called!"<<std::endl;

}
void VirtualGuitarist::learn ()
{

    std::cout<<m_name<<"::"<<" VirtualGuitarist::learn ()  called!"<<std::endl;

}

void VirtualGuitarist::playMusic ()
{

    std::cout<<m_name<<"::"<<" VirtualGuitarist::playMusic ()  called!"<<std::endl;

}

void VirtualGuitarist::listen ()
{

    std::cout<<m_name<<"::"<<" VirtualGuitarist::listen ()  called!"<<std::endl;

}

void VirtualGuitarist::practice ()
{

    std::cout<<m_name<<"::"<<" VirtualGuitarist::practice ()  called!"<<std::endl;

}

void VirtualGuitarist::party ()
{

    std::cout<<m_name<<"::"<<" VirtualGuitarist::party ()  called!"<<std::endl;

}
}
