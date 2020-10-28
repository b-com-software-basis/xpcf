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

#ifndef PROJECTGENERATOR_H
#define PROJECTGENERATOR_H
#include <xpcf/component/ComponentBase.h>

#include "AbstractGenerator.h"



class ProjectGenerator : public AbstractGenerator
{
public:
    ProjectGenerator();
    ~ProjectGenerator() override;
    std::map<MetadataType,std::string> generate(ClassDescriptor & c, std::map<MetadataType,std::string> metadata) override;
    //stringstream to aggregate all project info, serialized to out upon destruction ? or unload ?
    //howto forward project option generation standalone/embedded ? configurable comp? through metadata ?
    void finalize(std::map<MetadataType,std::string> metadata) override;

private:
    void generateModuleMain(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata);
    void generateProjectFile(std::map<MetadataType,std::string> metadata, std::ostream& out);
    std::stringstream m_srcProjectInfos;
    std::stringstream m_headerProjectInfos;
    std::stringstream m_protoProjectInfos;
    std::stringstream m_moduleMainInfosHeader;
    std::stringstream m_moduleMainInfosSource;
    std::stringstream m_moduleMainDeclareComponents;
    std::stringstream m_moduleMainDeclareModule;
    std::unique_ptr<CppBlockManager> m_moduleHdrMgr;
    std::unique_ptr<CppBlockManager> m_moduleSrcMgr;
    std::unique_ptr<CppBlockManager> m_moduleDeclareMgr;

};




template <> struct org::bcom::xpcf::ComponentTraits<ProjectGenerator>
{
    static constexpr const char * UUID = "{9d465f72-0935-4f64-8620-7fd349edcb9a}";
    static constexpr const char * NAME = "ProjectGenerator";
    static constexpr const char * DESCRIPTION = "ProjectGenerator implements AbstractGenerator interface";
};

#endif // PROJECTGENERATOR_H
