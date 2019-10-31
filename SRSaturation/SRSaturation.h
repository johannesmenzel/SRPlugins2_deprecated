#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "DSP/SRDynamics.h"
#include "DSP/SRFilters.h"


const int kNumPrograms = 1;

enum EParams
{
  kDrive = 0,
  kSaturation,
  kOutputGain,
  kNumParams
};

enum ECtrlTags {
  cDrive = 0,
  cSaturation,
  cOutputGain,
  // ---
  cEnergyMeter,
  cNumCtrlTags
};

using namespace iplug;
using namespace igraphics;

class SRSaturation : public Plugin
{
public:
  SRSaturation(const InstanceInfo& info);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset();
  void OnIdle();

private:
  double mCurrentEnvelopeState;
  SR::DSP::SRDynamicsDetector fEnvelope;
  SR::DSP::SRFilterIIR<sample, 2> fLowPass;
  IVMeterControl<1>::Sender mEnergyMeter{ cEnergyMeter };
 
#endif
};
