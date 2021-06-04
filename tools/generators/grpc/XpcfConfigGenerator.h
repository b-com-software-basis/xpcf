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

#ifndef XPCFCONFIGGENERATOR_H
#define XPCFCONFIGGENERATOR_H
#include <xpcf/component/ComponentBase.h>

#include "AbstractGenerator.h"
#include "XmlHelpers.h"


class XpcfConfigGenerator : public AbstractGenerator
{
public:
    XpcfConfigGenerator();
    ~XpcfConfigGenerator() override;
    std::map<MetadataType,std::string> generateImpl(SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata) override;
    //stringstream to aggregate all project info, serialized to out upon destruction ? or unload ?
    //howto forward project option generation standalone/embedded ? configurable comp? through metadata ?
    void initializeImpl(std::map<MetadataType,std::string> metadata) override;
    void finalizeImpl(std::map<MetadataType,std::string> metadata) override;

private:
    std::unique_ptr<XmlFileManager> createXmlFileManager(const std::string & fileName);
    std::unique_ptr<XmlBlockManager> m_xmlClientMgr;
    std::unique_ptr<XmlBlockManager> m_xmlServerMgr;
    std::unique_ptr<XmlBlockManager> m_xmlClientPropsMgr;
    std::unique_ptr<XmlBlockManager> m_xmlServerPropsMgr;
    std::vector<std::string> m_grpcComponentBinds;
};

template <> struct org::bcom::xpcf::ComponentTraits<XpcfConfigGenerator>
{
    static constexpr const char * UUID = "{E794AA3D-5939-495B-A569-D3EBB536BD2B}";
    static constexpr const char * NAME = "XpcfConfigGenerator";
    static constexpr const char * DESCRIPTION = "XpcfConfigGenerator implements AbstractGenerator interface";
};

#endif // XPCFCONFIGGENERATOR_H
