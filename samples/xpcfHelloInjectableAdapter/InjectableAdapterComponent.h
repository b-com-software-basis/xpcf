#ifndef InjectableAdapterComponent_H
#define InjectableAdapterComponent_H
#include <xpcf/component/ComponentBase.h>
#include "IMessage.h"

#include <string>


class InjectableAdapterComponent : public org::bcom::xpcf::ComponentBase, public virtual IMessage
{
public:
    InjectableAdapterComponent();
    ~InjectableAdapterComponent() override;
    void unloadComponent () override;
    void print() override;

private:
    SRef<IMessage> m_concreteMessageHandler;
};




template <> struct org::bcom::xpcf::ComponentTraits<InjectableAdapterComponent>
{
    static constexpr const char * UUID = "490A887F-1B64-44A1-902E-0180D8C6FACD";
    static constexpr const char * NAME = "InjectableAdapterComponent";
    static constexpr const char * DESCRIPTION = "InjectableAdapterComponent implements IMessage interface";
};

#endif // InjectableAdapterComponent_H
