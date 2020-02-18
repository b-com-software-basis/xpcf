#ifndef PeopleComponent_H
#define PeopleComponent_H
#include <xpcf/component/ComponentBase.h>
#include "IMessage.h"


class PeopleComponent : public org::bcom::xpcf::ComponentBase, virtual public IMessage
{
public:
    PeopleComponent();
    ~PeopleComponent() override;
    void unloadComponent () override;
    void print() override;
};




template <> struct org::bcom::xpcf::ComponentTraits<PeopleComponent>
{
    static constexpr const char * UUID = "8D664B2A-16D5-4D7C-8675-E85D244CD8EE";
    static constexpr const char * NAME = "PeopleComponent";
    static constexpr const char * DESCRIPTION = "PeopleComponent implements IMessage interface";
};

#endif // PeopleComponent_H
