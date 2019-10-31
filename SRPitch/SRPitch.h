#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "DSP/SRFilters.h"

const int kNumPrograms = 1;

enum EParams
{
  kWet = 0,
  kLp,
  kHp,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class SRPitch : public Plugin
{
public:
  SRPitch(const InstanceInfo& info);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;

private:
  double mSampleRate, mWet, mHpFreq, mLpFreq;
  SR::DSP::SRFilterIIR<sample, 2> fLowpass, fDCBlocker;



#endif
};
