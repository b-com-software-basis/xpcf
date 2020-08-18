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
#include "Ibanez7ElectricGuitar.h"
#include <iostream>
#include "xpcfSampleComponentAPI.h"

namespace xpcf = org::bcom::xpcf;

template<> component::Ibanez7ElectricGuitar* xpcf::ComponentFactory::createInstance<component::Ibanez7ElectricGuitar>();

namespace component {


Ibanez7ElectricGuitar::Ibanez7ElectricGuitar ():Guitar(xpcf::toMap<Ibanez7ElectricGuitar>())
{
    declareInterface<IElectricGuitar>(this);
    std::cout<<"Constructor Ibanez7ElectricGuitar::Ibanez7ElectricGuitar () called!"<<std::endl;
    m_brandName = "Ibanez";
    m_nbStrings = 7;
    m_type = IGuitar::GuitarType::Electric;
}

Ibanez7ElectricGuitar::~Ibanez7ElectricGuitar ()
{
    std::cout<<"Destructor Ibanez7ElectricGuitar::Ibanez7ElectricGuitar () called!"<<std::endl;
}
}
