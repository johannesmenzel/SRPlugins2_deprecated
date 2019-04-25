#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "verbengine.h"
#include "../SRClasses/DSP/SRFilters.h"
#include "../SRClasses/DSP/SRDynamics.h"

const int kNumPrograms = 1;

enum EParams
{
  kDry = 0,
  kWidth,
  kDamp,
  kSize,
  kWet,
  kHighpass,
  kLowpass,
  kCompress,
  kNumParams
};

class SRReverb : public IPlug
{
public:
  SRReverb(IPlugInstanceInfo instanceInfo);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;

private:
  WDL_ReverbEngine fReverb;
  SR::DSP::SRFilterIIR<sample, 2> fHighpass, fLowpass;
  SR::DSP::SRCompressorRMS fCompressor;
  double mDry, mWidth, mDamp, mSize, mWet, mHighpass, mLowpass, mCompress;

  double mCompThresh, mCompRatio, mCompAttack, mCompRelease;
#endif
};
