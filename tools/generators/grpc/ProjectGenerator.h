#ifndef PROJECTGENERATOR_H
#define PROJECTGENERATOR_H
#include <xpcf/component/ComponentBase.h>

#include "AbstractGenerator.h"



class ProjectGenerator : public AbstractGenerator
{
public:
    ProjectGenerator();
    ~ProjectGenerator() override;
    std::map<MetadataType,std::string> generate(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata) override;

};




template <> struct org::bcom::xpcf::ComponentTraits<ProjectGenerator>
{
    static constexpr const char * UUID = "{9d465f72-0935-4f64-8620-7fd349edcb9a}";
    static constexpr const char * NAME = "ProjectGenerator";
    static constexpr const char * DESCRIPTION = "ProjectGenerator implements AbstractGenerator interface";
};

#endif // PROJECTGENERATOR_H
