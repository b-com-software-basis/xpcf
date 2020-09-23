#include "GRPCFlatBufferGenerator.h"

namespace xpcf = org::bcom::xpcf;

template<> GRPCFlatBufferGenerator * xpcf::ComponentFactory::createInstance<GRPCFlatBufferGenerator>();

const std::map<enum cpp_builtin_type,std::string> builtinType2flatbufferTypeMap =
{
    { cpp_void,""},

        { cpp_bool,""},

        { cpp_uchar,""},
        { cpp_ushort,""},
        { cpp_uint,""},
        { cpp_ulong,""},
        { cpp_ulonglong,""},
        { cpp_uint128,""},

        { cpp_schar,""},
        { cpp_short,""},
        { cpp_int,""},
        { cpp_long,""},
        { cpp_longlong,""},
        { cpp_int128,""},

        { cpp_float,""},
        { cpp_double,""},
        { cpp_longdouble,""},
        { cpp_float128,""},

        { cpp_char,""},
        { cpp_wchar,""},
        { cpp_char16,""},
        { cpp_char32,""},

        { cpp_nullptr,""},
        { cpp_int8_t,""},
        { cpp_int16_t,""},
        { cpp_int32_t,""},
        { cpp_int64_t,""},

        { cpp_uint8_t,""},
        { cpp_uint16_t,""},
        { cpp_uint32_t,""},
        { cpp_uint64_t,""},

        { cpp_int_least8_t,""},
        { cpp_int_least16_t,""},
        { cpp_int_least32_t,""},
        { cpp_int_least64_t,""},

        { cpp_uint_least8_t,""},
        { cpp_uint_least16_t,""},
        { cpp_uint_least32_t,""},
        { cpp_uint_least64_t,""},

        { cpp_int_fast8_t,""},
        { cpp_int_fast16_t,""},
        { cpp_int_fast32_t,""},
        { cpp_int_fast64_t,""},

        { cpp_uint_fast8_t,""},
        { cpp_uint_fast16_t,""},
        { cpp_uint_fast32_t,""},
        { cpp_uint_fast64_t,""},

        { cpp_intptr_t,""},
        { cpp_uintptr_t,""},

        { cpp_intmax_t,""},
        { cpp_uintmax_t,""},
};

GRPCFlatBufferGenerator::GRPCFlatBufferGenerator():AbstractGenerator(xpcf::toMap<GRPCFlatBufferGenerator>())
{
}


GRPCFlatBufferGenerator::~GRPCFlatBufferGenerator()
{

}

inline const std::string & tryConvertType(enum cpp_builtin_type type)
{
    static const std::string typeStr = "";
    if (builtinType2flatbufferTypeMap.find(type) != builtinType2flatbufferTypeMap.end()) {
        return builtinType2flatbufferTypeMap.at(type);
    }
    return typeStr;
}

void GRPCFlatBufferGenerator::generateService(const ClassDescriptor &c)
{
    std::cout<<"rpc_service grpc"<<c.getName()<<"Service {"<<std::endl;
    for (auto & methodDesc : c.methods()) {
        std::string inRequest = "Empty";
        std::string outRequest = "Empty";
        std::string streaming;

        if (methodDesc.m_inParams.size() != 0 || methodDesc.m_inoutParams.size() != 0) {
            methodDesc.m_requestName = methodDesc.m_rpcName + "Request";
        }
        if (methodDesc.m_outParams.size() != 0 || methodDesc.m_inoutParams.size() != 0 || !methodDesc.returnType().isVoid()) {
            methodDesc.m_responseName = methodDesc.m_rpcName + "Response";
        }
        if ((methodDesc.streamingType() == MethodDescriptor::streaming_type::client) && methodDesc.m_requestName != "Empty") {
            streaming += "(streaming: client)";
        }
        if ((methodDesc.streamingType() == MethodDescriptor::streaming_type::server)  && methodDesc.m_responseName != "Empty"){
            streaming += "(streaming: server)";
        }
        if ((methodDesc.streamingType() == MethodDescriptor::streaming_type::bidir)  && methodDesc.m_responseName != "Empty" && methodDesc.m_requestName != "Empty"){
            streaming += "(streaming: bidi)";
        }
        std::cout<<methodDesc.m_rpcName<<"("<<methodDesc.m_requestName<<"):"<<methodDesc.m_responseName<<" "<<streaming<<";"<<std::endl;
    }
    std::cout<<"}"<<std::endl;
}

void GRPCFlatBufferGenerator::generateMessages(const MethodDescriptor & m)
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

void GRPCFlatBufferGenerator::generate(const ClassDescriptor & c)
{
    generateService(c);
}
