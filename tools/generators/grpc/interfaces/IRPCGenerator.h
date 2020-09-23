#ifndef IRPCGENERATOR_H
#define IRPCGENERATOR_H


#include <xpcf/api/IComponentIntrospect.h>

#include "ClassDescriptor.h"

class IRPCGenerator: public virtual org::bcom::xpcf::IComponentIntrospect
{
public:
    typedef enum {
        STD_COUT = 0,
        FILE = 1
    } GenerateMode;
    virtual ~IRPCGenerator() override = default;
    virtual void setDestinationFolder(const std::string & folder) = 0;
    virtual void setGenerateMode(const GenerateMode & mode = GenerateMode::STD_COUT) = 0;
    virtual void generate(const ClassDescriptor & c) = 0;
};



template <> struct org::bcom::xpcf::InterfaceTraits<IRPCGenerator>
{
    static constexpr const char * UUID = "{b4329fcb-7fcf-4801-9315-169dfd42ba41}";
    static constexpr const char * NAME = "IRPCGenerator";
    static constexpr const char * DESCRIPTION = "IRPCGenerator interface description";
};

#endif // IRPCGENERATOR_H
