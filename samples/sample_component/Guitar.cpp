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
#include "Guitar.h"
#include <iostream>
#include "xpcfSampleComponentAPI.h"

namespace xpcf = org::bcom::xpcf;

namespace component {

Guitar::Guitar (std::map<std::string,std::string> componentInfosMap):ComponentBase(componentInfosMap)
{
   declareInterface<IGuitar>(this);
   declareInjectable<ITuner>(m_tuner);
   std::cout<<"Constructor Guitar::Guitar () called!"<<std::endl;
}

Guitar::~Guitar ()
{
    std::cout<<"Destructor Guitar::Guitar () called!"<<std::endl;
}

void Guitar::unloadComponent ()
{
  std::cout<<"Guitar::unload () called!"<<std::endl;

  delete this;
  return;
}

}
