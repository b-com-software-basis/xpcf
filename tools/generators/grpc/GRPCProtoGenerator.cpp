#include "GRPCProtoGenerator.h"

namespace xpcf = org::bcom::xpcf;

template<> GRPCProtoGenerator * xpcf::ComponentFactory::createInstance<GRPCProtoGenerator>();

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


GRPCProtoGenerator::GRPCProtoGenerator():xpcf::ComponentBase(xpcf::toMap<GRPCProtoGenerator>())
{
    declareInterface<IRPCGenerator>(this);
    //  Inject declarations come here : declare any component that must be injected to your component through its interface /////////////////////////..///// declareInjectable<IFilter>(m_filter);
    //
    // Inject declaration can have a name :
    // declareInjectable<IFilter>(m_blurFilter, "blurFilter");
    //
    // Inject declaration can be optional i.e. not finding a binding component for the interface is not an error :
    // declareInjectable<IImageFilter>(m_imageFilter, false);

}


GRPCProtoGenerator::~GRPCProtoGenerator()
{

}

void GRPCProtoGenerator::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    // delete this;
    return;
}

void GRPCProtoGenerator::generateService(const ClassDescriptor &c)
{
    std::cout<<"service grpc"<<c.getName()<<"Service {"<<std::endl;
    for (auto & methodDesc : c.methods()) {
        std::string streamingClient, streamingServer;

        if (methodDesc.m_inParams.size() != 0 || methodDesc.m_inoutParams.size() != 0) {
            methodDesc.m_requestName = methodDesc.m_rpcName + "Request";
        }
        if (methodDesc.m_outParams.size() != 0 || methodDesc.m_inoutParams.size() != 0 || !methodDesc.returnType().isVoid()) {
            methodDesc.m_responseName = methodDesc.m_rpcName + "Response";
        }
        if ((methodDesc.streamingType() & MethodDescriptor::streaming_type::client) && methodDesc.m_requestName != "Empty") {
            streamingClient = "stream ";
        }
        if ((methodDesc.streamingType() & MethodDescriptor::streaming_type::server)  && methodDesc.m_responseName != "Empty"){
            streamingServer = "stream ";
        }
        std::cout<<"rpc "<<methodDesc.m_rpcName<<"("<<streamingClient<<methodDesc.m_requestName<<") returns("<<streamingServer<<methodDesc.m_responseName<<") {}"<<std::endl;
    }
    std::cout<<"}"<<std::endl<<std::endl;
}

const std::string & tryConvertType(enum cpp_builtin_type type)
{
    static const std::string typeStr = "";
    if (builtinType2protobufTypeMap.find(type) != builtinType2protobufTypeMap.end()) {
        return builtinType2protobufTypeMap.at(type);
    }
    return typeStr;
}

void GRPCProtoGenerator::generateMessages(const MethodDescriptor & m)
{
    if (m.m_requestName != "Empty") {
        std::cout<<"Message "<<m.m_requestName<<std::endl;
        std::cout<<"{"<<std::endl;
        std::size_t fieldIndex = 0;
        std::string typeName = "int64";
        for (auto & p : m.m_inParams) {
            if (p.type().kind() != type_kind::enum_t) {
                typeName = tryConvertType(p.type().getBuiltinType());
                if (p.type().kind() != type_kind::builtin_t) {
                    typeName = p.type().getTypename();
                }
            }
            std::cout<<typeName<<" "<<p.getName()<<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }

        for (auto & p : m.m_inoutParams) {
            if (p.type().kind() != type_kind::enum_t) {
                typeName = tryConvertType(p.type().getBuiltinType());
                if (p.type().kind() != type_kind::builtin_t) {
                    typeName = p.type().getTypename();
                }
            }
            std::cout<<typeName<<" "<<p.getName()<<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
        std::cout<<"}"<<std::endl<<std::endl;
    }
    if (m.m_responseName != "Empty") {
        std::cout<<"Message "<<m.m_responseName<<std::endl;
        std::cout<<"{"<<std::endl;
        std::size_t fieldIndex = 0;
        std::string typeName = "int64";
        for (auto & p : m.m_inoutParams) {
            if (p.type().kind() != type_kind::enum_t) {
                typeName = tryConvertType(p.type().getBuiltinType());
                if (p.type().kind() != type_kind::builtin_t) {
                    typeName = p.type().getTypename();
                }
            }
            std::cout<<typeName<<" "<<p.getName()<<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
        for (auto & p : m.m_outParams) {
            if (p.type().kind() != type_kind::enum_t) {
                typeName = tryConvertType(p.type().getBuiltinType());
                if (p.type().kind() != type_kind::builtin_t) {
                    typeName = p.type().getTypename();
                }
            }
            std::cout<<typeName<<" "<<p.getName()<<" = "<<std::to_string(fieldIndex++)<<";"<<std::endl;
        }
        std::cout<<"}"<<std::endl<<std::endl;
    }
}

void GRPCProtoGenerator::generate(const ClassDescriptor & c)
{
    generateService(c);
    for (auto & methodDesc : c.methods()) {
        generateMessages(methodDesc);
    }
}
