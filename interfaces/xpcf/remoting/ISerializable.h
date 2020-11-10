#ifndef ISERIALIZABLE_H
#define ISERIALIZABLE_H


#include <xpcf/api/IComponentIntrospect.h>
#include <xpcf/remoting/BaseBuffer.h>

#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
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
    virtual std::string serialize() = 0;
    virtual void deserialize(const std::string & buffer) = 0;
};

class ISerializable: virtual public org::bcom::xpcf::IComponentIntrospect
{
public:
    virtual ~ISerializable() override = default;
    virtual std::string serialize() = 0;
    virtual void deserialize(const std::string & buffer) = 0;
};

template <typename T, typename S = boost::archive::binary_oarchive> std::string serialize(const T & object)
{
    std::stringstream ss;
    S oa(ss);
    oa << object;
    return ss.str();
}

template <typename T, typename S = boost::archive::binary_iarchive> T deserialize(const std::string & buffer)
{
    std::stringstream ss(buffer);
    S ia(ss);
    T obj; // T must be default-constructible
    ia >> obj;
    return obj;
}

template <> struct InterfaceTraits<ISerializable>
{
    static constexpr const char * UUID = "{dfac12dd-a4be-4594-be28-f06949ca97b3}";
    static constexpr const char * NAME = "ISerializable";
    static constexpr const char * DESCRIPTION = "ISerializable interface description";
};

}}}

namespace boost {
namespace serialization {

template<uint N>
struct Serialize
{
    template<class Archive, typename... Args>
    static void serialize(Archive & ar, std::tuple<Args...> & t, const unsigned int version)
    {
        ar & std::get<N-1>(t);
        Serialize<N-1>::serialize(ar, t, version);
    }
};

template<>
struct Serialize<0>
{
    template<class Archive, typename... Args>
    static void serialize(Archive & ar, std::tuple<Args...> & t, const unsigned int version)
    {
        (void) ar;
        (void) t;
        (void) version;
    }
};

template<class Archive, typename... Args>
void serialize(Archive & ar, std::tuple<Args...> & t, const unsigned int version)
{
    Serialize<sizeof...(Args)>::serialize(ar, t, version);
}

}
}


#endif // ISERIALIZABLE_H
