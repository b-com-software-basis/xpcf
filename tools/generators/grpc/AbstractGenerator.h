#ifndef ABSTRACTGENERATOR_H
#define ABSTRACTGENERATOR_H
#include <xpcf/component/ComponentBase.h>

#include "interfaces/IRPCGenerator.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

namespace fs = boost::filesystem;

class AbstractGenerator : public org::bcom::xpcf::ComponentBase, virtual public IRPCGenerator
{
public:
    AbstractGenerator(std::map<std::string,std::string> componentInfosMap);
    ~AbstractGenerator() override;
    void unloadComponent () override final;
    void setDestinationFolder(const std::string & folder) override;
    void setGenerateMode(const GenerateMode & mode = GenerateMode::STD_COUT) override;

protected:
    GenerateMode m_mode;
    fs::path m_folder;

};

#endif // ABSTRACTGENERATOR_H
