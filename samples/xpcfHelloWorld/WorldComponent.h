#ifndef WORLDCOMPONENT_H
#define WORLDCOMPONENT_H
#include <xpcf/component/ComponentBase.h>
#include "IMessage.h"


class WorldComponent : public org::bcom::xpcf::ComponentBase, virtual public IMessage
{
public:
    WorldComponent();
    ~WorldComponent() override;
    void unloadComponent () override;
    void print() override;
};




template <> struct org::bcom::xpcf::ComponentTraits<WorldComponent>
{
    static constexpr const char * UUID = "C97EA5EF-93DA-46ED-9A1C-AF9AC8F5F5A9";
    static constexpr const char * NAME = "WorldComponent";
    static constexpr const char * DESCRIPTION = "WorldComponent implements IMessage interface";
};

#endif // WORLDCOMPONENT_H
