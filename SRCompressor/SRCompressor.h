#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "SRConstants.h"
#include "DSP/SRDynamics.h"
#include "DSP/SRSaturation.h"

const int kNumPrograms = 1;

enum EParams
{
  kThresh = 0,
  kRatio,
  kAttack,
  kRelease,
  kKneeWidth,
  kSidechainFc,
  kMaxGr,
  kMakeup,
  kReference,
  kMix,
  kIsMaxGrRatioDependent,
  kIsFeedback,
  kIsAutoMakeup,
  kIsBypassed,
  kNumParams
};

enum ECtrlTags
{
  cThresh = 0,
  cRatio,
  cAttack,
  cRelease,
  cKneeWidth,
  cSidechainFc,
  cMaxGr,
  cMakeup,
  cReference,
  cMix,
  cIsMaxGrRatioDependent,
  cIsFeedback,
  cIsAutoMakeup,
  cIsBypassed,
  kNumCtrlTags
};

class SRCompressor : public IPlug
{
public:
  SRCompressor(IPlugInstanceInfo instanceInfo);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;

private:
  double mSamplerate;
  double mThresh, mRatio, mAttack, mRelease, mKneeWidth, mSidechainFc, mMaxGr, mMakeup, mReference, mMix;
  bool mIsMaxGrRatioDependent, mIsFeedback, mIsAutoMakeup, mIsBypassed;

  SR::DSP::SRCompressor fCompressor;
  SR::DSP::SRSaturation fSoftSatInput[2] = {};
  SR::DSP::SRSaturation fSoftSatOutput[2] = {};
#endif
};
