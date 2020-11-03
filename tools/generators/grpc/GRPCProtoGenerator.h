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
 * @date 2019-11-15
 */

#ifndef GRPCPROTOGENERATOR_H
#define GRPCPROTOGENERATOR_H
#include <xpcf/component/ComponentBase.h>

#include "AbstractGenerator.h"


// "simple backend" : no optimization copied datas to protobuf types
// "optimized backend" : optimization (avoid datas copy, use of bytes type ?)
// "intrusive backend" : based on an agnostic xpcf base datastructure provided by business datas : on the go serialized buffer
class GRPCProtoGenerator : public AbstractGenerator
{
public:
    GRPCProtoGenerator();
    ~GRPCProtoGenerator() override;
    std::map<MetadataType,std::string> generateImpl(ClassDescriptor & c, std::map<MetadataType,std::string> metadata) override;
    void finalizeImpl(std::map<MetadataType,std::string> metadata) override;

    static const std::pair<std::string,bool> & tryConvertType(enum cpp_builtin_type type);

private:
    static const std::string & tryTranscribeName(ParameterDescriptor & desc);
    void prepareMessages(const ClassDescriptor &c);
    void generateMessages(MethodDescriptor & m, std::ostream& out);
    void generateService(const ClassDescriptor & c, std::ostream& out);
    std::string m_serviceName;
    std::string m_grpcServiceFilePath;
    std::vector<fs::path> m_protoFilesPath;
    std::map<std::string,fs::path> m_protoNameFilesPathMap;
};




template <> struct org::bcom::xpcf::ComponentTraits<GRPCProtoGenerator>
{
    static constexpr const char * UUID = "{8ba42339-fb6b-4ef0-aa10-e4a4aa2ebcf8}";
    static constexpr const char * NAME = "GRPCProtoGenerator";
    static constexpr const char * DESCRIPTION = "GRPCProtoGenerator implements IRPCGenerator interface";
};

#endif // GRPCPROTOGENERATOR_H
