#ifndef IMESSAGE_H
#define IMESSAGE_H


#include <xpcf/api/IComponentIntrospect.h>



class IMessage: virtual public org::bcom::xpcf::IComponentIntrospect
{
public:
    virtual ~IMessage() override = default;
    virtual void print() = 0;
};



template <> struct org::bcom::xpcf::InterfaceTraits<IMessage>
{
    static constexpr const char * UUID = "9E04C615-8EE7-4C52-83A4-8104A209CAFC";
    static constexpr const char * NAME = "IMessage";
    static constexpr const char * DESCRIPTION = "IMessage interface description";
};

#endif // IMESSAGE_H
