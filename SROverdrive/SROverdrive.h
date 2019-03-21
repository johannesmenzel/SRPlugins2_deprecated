#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "../SRClasses/DSP/SRFilters.h"
#include "IVMeterControl.h"

const int kNumPrograms = 1;

enum EParams
{
  kDrive = 0,
  kGain,
  kTone,
  kMix,
  kLowpassFreq,
  kBypass,
  kNumParams
};

enum ECtrlTags {
  cDrive = 0,
  cGain,
  cTone,
  cMix,
  cLowpassFreq,
  cBypass,
  cBoostMeter,
  kNumCtrlTags
};

class SROverdrive : public IPlug
{
public:
  SROverdrive(IPlugInstanceInfo instanceInfo);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnIdle() override;
  void OnParamChange(int paramIdx) override;

private:
  double mSampleRate;
  double mDrive, mDriveEnv, mFilterFreq, mGain, mMix, mLowpassFreq;
  bool mBypass;
  SR::DSP::SRFilterParamSmooth fSidechainSmoother[2];
  SR::DSP::SRFilterIIR<sample, 2> fLowpass, fHighpass, fLowpassDriven;
  IVMeterControl<1, 1024>::IVMeterBallistics cMeterBoost;
  sample** meterValues;
#endif
};
