#include "GRPCProtoGenerator.h"
#include "RemakenPathHelper.h"
#include <boost/process.hpp>
#include <boost/predef.h>
#include <boost/predef/os.h>

namespace bp = boost::process;

namespace xpcf = org::bcom::xpcf;

template<> GRPCProtoGenerator * xpcf::ComponentFactory::createInstance<GRPCProtoGenerator>();
// service generation: generates grpcIPointCloudManagerService.grpc.pb.[h/cc]
//protoc grpcIPointCloudManagerService.proto --grpc_out=. --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin
//message generation: generates grpcIPointCloudManagerService.pb.[h/cc]
// protoc grpcIPointCloudManagerService.proto --cpp_out=./
static const std::map<cpp_builtin_type,std::pair<std::string,bool>> builtinType2protobufTypeMap =
{
{ cpp_builtin_type::cpp_void,{"Empty",false}},

{ cpp_builtin_type::cpp_bool,{"bool",false}},

{ cpp_builtin_type::cpp_uchar,{"uint32",true}},
{ cpp_builtin_type::cpp_ushort,{"uint32",true}},
{ cpp_builtin_type::cpp_uint,{"uint32",false}},
{ cpp_builtin_type::cpp_ulong,{"uint64",false}},
{ cpp_builtin_type::cpp_ulonglong,{"uint64",false}},
{ cpp_builtin_type::cpp_uint128,{"bytes",false}},

{ cpp_builtin_type::cpp_schar,{"sint32",true}},
{ cpp_builtin_type::cpp_short,{"sint32",true}},
{ cpp_builtin_type::cpp_int,{"sint32",false}},
{ cpp_builtin_type::cpp_long,{"sint64",false}},
{ cpp_builtin_type::cpp_longlong,{"sint64",false}},
{ cpp_builtin_type::cpp_int128,{"bytes",false}},

{ cpp_builtin_type::cpp_float,{"float",false}},
{ cpp_builtin_type::cpp_double,{"double",false}},
{ cpp_builtin_type::cpp_longdouble,{"bytes",false}},
{ cpp_builtin_type::cpp_float128,{"bytes",false}},

{ cpp_builtin_type::cpp_char,{"sint32",true}},
{ cpp_builtin_type::cpp_wchar,{"sint32",true}},
{ cpp_builtin_type::cpp_char16,{"sint32",true}},
{ cpp_builtin_type::cpp_char32,{"sint32",true}},
{ cpp_builtin_type::cpp_int8_t,{"sint32",true}},
{ cpp_builtin_type::cpp_int16_t,{"sint32",false}},
{ cpp_builtin_type::cpp_int32_t,{"sint32",true}},
{ cpp_builtin_type::cpp_int64_t,{"sint64",false}},

{ cpp_builtin_type::cpp_uint8_t,{"uint32",true}},
{ cpp_builtin_type::cpp_uint16_t,{"uint32",true}},
{ cpp_builtin_type::cpp_uint32_t,{"uint32",false}},
{ cpp_builtin_type::cpp_uint64_t,{"uint64",false}},

{ cpp_builtin_type::cpp_int_least8_t,{"sint32",true}},
{ cpp_builtin_type::cpp_int_least16_t,{"sint32",true}},
{ cpp_builtin_type::cpp_int_least32_t,{"sint32",false}},
{ cpp_builtin_type::cpp_int_least64_t,{"sint64",false}},

{ cpp_builtin_type::cpp_uint_least8_t,{"uint32",true}},
{ cpp_builtin_type::cpp_uint_least16_t,{"uint32",true}},
{ cpp_builtin_type::cpp_uint_least32_t,{"uint32",false}},
{ cpp_builtin_type::cpp_uint_least64_t,{"uint64",false}},

{ cpp_builtin_type::cpp_int_fast8_t,{"sint32",true}},
{ cpp_builtin_type::cpp_int_fast16_t,{"sint32",true}},
{ cpp_builtin_type::cpp_int_fast32_t,{"sint32",false}},
{ cpp_builtin_type::cpp_int_fast64_t,{"sint64",false}},

{ cpp_builtin_type::cpp_uint_fast8_t,{"uint32",true}},
{ cpp_builtin_type::cpp_uint_fast16_t,{"uint32",true}},
{ cpp_builtin_type::cpp_uint_fast32_t,{"uint32",false}},
{ cpp_builtin_type::cpp_uint_fast64_t,{"uint64",false}},
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
    {"grpcServerCompressionFormat","grpcServerCompressionFormatParam"},
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

        if (methodDesc->m_requestName != "::google::protobuf::Empty") {
            if (methodDesc->m_requestName.empty()) {
                methodDesc->m_requestName = methodDesc->m_rpcName + "Request";
            }
        }
        else {
            methodDesc->m_requestName = "google.protobuf.Empty";
        }
        if (methodDesc->hasOutputs()) {
            if (methodDesc->m_responseName.empty()) {
                methodDesc->m_responseName = methodDesc->m_rpcName + "Response";
            }
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
        if (methodDesc->m_requestName != "google.protobuf.Empty") {
            methodDesc->m_requestName = "::" + c->getMetadata().at(ClassDescriptor::MetadataType::REMOTINGNSPACE) + "::" + methodDesc->m_requestName;
        }
        else {
            methodDesc->m_requestName = "::google::protobuf::Empty";
        }
        if (methodDesc->m_responseName != "google.protobuf.Empty") {
            methodDesc->m_responseName = "::" + c->getMetadata().at(ClassDescriptor::MetadataType::REMOTINGNSPACE) + "::" + methodDesc->m_responseName;
        }
        else {
            methodDesc->m_responseName = "::google::protobuf::Empty";
        }
    }
    out<<"}"<<std::endl<<std::endl;


}

const std::pair<std::string,bool> & GRPCProtoGenerator::tryConvertType(cpp_builtin_type type)
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
    if (p.kind() == type_kind::std_string_t) {
        return "string";
    }
    if (p.kind() == type_kind::c_string_t) {
        return "string";
    }

    std::string typeName;
    if (p.kind() == type_kind::enum_t) {
        typeName = "sint32";
        p.enableStaticCast(proto2cppTypeMap.at(typeName));
        return typeName;
    }

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
    return typeName;
}

void GRPCProtoGenerator::generateMessages(SRef<MethodDescriptor> m, std::ostream& out)
{
    if (m->m_requestName != "google.protobuf.Empty") {
        out<<"message "<<m->m_requestName<<std::endl;
        out<<"{"<<std::endl;
        std::size_t fieldIndex = 1;
        std::string typeName = "int64";
        if (m->m_responseName != "google.protobuf.Empty") {
            out << "int32 grpcServerCompressionFormat  = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
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
    (*c)[ClassDescriptor::MetadataType::GRPCSERVICENAME] = m_serviceName;
    (*c)[ClassDescriptor::MetadataType::GRPCPROTOFILENAME] = m_grpcServiceFilePath;
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
            c->setRemotingNamespace("grpc" + c->getName());
            grpcServiceFile << "package "<< c->getMetadata().at(ClassDescriptor::MetadataType::REMOTINGNSPACE) <<";\n\n";
            for (auto & methodDesc : c->methods()) {
                generateMessages(methodDesc, grpcServiceFile);
            }
            generateService(c, grpcServiceFile);
            grpcServiceFile.close();
            m_protoFilesPath.push_back(grpcServiceFilePath);
            m_protoNameFilesPathMap[ (*c)[ClassDescriptor::MetadataType::GRPCSERVICENAME] ] = grpcServiceFilePath;
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
    fs::path remakenGrpcRoot = RemakenPathHelper::computeRemakenRootPackageDir();
    std::vector<fs::path> envPath = boost::this_process::path();
    remakenGrpcRoot /= "grpc/1.37.1";
    fs::path grpcBin = remakenGrpcRoot;
    grpcBin /= "/bin/x86_64/shared/release/";
    fs::path grpcLibs = remakenGrpcRoot;
    grpcLibs /= "/lib/x86_64/shared/release/";
    std::string SharedLibraryPathEnvName(RemakenPathHelper::sharedLibraryPathEnvName());

    auto env = boost::this_process::environment();
    bp::environment runEnv = env;
    fs::detail::utf8_codecvt_facet utf8;
    std::cout << "Searching protobuf compiler (protoc) from path: " << std::endl;
    std::cout << "===> "<< grpcBin.generic_string(utf8) << std::endl;

    bool isRemakenGrpc = true;
    if (!fs::exists(grpcBin) || !fs::exists(grpcLibs)) {
        isRemakenGrpc = false;
     // else search in brew paths on unixes, what about win ?
#ifdef BOOST_OS_MACOS_AVAILABLE
    grpcBin = "/usr/local/bin/";
    grpcLibs = "/usr/local/lib";
#endif
#ifdef BOOST_OS_LINUX_AVAILABLE
    grpcBin = "/home/linuxbrew/.linuxbrew/bin/";
    grpcLibs = "/home/linuxbrew/.linuxbrew/lib";
#endif
#ifdef BOOST_OS_WIN_AVAILABLE
    std::cerr<<"Error grpc protoc compiler not found : check your grpc installation !"<<std::endl;
    return;
#endif
    }
    envPath.push_back(grpcBin);
    runEnv[SharedLibraryPathEnvName] += grpcLibs.generic_string(utf8);
    fs::path toolPath = bp::search_path("protoc", envPath);
    fs::path pluginPath = bp::search_path("grpc_cpp_plugin", envPath);
    if (toolPath.empty()) {
        std::cerr<<"Error grpc protoc compiler not found : check your grpc installation !"<<std::endl;
        return;
    }
    if (pluginPath.empty()) {
        std::cerr<<"Error grpc cpp plugin not found : check your grpc installation !"<<std::endl;
        return;
    }
    std::cout << "Using protobuf compiler (protoc) from path: " << std::endl;
    std::cout << "===> "<< grpcBin.generic_string(utf8) << std::endl;

    int result = -1;
    for (auto [name, protoFile] : m_protoNameFilesPathMap) {
        std::cout << "generating grpc service and messages for " << name << " from .proto "<< protoFile.generic_string(utf8) << std::endl;
        std::string protoPath = "--proto_path=";
        protoPath += m_folder.generic_string(utf8).c_str();
        std::string destProto = "--cpp_out=";
        destProto += m_folder.generic_string(utf8).c_str();
        if (isRemakenGrpc) {
            std::string googleProtoPath = "--proto_path=";
            fs::path protobufPath = remakenGrpcRoot;
            protobufPath /= "interfaces";
            googleProtoPath += protobufPath.generic_string(utf8).c_str();
            std::cout<<toolPath.generic_string(utf8).c_str()<<" "<<protoPath<<" "<<googleProtoPath<<" "<<protoFile<<" "<<destProto<<std::endl;
            result = bp::system(toolPath, protoPath.c_str(), googleProtoPath.c_str(), protoFile.generic_string(utf8).c_str(), destProto.c_str(), runEnv);

            std::string destGrpc = "--grpc_out=";
            destGrpc += m_folder.generic_string(utf8).c_str();
            std::string plugin = "--plugin=protoc-gen-grpc=";
            plugin += pluginPath.generic_string(utf8).c_str();

            result = bp::system(toolPath, protoPath.c_str(), googleProtoPath.c_str(), protoFile.generic_string(utf8).c_str(), destGrpc.c_str(), plugin.c_str(), runEnv);
        }
        else {
            std::cout<<toolPath.generic_string(utf8).c_str()<<" "<<protoPath<<" "<<protoFile<<" "<<destProto<<std::endl;
            result = bp::system(toolPath, protoPath.c_str(), protoFile.generic_string(utf8).c_str(), destProto.c_str(), runEnv);

            std::string destGrpc = "--grpc_out=";
            destGrpc += m_folder.generic_string(utf8).c_str();
            std::string plugin = "--plugin=protoc-gen-grpc=";
            plugin += pluginPath.generic_string(utf8).c_str();

            result = bp::system(toolPath, protoPath.c_str(), protoFile.generic_string(utf8).c_str(), destGrpc.c_str(), plugin.c_str(), runEnv);
        }
    }
    m_protoFilesPath.clear();
}
