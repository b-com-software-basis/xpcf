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
 * @date 2020-09-23
 */

#ifndef ABSTRACTGENERATOR_H
#define ABSTRACTGENERATOR_H
#include <xpcf/component/ComponentBase.h>
#include <xpcf/core/Exception.h>

#include "interfaces/IRPCGenerator.h"
#include "interfaces/ITypeParser.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include "CppHelpers.h"

namespace fs = boost::filesystem;

class AbstractGenerator : public org::bcom::xpcf::ComponentBase, virtual public IRPCGenerator
{
public:
    AbstractGenerator(std::map<std::string,std::string> componentInfosMap);
    ~AbstractGenerator() override;
    void unloadComponent () override final;
    void setDestinationFolder(const std::string & folder) override;
    void setGenerateMode(const GenerateMode & mode = GenerateMode::STD_COUT) override;

    virtual std::map<MetadataType,std::string> generateImpl([[maybe_unused]] SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata) { return metadata; }
    virtual std::map<MetadataType,std::string> validateImpl([[maybe_unused]]  const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata) { return metadata; }
    virtual void initializeImpl([[maybe_unused]]  std::map<MetadataType,std::string> metadata) {}
    virtual void finalizeImpl([[maybe_unused]]  std::map<MetadataType,std::string> metadata) {}

    std::map<MetadataType,std::string> generate(SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata) final {
        metadata = generateImpl(c, metadata);
        if (m_nextGenerator) {
            return m_nextGenerator->generate(c,metadata);
        }
        return metadata;
    }

    void initialize(std::map<MetadataType,std::string> metadata) final {
        initializeImpl(metadata);
        if (m_nextGenerator) {
            m_nextGenerator->initialize(metadata);
        }
    }

    std::map<MetadataType,std::string> validate(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata) final {
        metadata = validateImpl(c, metadata);
        if (m_nextGenerator) {
            return m_nextGenerator->validate(c,metadata);
        }
        return metadata;
    }

    void finalize(std::map<MetadataType,std::string> metadata) final {
        finalizeImpl(metadata);
        if (m_nextGenerator) {
            m_nextGenerator->finalize(metadata);
        }
    }

protected:
    GenerateMode m_mode;
    fs::path m_folder;
    SRef<IRPCGenerator> m_nextGenerator;
    SRef<ITypeParser> m_astParser;
};

class GenerationException : public org::bcom::xpcf::Exception {
public:
    GenerationException(org::bcom::xpcf::XPCFErrorCode errCode = org::bcom::xpcf::XPCFErrorCode::_FAIL);
    GenerationException(const char * what, org::bcom::xpcf::XPCFErrorCode errCode = org::bcom::xpcf::XPCFErrorCode::_FAIL);
    GenerationException(const std::string & what, org::bcom::xpcf::XPCFErrorCode errCode = org::bcom::xpcf::XPCFErrorCode::_FAIL);
};

#endif // ABSTRACTGENERATOR_H
