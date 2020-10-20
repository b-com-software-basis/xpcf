#include "GRPCProtoGenerator.h"

#include <boost/process.hpp>
#include <boost/predef.h>

namespace bp = boost::process;

namespace xpcf = org::bcom::xpcf;

template<> GRPCProtoGenerator * xpcf::ComponentFactory::createInstance<GRPCProtoGenerator>();
// service generation: generates grpcIPointCloudManagerService.grpc.pb.[h/cc]
//protoc grpcIPointCloudManagerService.proto --grpc_out=. --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin
//message generation: generates grpcIPointCloudManagerService.pb.[h/cc]
// protoc grpcIPointCloudManagerService.proto --cpp_out=./
const std::map<enum cpp_builtin_type,std::string> builtinType2protobufTypeMap =
{
{ cpp_void,"Empty"},

{ cpp_bool,"bool"},

{ cpp_uchar,"bytes"},
{ cpp_ushort,"bytes"},
{ cpp_uint,"uint32"},
{ cpp_ulong,"uint64"},
{ cpp_ulonglong,"uint64"},
{ cpp_uint128,"bytes"},

{ cpp_schar,"bytes"},
{ cpp_short,"bytes"},
{ cpp_int,"sint32"},
{ cpp_long,"sint64"},
{ cpp_longlong,"sint64"},
{ cpp_int128,"bytes"},

{ cpp_float,"float"},
{ cpp_double,"double"},
{ cpp_longdouble,"bytes"},
{ cpp_float128,"bytes"},

{ cpp_char,"bytes"},
{ cpp_wchar,"bytes"},
{ cpp_char16,"bytes"},
{ cpp_char32,"bytes"},
{ cpp_int8_t,"bytes"},
{ cpp_int16_t,"bytes"},
{ cpp_int32_t,"sint32"},
{ cpp_int64_t,"sint64"},

{ cpp_uint8_t,"bytes"},
{ cpp_uint16_t,"bytes"},
{ cpp_uint32_t,"uint32"},
{ cpp_uint64_t,"uint64"},

{ cpp_int_least8_t,"bytes"},
{ cpp_int_least16_t,"bytes"},
{ cpp_int_least32_t,"sint32"},
{ cpp_int_least64_t,"sint64"},

{ cpp_uint_least8_t,"bytes"},
{ cpp_uint_least16_t,"bytes"},
{ cpp_uint_least32_t,"uint32"},
{ cpp_uint_least64_t,"uint64"},

{ cpp_int_fast8_t,""},
{ cpp_int_fast16_t,""},
{ cpp_int_fast32_t,"sint32"},
{ cpp_int_fast64_t,"sint64"},

{ cpp_uint_fast8_t,""},
{ cpp_uint_fast16_t,""},
{ cpp_uint_fast32_t,"uint32"},
{ cpp_uint_fast64_t,"uint64"},
};


GRPCProtoGenerator::GRPCProtoGenerator():AbstractGenerator(xpcf::toMap<GRPCProtoGenerator>())
{
}


GRPCProtoGenerator::~GRPCProtoGenerator()
{

}

void GRPCProtoGenerator::prepareMessages(const ClassDescriptor &c)
{
    for (auto & methodDesc : c.methods()) {
        std::string streamingClient, streamingServer;

        if (methodDesc.hasInputs()) {
            methodDesc.m_requestName = methodDesc.m_rpcName + "Request";
        }
        else {
            methodDesc.m_requestName = "google.protobuf.Empty";
        }
        if (methodDesc.hasOutputs()) {
            methodDesc.m_responseName = methodDesc.m_rpcName + "Response";
        }
        else {
            methodDesc.m_responseName = "google.protobuf.Empty";
        }
    }
}

void GRPCProtoGenerator::generateService(const ClassDescriptor &c, std::ostream& out)
{
    out<<"service "<<m_serviceName<<" {"<<std::endl;
    for (auto & methodDesc : c.methods()) {
        std::string streamingClient, streamingServer;

        if ((methodDesc.streamingType() & MethodDescriptor::streaming_type::client) && methodDesc.m_requestName != "google.protobuf.Empty") {
            streamingClient = "stream ";
        }
        if ((methodDesc.streamingType() & MethodDescriptor::streaming_type::server)  && methodDesc.m_responseName != "google.protobuf.Empty"){
            streamingServer = "stream ";
        }
        out<<"rpc "<<methodDesc.m_rpcName<<"("<<streamingClient<<methodDesc.m_requestName<<") returns("<<streamingServer<<methodDesc.m_responseName<<") {}"<<std::endl;
    }
    out<<"}"<<std::endl<<std::endl;
}

const std::string & GRPCProtoGenerator::tryConvertType(enum cpp_builtin_type type)
{
    static const std::string typeStr = "";
    if (builtinType2protobufTypeMap.find(type) != builtinType2protobufTypeMap.end()) {
        return builtinType2protobufTypeMap.at(type);
    }
    return typeStr;
}

inline std::string getTypeName(const TypeDescriptor & p) {
    std::string typeName;
    if (p.kind() == type_kind::std_string_t) {
        typeName = "string";
    }
    else {
        if (p.kind() != type_kind::enum_t) {
            if (p.isVoid() && p.isReference()) {
                throw GenerationException("GRPCProtoGenerator: void * serialization is not defined !");
            }
            typeName = GRPCProtoGenerator::tryConvertType(p.getBuiltinType());
            if (typeName.size() == 0 && p.kind() != type_kind::builtin_t) {
                typeName = "bytes";
            }
        }
        else {
            typeName = "sint32";
        }
    }
    return typeName;
}

void GRPCProtoGenerator::generateMessages(const MethodDescriptor & m, std::ostream& out)
{
    if (m.m_requestName != "google.protobuf.Empty") {
        out<<"message "<<m.m_requestName<<std::endl;
        out<<"{"<<std::endl;
        std::size_t fieldIndex = 1;
        std::string typeName = "int64";
        for (auto & p : m.m_inParams) {
            out<<getTypeName(p.type())<<" "<<p.getName()<<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }

        for (auto & p : m.m_inoutParams) {
            out<<getTypeName(p.type())<<" "<<p.getName()<<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
        out<<"}"<<std::endl<<std::endl;
    }
    if (m.m_responseName != "google.protobuf.Empty") {
        out<<"message "<<m.m_responseName<<std::endl;
        out<<"{"<<std::endl;
        std::size_t fieldIndex = 1;
        std::string typeName = "int64";
        for (auto & p : m.m_inoutParams) {
            out<<getTypeName(p.type())<<" "<<p.getName()<<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
        for (auto & p : m.m_outParams) {
            out<<getTypeName(p.type())<<" "<<p.getName()<<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
        if (!m.returnType().isVoid()) {
            out<<getTypeName(m.returnType())<<" xpcfGrpcReturnValue = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
        out<<"}"<<std::endl<<std::endl;
    }
}

std::map<IRPCGenerator::MetadataType,std::string> GRPCProtoGenerator::generate(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata)
{
    m_serviceName = "grpc" + c.getName() + "Service";
    m_grpcServiceFilePath = m_serviceName + ".proto";
    metadata[MetadataType::GRPCSERVICENAME] = m_serviceName;
    metadata[MetadataType::GRPCPROTOFILENAME] = m_grpcServiceFilePath;
    prepareMessages(c);

    try {
        if (m_mode == GenerateMode::STD_COUT) {
            for (auto & methodDesc : c.methods()) {
                generateMessages(methodDesc, std::cout);
            }
            generateService(c, std::cout);
        }
        else {
            fs::detail::utf8_codecvt_facet utf8;
            fs::path grpcServiceFilePath(m_grpcServiceFilePath,utf8);
            grpcServiceFilePath = m_folder/grpcServiceFilePath;
            std::ofstream grpcServiceFile(grpcServiceFilePath.generic_string(utf8).c_str(), std::ios::out);
            grpcServiceFile << "syntax = \"proto3\";\n\n";
            grpcServiceFile << "import \"google/protobuf/empty.proto\";\n\n";
            for (auto & methodDesc : c.methods()) {
                generateMessages(methodDesc, grpcServiceFile);
            }
            generateService(c, grpcServiceFile);
            grpcServiceFile.close();
            m_protoFilesPath.push_back(grpcServiceFilePath);
            m_protoNameFilesPathMap[ metadata[MetadataType::GRPCSERVICENAME] ] = grpcServiceFilePath;
        }
    }
    catch (const GenerationException & e) {
        fs::detail::utf8_codecvt_facet utf8;
        fs::path grpcServiceFilePath(m_grpcServiceFilePath,utf8);
        fs::remove(grpcServiceFilePath);
        throw e;
    }
    return metadata;
}

void GRPCProtoGenerator::finalize(std::map<MetadataType,std::string> metadata)
{
    fs::path toolPath = bp::search_path("protoc");
    fs::path pluginPath = bp::search_path("grpc_cpp_plugin");
    if (toolPath.empty()) {
        std::cerr<<"Error grpc protoc compiler not found : check your grpc installation !"<<std::endl;
        return;
    }
    if (pluginPath.empty()) {
        std::cerr<<"Error grpc cpp plugin not found : check your grpc installation !"<<std::endl;
        return;
    }
    int result = -1;
    fs::detail::utf8_codecvt_facet utf8;
    for (auto [name, protoFile] : m_protoNameFilesPathMap) {
        std::cout << "generating grpc service and messages for " << name << " from .proto "<< protoFile.generic_string(utf8) << std::endl;
        std::string protoPath = "--proto_path=";
        protoPath += m_folder.generic_string(utf8).c_str();
        std::string destProto = "--cpp_out=";
        destProto += m_folder.generic_string(utf8).c_str();
        result = bp::system(toolPath, protoPath.c_str(), protoFile.generic_string(utf8).c_str(), destProto.c_str());

        std::string destGrpc = "--grpc_out=";
        destGrpc += m_folder.generic_string(utf8).c_str();
        std::string plugin = "--plugin=protoc-gen-grpc=";
        plugin += pluginPath.generic_string(utf8).c_str();

        result = bp::system(toolPath, protoPath.c_str(), protoFile.generic_string(utf8).c_str(), destGrpc.c_str(), plugin.c_str());
    }
    m_protoFilesPath.clear();
}
