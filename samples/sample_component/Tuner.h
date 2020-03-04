#ifndef TUNER_H
#define TUNER_H
#include <xpcf/component/ConfigurableBase.h>

#include "IGuitar.h"

namespace component {

class Tuner : public org::bcom::xpcf::ConfigurableBase, virtual public ITuner
{
public:
    Tuner();
    ~Tuner() override;
    void unloadComponent () override;

    TuneType getTuneType () override { return static_cast<TuneType>(m_tuneType); }
    void setTuneType(const TuneType & tuneType) override { m_tuneType = tuneType; }

    org::bcom::xpcf::XPCFErrorCode onConfigured() override;
    void onInjected() override;

private:
    uint64_t m_tuneType;
};
}


template <> struct org::bcom::xpcf::ComponentTraits<component::Tuner>
{
    static constexpr const char * UUID = "{5644b791-b684-4203-8479-391776f3f623}";
    static constexpr const char * NAME = "Tuner";
    static constexpr const char * DESCRIPTION = "Tuner implements ITuner interface";
};


#endif // TUNER_H
