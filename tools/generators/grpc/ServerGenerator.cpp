#include "ServerGenerator.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/date_time.hpp>

namespace xpcf = org::bcom::xpcf;

template<> ServerGenerator * xpcf::ComponentFactory::createInstance<ServerGenerator>();

ServerGenerator::ServerGenerator():AbstractGenerator(xpcf::toMap<ServerGenerator>())
{
}


ServerGenerator::~ServerGenerator()
{

}

void ServerGenerator::processHeaderMethods(const SRef<ClassDescriptor> c, CppBlockManager & blockMgr)
{
    for (auto m : c->methods()) {
        std::string request = m->m_requestName;
        std::string response = m->m_responseName;
        blockMgr.out() << "::grpc::Status " + m->m_rpcName + "(::grpc::ServerContext* context, const " + request + "* request, " + response + "* response) override;\n";
    }
    blockMgr.newline();
}


void ServerGenerator::generateHeader(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata, std::ostream& out)
{
    //NOTE : server is configurable to set grpc channel etc...
    xpcf::uuids::uuid serverUUID = c->getServerUUID();
    if (serverUUID.is_nil()) {
        serverUUID = c->createServerUUID();
        m_serviceUuidMap[m_className] = xpcf::uuids::to_string(serverUUID);
    }

    CppBlockManager blockMgr(out);
    blockMgr.out() << "// GRPC Server Class Header generated with xpcf_grpc_gen\n";
    blockMgr.newline();
    blockMgr.includeGuardsStart(m_className);
    blockMgr.include(c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACEFILEPATH)); // relative or absolute path?? TODO:  retrieve filepath from metadata
    blockMgr.include("xpcf/component/ConfigurableBase.h",false);
    blockMgr.include("xpcf/remoting/IGrpcService.h",false);
    blockMgr.include("xpcf/remoting/GrpcHelper.h",false);
    blockMgr.include((*c)[ClassDescriptor::MetadataType::GRPCSERVICENAME] + ".grpc.pb.h");
    blockMgr.include("grpc/grpc.h",false);
    blockMgr.newline();

    blockMgr.out() << "namespace " + m_nameSpace;
    blockMgr.out() << " ";
    {
        block_guard<CPP::NSPACE> scopedNspace(blockMgr);
        blockMgr.newline();
        blockMgr.out() << "class " + m_className + ":  public org::bcom::xpcf::ConfigurableBase, virtual public org::bcom::xpcf::IGrpcService\n";
        {
            block_guard<CPP::CLASS> classBlk(blockMgr);
            {
                block_guard<CPP::PUBLIC> publicBlk(blockMgr);
                blockMgr.out() <<  m_className +"();\n";
                blockMgr.out() << "~"+ m_className +"() override = default;\n";
                blockMgr.out() << "::grpc::Service * getService() override;\n";
                blockMgr.out() << "const char * getServiceName() override { return \""<< c->getName() <<"\"; }\n";
                blockMgr.out() << "void unloadComponent () override final;\n";
                blockMgr.out() << "org::bcom::xpcf::XPCFErrorCode onConfigured() override;\n";
                blockMgr.newline();

                blockMgr.out() << "class " + m_grpcClassName + "Impl:  public ::" +  c->getMetadata().at(ClassDescriptor::MetadataType::REMOTINGNSPACE) + "::" + m_grpcClassName+"::Service\n";
                {
                    block_guard<CPP::CLASS> innerClassBlk(blockMgr);
                    {
                        block_guard<CPP::PUBLIC> innerPublicBlk(blockMgr);
                        blockMgr.out() << m_grpcClassName + "Impl() = default;\n";// missing something to set the ref toward the injected I* component
                        // foreach method
                        processHeaderMethods(c,blockMgr);
                        std::string baseInterface;
                        if (!c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE).empty()) {
                            baseInterface = c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE) + "::";
                        }
                        baseInterface += c->getName();
                        blockMgr.out() << "SRef<" + baseInterface + "> m_xpcfComponent;\n";
                        if (!c->compressionDisabled()) {
                            blockMgr.out() << "xpcf::grpcServerCompressionInfos m_serviceCompressionInfos;\n";
                            blockMgr.out() << "std::map<std::string, xpcf::grpcServerCompressionInfos> m_methodCompressionInfosMap;\n";
                        }
                    }
                }
            }
            {
                block_guard<CPP::PRIVATE> privateBlk(blockMgr);
                blockMgr.out() << m_grpcClassName + "Impl m_grpcService;\n";
                if (!c->compressionDisabled()) {
                    blockMgr.out() << "std::vector<std::string> m_grpcServerCompressionConfig;\n";
                }
            }
        }
    }

    blockMgr.newline();

    blockMgr.out() << "template <> struct org::bcom::xpcf::ComponentTraits<" + m_nameSpace + "::" + m_className +">\n";//xpcf::grpc::proxy::c.name::c.name_grpcProxy>
    {
        block_guard<CPP::CLASS> classBlk(blockMgr);
        blockMgr.out() << "static constexpr const char * UUID = \"" + xpcf::uuids::to_string(serverUUID) + "\";\n";
        blockMgr.out() << "static constexpr const char * NAME = \"" + m_className + "\";\n";
        blockMgr.out() << "static constexpr const char * DESCRIPTION = \"" + m_className + " grpc server component\";\n";
    }
    blockMgr.includeGuardsEnd();
}

void ServerGenerator::bindInput(const ParameterDescriptor & p, CppBlockManager & blockMgr)
{
    std::stringstream requestParamTranscription;
    if (p.ioType() == ParameterDescriptor::io_type::in || p.ioType() == ParameterDescriptor::io_type::inout) {
        if (p.type().kind() == type_kind::builtin_t
                || p.type().kind() == type_kind::enum_t
                || p.type().kind() == type_kind::std_string_t) {
            if (p.type().needsStaticCast()) {
                requestParamTranscription << " = static_cast<" << p.type().getFullTypeDescription() << ">(request->" << boost::to_lower_copy(p.getName()) << "());\n";
            }
            else {
                requestParamTranscription << " = request->" << boost::to_lower_copy(p.getName()) << "();\n";
            }
        }
        else {
            requestParamTranscription << " = xpcf::deserialize<"<<p.type().getFullTypeDescription()<<">(request->" << boost::to_lower_copy(p.getName()) << "());\n";
        }
    }
    else {
        requestParamTranscription << ";\n";
    }

    blockMgr.out() << p.type().getFullTypeDescription() << " " << p.getName() << requestParamTranscription.str();
}

void ServerGenerator::bindOutput(const ParameterDescriptor & p, CppBlockManager & blockMgr)
{
    if (p.ioType() == ParameterDescriptor::io_type::inout || p.ioType() == ParameterDescriptor::io_type::out) {
        if (p.type().kind() == type_kind::builtin_t
                || p.type().kind() == type_kind::enum_t
                || p.type().kind() == type_kind::std_string_t) {
            if (p.type().needsStaticCast()) {
                blockMgr.out() << "response->set_"<< boost::to_lower_copy(p.getName()) <<"(static_cast<"<< p.type().getRPCType() <<">("<<p.getName() <<"));\n";
            }
            else {
                blockMgr.out() << "response->set_" << boost::to_lower_copy(p.getName()) << "(" << p.getName() << ");\n";
            }
        }
        else if (p.type().kind() == type_kind::user_defined_t
                 || p.type().kind() == type_kind::template_t) {
            blockMgr.out() << "response->set_" << boost::to_lower_copy(p.getName()) << "(xpcf::serialize<" << p.type().getFullTypeDescription() <<">(" << p.getName() << "));\n";
        }
    }
}

void ServerGenerator::processBodyMethods(const SRef<ClassDescriptor> c, const std::map<MetadataType,std::string>& metadata, CppBlockManager & blockMgr)
{
    for (auto m : c->methods()) {
        std::string request = m->m_requestName;
        std::string response = m->m_responseName;

        blockMgr.out() << "::grpc::Status " + m_className +"::"+ m_grpcClassName + "Impl::" + m->m_rpcName + "(::grpc::ServerContext* context, const "+ request + "* request, " + response + "* response)\n";
        {
            block_guard methodBlock(blockMgr);

            blockMgr.out() << "auto prop = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();\n";
            blockMgr.out() << "auto currentCtx = opentelemetry::context::RuntimeContext::GetCurrent();\n";
            blockMgr.out() << "GrpcServerCarrier carrier(context);\n";
            blockMgr.out() << "auto newContext = prop->Extract(carrier, currentCtx);\n";
            blockMgr.out() << "ContextScope ctxtScope(newContext);\n";
            blockMgr.out() << "\n";
            blockMgr.out() << "opentelemetry::trace::StartSpanOptions options;\n";
            blockMgr.out() << "options.kind = opentelemetry::trace::SpanKind::kServer;\n";
            blockMgr.out() << "options.parent = opentelemetry::trace::GetSpan(newContext)->GetContext();\n";
            blockMgr.out() << "\n";
            blockMgr.out() << "auto provider = opentelemetry::trace::Provider::GetTracerProvider();\n";
            blockMgr.out() << "auto tracer = provider->GetTracer(\"" << metadata.at(IRPCGenerator::MetadataType::MODULE_NAME) << "\", \"" << metadata.at(IRPCGenerator::MetadataType::PROJECT_VERSION) << "\");\n";
            blockMgr.out() << "auto span = tracer->StartSpan(\""<<m_className<<"."<<m->getName()<<"\",\n";
            blockMgr.out() << "                              {{opentelemetry::semconv::rpc::kRpcSystem, \"grpc\"},\n";
            blockMgr.out() << "                               {opentelemetry::semconv::rpc::kRpcService, \"" << c->getMetadata().at(ClassDescriptor::MetadataType::REMOTINGNSPACE) << "." << m_grpcClassName << "\"},\n";
            blockMgr.out() << "                               {opentelemetry::semconv::rpc::kRpcMethod, \"" << m->m_rpcName << "\"},\n";
            blockMgr.out() << "                               {opentelemetry::semconv::rpc::kRpcGrpcStatusCode, 0}},\n";
            blockMgr.out() << "                              options);\n";
            blockMgr.out() << "SpanScope spanScope(span);\n";
            blockMgr.out() << "auto scope= tracer->WithActiveSpan(span);\n";
            blockMgr.out() << "\n";
            
            std::stringstream methodCall;
            if (!m->returnType().isVoid()) {
                if (m->returnType().kind() == type_kind::c_string_t) {
                    methodCall << "std::string" << " returnValue = ";
                }
                else {
                    methodCall << m->returnType().getFullTypeDescription()<< " returnValue = ";
                }
            }
            if (m->hasOutputs() && !m->compressionDisabled()) {
                blockMgr.out() <<"#ifndef DISABLE_GRPC_COMPRESSION\n";
                blockMgr.out() << "xpcf::grpcCompressType askedCompressionType = static_cast<xpcf::grpcCompressType>(request->grpcservercompressionformat());\n";
                blockMgr.out() << "xpcf::grpcServerCompressionInfos serverCompressInfo = xpcf::deduceServerCompressionType(askedCompressionType, m_serviceCompressionInfos, \""<<m->getName()<<"\", m_methodCompressionInfosMap);\n";
                blockMgr.out() << "xpcf::prepareServerCompressionContext(context, serverCompressInfo);\n";
                blockMgr.out() <<"#endif\n";
            }
            blockMgr.out() <<"#ifdef ENABLE_SERVER_TIMERS\n";
            blockMgr.out() <<"boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();\n";
            blockMgr.out() <<"std::cout << \"====> "<<m_className<<"::"<<m->getName()<<" request received at \" << to_simple_string(start) << std::endl;\n";
            blockMgr.out() <<"#endif\n";
            methodCall << "m_xpcfComponent->" << m->getName() << "(";
            uint32_t nbTypes = 0;
            for (SRef<ParameterDescriptor> p : m->m_params) {
                if (nbTypes != 0) {
                    methodCall << ", ";
                }
                bindInput(*p, blockMgr);
                methodCall << p->getName();
                nbTypes++;
            }
            blockMgr.out() << methodCall.str() << ");\n";
            for (SRef<ParameterDescriptor> p : m->m_params) {
                bindOutput(*p, blockMgr);
            }
            if (!m->returnType().isVoid()) {
                // TODO : serialize return type !!!
                if (m->returnType().kind() == type_kind::builtin_t) {
                    if (m->returnType().needsStaticCast()) {
                        blockMgr.out() << "response->set_xpcfgrpcreturnvalue(static_cast<"<< m->returnType().getRPCType() <<">(returnValue));\n";
                    }
                    else {
                        blockMgr.out() << "response->set_xpcfgrpcreturnvalue(returnValue);\n";
                    }
                }
                else if ((m->returnType().kind() == type_kind::user_defined_t) || (m->returnType().kind() == type_kind::template_t)) {
                    blockMgr.out() << "response->set_xpcfgrpcreturnvalue(xpcf::serialize<" << m->returnType().getFullTypeDescription() << ">(returnValue));\n";
                }
                else if (m->returnType().kind() == type_kind::enum_t) {
                    if (m->returnType().needsStaticCast()) {
                        blockMgr.out() << "response->set_xpcfgrpcreturnvalue(static_cast<"<< m->returnType().getRPCType() <<">(returnValue));\n";
                    }
                    else {
                        blockMgr.out() << "response->set_xpcfgrpcreturnvalue(returnValue);\n";
                    }
                }
                else if (m->returnType().kind() == type_kind::c_string_t ) {
                    blockMgr.out() << "response->set_xpcfgrpcreturnvalue(returnValue);\n";
                }
            }
            blockMgr.out() <<"#ifdef ENABLE_SERVER_TIMERS\n";
            blockMgr.out() <<"boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();\n";
            blockMgr.out() <<"std::cout << \"====> "<<m_className<<"::"<<m->getName()<<" response sent at \" << to_simple_string(end) << std::endl;\n";
            blockMgr.out() <<"std::cout << \"   => elapsed time = \" << ((end - start).total_microseconds() / 1000.00) << \" ms\" << std::endl;\n";
            blockMgr.out() <<"#endif\n";
            //boost::posix_time::ptime end = boost::posix_time::microsec_clock::local_time();
            //std::cout << “====>IMappingPipeline_grpcServer::init response sent at “ << to_simple_string(end)
            
            blockMgr.out() << "return ::grpc::Status::OK;\n";
        }
        blockMgr.newline();
    }
}

void ServerGenerator::generateBody(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata, std::ostream& out)
{
    CppBlockManager blockMgr(out);
    out << "// GRPC Server Class implementation generated with xpcf_grpc_gen\n";
    out << "#include \"" + m_headerFileName + "\"\n";
    blockMgr.include("cstddef",false);
    blockMgr.include("boost/date_time.hpp",false);
    blockMgr.include("xpcf/remoting/ISerializable.h",false);
    blockMgr.include("xpcf/remoting/GrpcHelper.h",false);

    // the body will use the grpcCLient generated from the proto or flat generators hence the following inclusion :
    //#include "grpcgeneratedclient.grpc.[pb|fb].h"

    blockMgr.newline();

    blockMgr.include("opentelemetry/context/propagation/global_propagator.h",false);
    blockMgr.include("opentelemetry/context/propagation/text_map_propagator.h",false);
    blockMgr.include("opentelemetry/context/runtime_context.h",false);
    blockMgr.include("opentelemetry/nostd/shared_ptr.h",false);
    blockMgr.include("opentelemetry/nostd/string_view.h",false);
    blockMgr.include("opentelemetry/nostd/unique_ptr.h",false);
    blockMgr.include("opentelemetry/nostd/variant.h",false);
    blockMgr.include("opentelemetry/semconv/incubating/rpc_attributes.h",false);
    blockMgr.include("opentelemetry/trace/context.h",false);
    blockMgr.include("opentelemetry/trace/provider.h",false);
    blockMgr.include("opentelemetry/trace/span.h",false);
    blockMgr.include("opentelemetry/trace/span_metadata.h",false);
    blockMgr.include("opentelemetry/trace/span_startoptions.h",false);
    blockMgr.include("opentelemetry/trace/tracer.h",false);

    blockMgr.newline();

    blockMgr.out() << "namespace\n";
    blockMgr.out() << "{\n";
    blockMgr.out() << "\n";
    blockMgr.out() << "class GrpcServerCarrier : public opentelemetry::context::propagation::TextMapCarrier\n";
    blockMgr.out() << "{\n";
    blockMgr.out() << "public:\n";
    blockMgr.out() << "  explicit GrpcServerCarrier(grpc::ServerContext *context) : context_(context) {}\n";
    blockMgr.out() << "  GrpcServerCarrier() = default;\n";
    blockMgr.out() << "  virtual opentelemetry::nostd::string_view Get(\n";
    blockMgr.out() << "      opentelemetry::nostd::string_view key) const noexcept override\n";
    blockMgr.out() << "  {\n";
    blockMgr.out() << "    auto it = context_->client_metadata().find({key.data(), key.size()});\n";
    blockMgr.out() << "    if (it != context_->client_metadata().end())\n";
    blockMgr.out() << "    {\n";
    blockMgr.out() << "      return opentelemetry::nostd::string_view(it->second.data(), it->second.size());\n";
    blockMgr.out() << "    }\n";
    blockMgr.out() << "    return \"\";\n";
    blockMgr.out() << "  }\n";
    blockMgr.out() << "\n";
    blockMgr.out() << "  virtual void Set(opentelemetry::nostd::string_view /* key */,\n";
    blockMgr.out() << "                   opentelemetry::nostd::string_view /* value */) noexcept override\n";
    blockMgr.out() << "  {\n";
    blockMgr.out() << "    // Not required for server\n";
    blockMgr.out() << "  }\n";
    blockMgr.out() << "\n";
    blockMgr.out() << "  grpc::ServerContext *context_ = nullptr;\n";
    blockMgr.out() << "};\n";
    blockMgr.out() << "\n";
    blockMgr.out() << "class ContextScope {\n";
    blockMgr.out() << "public:\n";
    blockMgr.out() << "    explicit ContextScope(const opentelemetry::context::Context& context) {\n";
    blockMgr.out() << "        m_token = opentelemetry::context::RuntimeContext::Attach(context);\n";
    blockMgr.out() << "    }\n";
    blockMgr.out() << "\n";
    blockMgr.out() << "    ~ContextScope() {\n";
    blockMgr.out() << "        if (m_token) {\n";
    blockMgr.out() << "            opentelemetry::context::RuntimeContext::Detach(*m_token);\n";
    blockMgr.out() << "        }\n";
    blockMgr.out() << "    }\n";
    blockMgr.out() << "\n";
    blockMgr.out() << "    ContextScope(const ContextScope&) = delete;\n";
    blockMgr.out() << "    ContextScope& operator=(const ContextScope&) = delete;\n";
    blockMgr.out() << "\n";
    blockMgr.out() << "private:\n";
    blockMgr.out() << "    opentelemetry::nostd::unique_ptr<opentelemetry::context::Token> m_token;\n";
    blockMgr.out() << "};\n";
    blockMgr.out() << "\n";
    blockMgr.out() << "class SpanScope {\n";
    blockMgr.out() << "public:\n";
    blockMgr.out() << "    explicit SpanScope(const opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span> span) : m_span(span){}\n";
    blockMgr.out() << "\n";
    blockMgr.out() << "    ~SpanScope() {\n";
    blockMgr.out() << "      if (m_span) {\n";
    blockMgr.out() << "        m_span->End();\n";
    blockMgr.out() << "      }\n";
    blockMgr.out() << "    }\n";
    blockMgr.out() << "\n";
    blockMgr.out() << "    SpanScope(const ContextScope&) = delete;\n";
    blockMgr.out() << "    SpanScope& operator=(const SpanScope&) = delete;\n";
    blockMgr.out() << "\n";
    blockMgr.out() << "private:\n";
    blockMgr.out() << "    opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span> m_span;\n";
    blockMgr.out() << "};\n";
    blockMgr.out() << "} // namespace\n";

    blockMgr.newline();

    blockMgr.out() << "namespace xpcf = org::bcom::xpcf;\n";
    blockMgr.newline();
    blockMgr.out() << "template<> " + m_nameSpace + "::" + m_className + "* xpcf::ComponentFactory::createInstance<"+ m_nameSpace + "::" + m_className + ">();\n";
    blockMgr.newline();

    blockMgr.out() << "namespace " + m_nameSpace;
    blockMgr.out() << " ";
    {
        block_guard<CPP::NSPACE> nspaceBlk(blockMgr);
        blockMgr.newline();
        blockMgr.out() << m_className + "::" + m_className + "():xpcf::ConfigurableBase(xpcf::toMap<"+ m_className + ">())\n";
        {
            block_guard methodBlock(blockMgr);
            blockMgr.out() << "declareInterface<xpcf::IGrpcService>(this);\n";
            std::string baseInterface;
            if (!c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE).empty()) {
                baseInterface = c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE) + "::";
            }
            baseInterface += c->getName();
            blockMgr.out() << "declareInjectable<" + baseInterface + ">(m_grpcService.m_xpcfComponent);\n";
            if (!c->compressionDisabled()) {
                blockMgr.out() << "m_grpcServerCompressionConfig.resize("<< c->methods().size() + 1 <<");\n";
                blockMgr.out() << "declarePropertySequence(\"grpc_compress_server\", m_grpcServerCompressionConfig);\n";
            }
        }
        blockMgr.newline();
        blockMgr.out() << "void " + m_className + "::unloadComponent ()\n";
        {
            block_guard methodBlock(blockMgr);
            blockMgr.out() << "delete this;\n";
            blockMgr.out() << "return;\n";
        }
        blockMgr.newline();
        blockMgr.out() << "XPCFErrorCode " + m_className +"::onConfigured()\n";
        {
            block_guard methodBlock(blockMgr);
            if (!c->compressionDisabled()) {
                blockMgr.out() << "for (auto & grpcCompressionLine : m_grpcServerCompressionConfig) {\n;";
                blockMgr.out() << "      translateServerConfiguration(grpcCompressionLine, m_grpcService.m_serviceCompressionInfos, m_grpcService.m_methodCompressionInfosMap);\n";
                blockMgr.out() << "}\n";
            }
            blockMgr.out() << "return xpcf::XPCFErrorCode::_SUCCESS;\n";
        }
        blockMgr.newline();
        blockMgr.out() << "::grpc::Service * " + m_className +"::getService()\n";
        {
            block_guard methodBlock(blockMgr);
            blockMgr.out() << "return &m_grpcService;\n";
        }
        processBodyMethods(c, metadata, blockMgr);
    }
}

std::map<IRPCGenerator::MetadataType,std::string> ServerGenerator::generateImpl(SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata)
{
    m_nameSpace =  "org::bcom::xpcf::grpc::server" + c->getName();
    m_className = c->getName() + "_grpcServer";
    m_headerFileName = m_className + ".h";
    m_cppFileName = m_className + ".cpp";
    m_grpcClassName = (*c)[ClassDescriptor::MetadataType::GRPCSERVICENAME];
    if (m_mode == GenerateMode::STD_COUT) {
        generateHeader(c, metadata, std::cout);
        generateBody(c, metadata, std::cout);
        std::cout<<std::endl;
    }
    else {
        fs::detail::utf8_codecvt_facet utf8;
        fs::path headerFilePath(m_headerFileName,utf8);
        fs::path cppFilePath(m_cppFileName,utf8);
        headerFilePath = m_folder/headerFilePath;
        cppFilePath = m_folder/cppFilePath;
        std::ofstream headerFile(headerFilePath.generic_string(utf8).c_str(), std::ios::out);
        generateHeader(c, metadata, headerFile);
        headerFile.close();
        std::ofstream cppFile(cppFilePath.generic_string(utf8).c_str(), std::ios::out);
        generateBody(c, metadata, cppFile);
        cppFile.close();
    }
    metadata[SERVER_XPCFGRPCCOMPONENTNAME] = m_className;
    metadata[SERVER_XPCFGRPCNAMESPACE] = m_nameSpace;
    metadata[MetadataType::SERVER_HEADERFILENAME] = m_headerFileName;
    metadata[MetadataType::SERVER_CPPFILENAME] = m_cppFileName;
    return metadata;
}

void ServerGenerator::finalizeImpl(std::map<MetadataType,std::string> metadata)
{
    for (auto [name, uuid] : m_serviceUuidMap) {
        std::cout << "Component " << name << " generated uuid = [[xpcf::serverUUID(" << uuid <<")]]" << std::endl;
    }
    m_serviceUuidMap.clear();
}

