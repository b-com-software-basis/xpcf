#ifndef ConfigurableComponent_H
#define ConfigurableComponent_H
#include <xpcf/component/ConfigurableBase.h>
#include "IMessage.h"
#include <string>

class ConfigurableComponent : public org::bcom::xpcf::ConfigurableBase, virtual public IMessage
{
public:
    ConfigurableComponent();
    ~ConfigurableComponent() override;
    void unloadComponent () override;
    void print() override;

private:
    std::string m_message;
};




template <> struct org::bcom::xpcf::ComponentTraits<ConfigurableComponent>
{
    static constexpr const char * UUID = "C77F20A5-9E06-4C42-B64B-5CC838FD9447";
    static constexpr const char * NAME = "ConfigurableComponent";
    static constexpr const char * DESCRIPTION = "ConfigurableComponent implements IMessage interface";
};

#endif // ConfigurableComponent_H
