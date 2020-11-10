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
static const std::map<enum cpp_builtin_type,std::pair<std::string,bool>> builtinType2protobufTypeMap =
{
{ cpp_void,{"Empty",false}},

{ cpp_bool,{"bool",false}},

{ cpp_uchar,{"uint32",true}},
{ cpp_ushort,{"uint32",true}},
{ cpp_uint,{"uint32",false}},
{ cpp_ulong,{"uint64",false}},
{ cpp_ulonglong,{"uint64",false}},
{ cpp_uint128,{"bytes",false}},

{ cpp_schar,{"sint32",true}},
{ cpp_short,{"sint32",true}},
{ cpp_int,{"sint32",false}},
{ cpp_long,{"sint64",false}},
{ cpp_longlong,{"sint64",false}},
{ cpp_int128,{"bytes",false}},

{ cpp_float,{"float",false}},
{ cpp_double,{"double",false}},
{ cpp_longdouble,{"bytes",false}},
{ cpp_float128,{"bytes",false}},

{ cpp_char,{"sint32",true}},
{ cpp_wchar,{"sint32",true}},
{ cpp_char16,{"sint32",true}},
{ cpp_char32,{"sint32",true}},
{ cpp_int8_t,{"sint32",true}},
{ cpp_int16_t,{"sint32",false}},
{ cpp_int32_t,{"sint32",true}},
{ cpp_int64_t,{"sint64",false}},

{ cpp_uint8_t,{"uint32",true}},
{ cpp_uint16_t,{"uint32",true}},
{ cpp_uint32_t,{"uint32",false}},
{ cpp_uint64_t,{"uint64",false}},

{ cpp_int_least8_t,{"sint32",true}},
{ cpp_int_least16_t,{"sint32",true}},
{ cpp_int_least32_t,{"sint32",false}},
{ cpp_int_least64_t,{"sint64",false}},

{ cpp_uint_least8_t,{"uint32",true}},
{ cpp_uint_least16_t,{"uint32",true}},
{ cpp_uint_least32_t,{"uint32",false}},
{ cpp_uint_least64_t,{"uint64",false}},

{ cpp_int_fast8_t,{"sint32",true}},
{ cpp_int_fast16_t,{"sint32",true}},
{ cpp_int_fast32_t,{"sint32",false}},
{ cpp_int_fast64_t,{"sint64",false}},

{ cpp_uint_fast8_t,{"uint32",true}},
{ cpp_uint_fast16_t,{"uint32",true}},
{ cpp_uint_fast32_t,{"uint32",false}},
{ cpp_uint_fast64_t,{"uint64",false}},
};


static const std::map<std::string,std::string> proto2cppTypeMap = {
    {"uint32","uint32_t"},
    {"sint32","int32_t"},
    {"uint64","uint64_t"},
    {"sint64","int64_t"},
    {"bytes","std::string"},
};

static const std::map<std::string,std::string> protoReservedKeywordsTranscription = {
    {"descriptor","descriptorParam"},
};

GRPCProtoGenerator::GRPCProtoGenerator():AbstractGenerator(xpcf::toMap<GRPCProtoGenerator>())
{
}


GRPCProtoGenerator::~GRPCProtoGenerator()
{

}

void GRPCProtoGenerator::prepareMessages(const SRef<ClassDescriptor> c)
{
    for (auto & methodDesc : c->methods()) {
        std::string streamingClient, streamingServer;

        if (methodDesc->hasInputs()) {
            methodDesc->m_requestName = methodDesc->m_rpcName + "Request";
        }
        else {
            methodDesc->m_requestName = "google.protobuf.Empty";
        }
        if (methodDesc->hasOutputs()) {
            methodDesc->m_responseName = methodDesc->m_rpcName + "Response";
        }
        else {
            methodDesc->m_responseName = "google.protobuf.Empty";
        }
    }
}

void GRPCProtoGenerator::generateService(const SRef<ClassDescriptor> c, std::ostream& out)
{
    out<<"service "<<m_serviceName<<" {"<<std::endl;
    for (auto & methodDesc : c->methods()) {
        std::string streamingClient, streamingServer;

        if ((methodDesc->streamingType() & MethodDescriptor::streaming_type::client) && methodDesc->m_requestName != "google.protobuf.Empty") {
            streamingClient = "stream ";
        }
        if ((methodDesc->streamingType() & MethodDescriptor::streaming_type::server)  && methodDesc->m_responseName != "google.protobuf.Empty"){
            streamingServer = "stream ";
        }
        out<<"rpc "<<methodDesc->m_rpcName<<"("<<streamingClient<<methodDesc->m_requestName<<") returns("<<streamingServer<<methodDesc->m_responseName<<") {}"<<std::endl;
        if (!methodDesc->hasInputs()) {
            methodDesc->m_requestName = "::google::protobuf::Empty";
        }
        if (!methodDesc->hasOutputs()) {
            methodDesc->m_responseName = "::google::protobuf::Empty";
        }
    }
    out<<"}"<<std::endl<<std::endl;


}

const std::pair<std::string,bool> & GRPCProtoGenerator::tryConvertType(enum cpp_builtin_type type)
{
    static const std::pair<std::string,bool> typeStr = {"",false};
    if (builtinType2protobufTypeMap.find(type) != builtinType2protobufTypeMap.end()) {
        return builtinType2protobufTypeMap.at(type);
    }
    return typeStr;
}

const std::string & GRPCProtoGenerator::tryTranscribeName(ParameterDescriptor & desc)
{
    if (protoReservedKeywordsTranscription.find(desc.getName()) != protoReservedKeywordsTranscription.end()) {
        desc.setName(protoReservedKeywordsTranscription.at(desc.getName()));
    }
    return desc.getName();
}

std::string GRPCProtoGenerator::getTypeName(const TypeDescriptor & p)
{
    std::string typeName;
    if (p.kind() == type_kind::std_string_t) {
        typeName = "string";
    }
    else {
        if (p.kind() != type_kind::enum_t) {
            if (p.isVoid() && p.isReference()) {
                throw GenerationException("GRPCProtoGenerator: void * serialization is not defined !");
            }
            //TODO : differentiate between pointers and refs !
            //pointer means array of elements : need a size to figure out howto handle the parameter !
            //reference to builtin means builtin not array : must be different while transcribing and generating stubs !
            if ( p.kind() == type_kind::builtin_t ) {
                const std::pair<std::string,bool> & typeInfo = GRPCProtoGenerator::tryConvertType(p.getBuiltinType());
                typeName = typeInfo.first;
                if (typeInfo.second) {
                    p.enableStaticCast(proto2cppTypeMap.at(typeName));
                }
            }
            if (typeName.size() == 0 && p.kind() != type_kind::builtin_t) {
                typeName = "bytes";
            }
        }
        else {
            typeName = "sint32";
            p.enableStaticCast(typeName);
        }
    }
    return typeName;
}

void GRPCProtoGenerator::generateMessages(SRef<MethodDescriptor> m, std::ostream& out)
{
    if (m->m_requestName != "google.protobuf.Empty") {
        out<<"message "<<m->m_requestName<<std::endl;
        out<<"{"<<std::endl;
        std::size_t fieldIndex = 1;
        std::string typeName = "int64";
        for (ParameterDescriptor * p : m->m_inParams) {
            auto typeName = getTypeName(p->type());
            out << typeName <<" "<< GRPCProtoGenerator::tryTranscribeName(*p) <<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }

        for (ParameterDescriptor * p : m->m_inoutParams) {
            auto typeName = getTypeName(p->type());
            out<< typeName <<" "<< GRPCProtoGenerator::tryTranscribeName(*p) <<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
        out<<"}"<<std::endl<<std::endl;
    }
    if (m->m_responseName != "google.protobuf.Empty") {
        out<<"message "<<m->m_responseName<<std::endl;
        out<<"{"<<std::endl;
        std::size_t fieldIndex = 1;
        std::string typeName = "int64";
        for (ParameterDescriptor * p : m->m_inoutParams) {
            auto typeName = getTypeName(p->type());

            out<< typeName <<" "<< GRPCProtoGenerator::tryTranscribeName(*p) <<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
        for (ParameterDescriptor * p : m->m_outParams) {
            auto typeName = getTypeName(p->type());

            out<< typeName <<" "<< GRPCProtoGenerator::tryTranscribeName(*p) <<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
        if (!m->returnType().isVoid()) {
            auto typeName = getTypeName(m->returnType());
            out<< typeName <<" xpcfGrpcReturnValue = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
        out<<"}"<<std::endl<<std::endl;
    }
}

std::map<IRPCGenerator::MetadataType,std::string> GRPCProtoGenerator::generateImpl(SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata)
{
    m_serviceName = "grpc" + c->getName() + "Service";
    m_grpcServiceFilePath = m_serviceName + ".proto";
    metadata[MetadataType::GRPCSERVICENAME] = m_serviceName;
    metadata[MetadataType::GRPCPROTOFILENAME] = m_grpcServiceFilePath;
    prepareMessages(c);

    try {
        if (m_mode == GenerateMode::STD_COUT) {
            for (auto & methodDesc : c->methods()) {
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
            for (auto & methodDesc : c->methods()) {
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

void GRPCProtoGenerator::finalizeImpl(std::map<MetadataType,std::string> metadata)
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
        std::cout<<toolPath.generic_string(utf8).c_str()<<" "<<protoPath<<" "<<protoFile<<" "<<destProto<<std::endl;
        result = bp::system(toolPath, protoPath.c_str(), protoFile.generic_string(utf8).c_str(), destProto.c_str());

        std::string destGrpc = "--grpc_out=";
        destGrpc += m_folder.generic_string(utf8).c_str();
        std::string plugin = "--plugin=protoc-gen-grpc=";
        plugin += pluginPath.generic_string(utf8).c_str();

        result = bp::system(toolPath, protoPath.c_str(), protoFile.generic_string(utf8).c_str(), destGrpc.c_str(), plugin.c_str());
    }
    m_protoFilesPath.clear();
}
