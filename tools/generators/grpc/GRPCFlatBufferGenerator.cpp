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

GRPCFlatBufferGenerator::GRPCFlatBufferGenerator():xpcf::ComponentBase(xpcf::toMap<GRPCFlatBufferGenerator>())
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


GRPCFlatBufferGenerator::~GRPCFlatBufferGenerator()
{

}

void GRPCFlatBufferGenerator::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    // delete this;
    return;
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

}

void GRPCFlatBufferGenerator::generate(const ClassDescriptor & c)
{
    generateService(c);
}
