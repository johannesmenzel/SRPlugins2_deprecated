#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "verbengine.h"
#include "simple_pitchshift.h"
//#include "simple_pitchshift2.h"
#include "DSP/SRFilters.h"
#include "DSP/SRDynamics.h"


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
  kPitchShift,
  kPitchTempo,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class SRReverb : public Plugin
{
public:
  SRReverb(const InstanceInfo& info);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;

private:
  WDL_ReverbEngine fReverb;
  WDL_SimplePitchShifter fPitch;
  //WDL_SimplePitchShifter2 fPitch2;
  SR::DSP::SRFilterIIR<sample, 2> fHighpass, fLowpass;
  SR::DSP::SRCompressorRMS fCompressor;
  double mDry, mWidth, mDamp, mSize, mWet, mHighpass, mLowpass, mCompress;
  double mCompThresh, mCompRatio, mCompAttack, mCompRelease;
  double mPitchShift, mPitchTempo;

#endif
};
