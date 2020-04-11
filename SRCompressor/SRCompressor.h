#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "SRConstants.h"
#include "Graphics/SRControls.h"
#include "DSP/SRBuffer.h"
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
  // Meter
  cGrMeter,
  kNumCtrlTags
};

using namespace iplug;
using namespace igraphics;

class SRCompressor final: public Plugin
{
public:
  SRCompressor(const InstanceInfo& info);


#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnIdle();
  void OnParamChange(int paramIdx) override;
  const double GetCompressorValues(int stage, int param, double value);


private:
  double mSamplerate;
  double mThresh, mRatio, mAttack, mRelease, mKneeWidth, mSidechainFc, mMaxGr, mMakeup, mReference, mMix;
  bool mIsMaxGrRatioDependent, mIsFeedback, mIsAutoMakeup, mIsBypassed;

  SR::DSP::SRCompressor fCompLevel;
  SR::DSP::SRCompressor fCompOpto;
  SR::DSP::SRDeesser fDeesser;
  SR::DSP::SRCompressor fCompFet;
  SR::DSP::SRCompressor fCompLim;

  SR::DSP::SRSaturation fSoftSatInput[2] = {};
  SR::DSP::SRSaturation fSoftSatOutput[2] = {};
#endif
  SR::Graphics::Base::SRRoomInfo mRoomInfo;

  SR::Graphics::Controls::SRMeter<5, 1024>::SRMeterBallistics mGrMeterBallistics{ cGrMeter };
  SR::DSP::SRBuffer<sample, 5, 1024> bGrMeter;



};

