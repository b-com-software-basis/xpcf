/**
 * @copyright Copyright (c) 2020 B-com http://www.b-com.com/
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
 * @date 2020-09-25
 */

#ifndef REMOTESERVICEGENERATOR_H
#define REMOTESERVICEGENERATOR_H
#include <xpcf/component/ComponentBase.h>

#include "AbstractGenerator.h"



class RemoteServiceGenerator : public AbstractGenerator
{
public:
    RemoteServiceGenerator();
    ~RemoteServiceGenerator() override;
    std::map<MetadataType,std::string> generate(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata) override;
    void setDestinationFolder(const std::string & folder) override;
    void setGenerateMode(const GenerateMode & mode = GenerateMode::STD_COUT) override;

private:
    SRef<IRPCGenerator> m_grpcGenerator;
    SRef<IRPCGenerator> m_proxyGenerator;
    SRef<IRPCGenerator> m_serverGenerator;
    SRef<IRPCGenerator> m_projectGenerator;
};




template <> struct org::bcom::xpcf::ComponentTraits<RemoteServiceGenerator>
{
    static constexpr const char * UUID = "{0b9f045b-28aa-45c2-b353-e22917208059}";
    static constexpr const char * NAME = "RemoteServiceGenerator";
    static constexpr const char * DESCRIPTION = "RemoteServiceGenerator implements AbstractGenerator interface";
};

#endif // REMOTESERVICEGENERATOR_H
