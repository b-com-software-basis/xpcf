/**
 * @copyright Copyright (c) 2019 B-com http://www.b-com.com/
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
 * @date 2020-02-10
 */
#ifndef IRPCGENERATOR_H
#define IRPCGENERATOR_H


#include <xpcf/api/IComponentIntrospect.h>

#include "ClassDescriptor.h"

class IRPCGenerator: public virtual org::bcom::xpcf::IComponentIntrospect
{
public:
    typedef enum {
        STD_COUT = 0,
        FILE = 1
    } GenerateMode;

    typedef enum {
        INTERFACENAMESPACE,
        GRPCSERVICENAME,
        GRPCPROTOFILENAME,
        GRPCPACKAGE,
        HEADERFILENAME,
        CPPFILENAME,
        CPPNAMESPACE
    } MetadataType;
    virtual ~IRPCGenerator() override = default;
    virtual void setDestinationFolder(const std::string & folder) = 0;
    virtual void setGenerateMode(const GenerateMode & mode = GenerateMode::STD_COUT) = 0;
    virtual std::map<MetadataType,std::string> generate(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata) = 0;
};

template <> struct org::bcom::xpcf::InterfaceTraits<IRPCGenerator>
{
    static constexpr const char * UUID = "{b4329fcb-7fcf-4801-9315-169dfd42ba41}";
    static constexpr const char * NAME = "IRPCGenerator";
    static constexpr const char * DESCRIPTION = "IRPCGenerator interface description";
};

#endif // IRPCGENERATOR_H
