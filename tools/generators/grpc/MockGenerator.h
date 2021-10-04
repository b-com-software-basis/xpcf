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

#ifndef MockGenerator_H
#define MockGenerator_H
#include <xpcf/component/ComponentBase.h>

#include "AbstractGenerator.h"



class MockGenerator : public AbstractGenerator
{
public:
    MockGenerator();
    ~MockGenerator() override;
    std::map<MetadataType,std::string> generateImpl(SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata) override;
    void finalizeImpl(std::map<MetadataType,std::string> metadata) override;

protected:
    void generateHeader(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata, std::ostream& out);
    void generateBody(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata, std::ostream& out);

private:
    void processHeaderMethods(const SRef<ClassDescriptor> c, CppBlockManager & blockMgr);
    void processBodyMethods(const SRef<ClassDescriptor> c, CppBlockManager & blockMgr);
    void bindInput(const ParameterDescriptor & p, CppBlockManager & blockMgr);
    void bindOutput(const ParameterDescriptor & p, CppBlockManager & blockMgr);
    std::string m_nameSpace;
    std::string m_className;
    std::string m_headerFileName;
    std::string m_cppFileName;
    std::map<std::string,std::string> m_serviceUuidMap;
    //TODO : replace m_proxyMembersVariables member with method argument forwarding, shouldn't be a member !
    std::map<std::string,std::pair<std::string,bool>> m_proxyMembersVariables;//works only without simple naming conflict, should be nspaced !
};

template <> struct org::bcom::xpcf::ComponentTraits<MockGenerator>
{
    static constexpr const char * UUID = "{79CA746B-A32F-490A-A988-DDC4FDD9A5B2}";
    static constexpr const char * NAME = "MockGenerator";
    static constexpr const char * DESCRIPTION = "MockGenerator implements AbstractGenerator interface";
};

#endif // MockGenerator_H
