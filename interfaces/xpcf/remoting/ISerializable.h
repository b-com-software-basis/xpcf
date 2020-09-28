#ifndef ISERIALIZABLE_H
#define ISERIALIZABLE_H


#include <xpcf/api/IComponentIntrospect.h>
#include <xpcf/remoting/BaseBuffer.h>

// template typename T to_proto/from_proto OR another solution with IGrpcProtoInterface / IRemotingBufferInterface ??
// Simple backend [proto|flat] buffers
// Data class C either :
// - inherits from [proto|flat] buffer message class. C.[proto|fbs] file exists
// - can be converted to [proto|flat]::C class with template methods : a C.proto or C.fbs file must exists in the source folder
// - inherits ISerializable ([de]serialize from/to bytes buffer)
// - can be serialized to bytes buffer with template overload

//XPCF remoting study
//Message options:
//1/ custom buffered messages
//- serialisation interface
//- serialisation  generation from cppast using flexbuffers, capnproto. ... or dedicated serialisation type

//2/ standard messages based upon proto/flat buffers
//- proto/flat buffers file generation

//Service :
//1 -> type agnostic in/out service for serialisation generation/interface
//2 -> proto/flat based service for proto/flat defined datastructure

//serialisation generation/interface :
//-> pros :
//- allows an easier integration in existing c++ code

//-> cons :
//- needs to generate deserialisation code in other languages too

//proto/flat buffers messages :
//-> pros :
//- cross languages deserialisation is provided out of the box

//-> cons :
//- needs to generate/create proto files
//- adds a dependency toward proto/flat buffers plugin for grpc

//Endpoint notion ? Interface with destination configuration ?
namespace org { namespace bcom { namespace xpcf {


class Serializable
{
public:
    virtual ~Serializable() = default;
    virtual std::vector<uint8_t> serialize() = 0;
    virtual void deserialize(const std::vector<uint8_t> & buffer) = 0;
};

class ISerializable: virtual public org::bcom::xpcf::IComponentIntrospect
{
public:
    virtual ~ISerializable() override = default;
    virtual std::vector<uint8_t> serialize() = 0;
    virtual void deserialize(const std::vector<uint8_t> & buffer) = 0;
};

template <typename T> std::vector<uint8_t> serialize(SRef<T> & object);

template <typename T> SRef<T> deserialize(const std::vector<uint8_t> & buffer);

template <typename T> std::vector<uint8_t> serialize(const T & object);

template <typename T> T deserialize(const std::vector<uint8_t> & buffer);

template <> struct InterfaceTraits<ISerializable>
{
    static constexpr const char * UUID = "{dfac12dd-a4be-4594-be28-f06949ca97b3}";
    static constexpr const char * NAME = "ISerializable";
    static constexpr const char * DESCRIPTION = "ISerializable interface description";
};

}}}

#endif // ISERIALIZABLE_H
