#ifndef SRCHANNEL_H
#define SRCHANNEL_H

/*
Which current test buffer implementation:
1 - ordinary sample** buffer
2 - SRBuffer class (Ptrlist or vector of WDL_Typedbuf or T**)
*/
#define USEBUFFER 2

// Use automatic gain compensation
#define USEAGC

// C++ classes
#include <functional> // for oversampler std::function

// IPlug2 source
#include "IPlug_include_in_plug_hdr.h"

// Extra IPlug2 libs
#include "IVMeterControl.h"
#include "IVScopeControl.h"
#include "Oversampler.h"
//#include "IPlugQueue.h"

// SRClasses
#include "../SRClasses/SRConstants.h"
#if USEBUFFER >= 2
#include "../SRClasses/DSP/SRBuffer.h"
#endif // USEBUFFER
#include "../SRClasses/DSP/SRGain.h"
#include "../SRClasses/DSP/SRFilters.h"
#include "../SRClasses/DSP/SRDynamics.h"
#include "../SRClasses/DSP/SRSaturation.h"
#include "../SRClasses/Utils/SRHelpers.h"
#include "../SRClasses/Graphics/SRControls.h"
// Plugin constants
#ifndef SRCHANNELCONSTANTS_H
#include "SRChannelConstants.h"
#endif // !SRCHANNELCONSTANTS_H




class SRChannel : public IPlug
{
public:
  SRChannel(IPlugInstanceInfo instanceInfo);


#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  void OnIdle() override;
#endif
#if IPLUG_EDITOR
  void OnParamChangeUI(int paramIdx, EParamSource source = kUnknown) override;
#endif

private:
  // User Functions
  void MakePresets();
  void InitEffects();
  void GrayOutControls();
  void SetFreqMeterValues();

  // GLOBAL members
  double mSampleRate;
  int mNumInChannels, mNumOutChannels;

  // GAIN members
  double mInputGain, mOutputGain;
  // SAT members
  double mSaturationDrive, mSaturationAmount, mSaturationHarmonics, mSaturationSkew;
  int mSaturationType, mOversamplingRate;
  // EQ members
  double mEqHpFreq, mEqLpFreq,
    mEqLfGain, mEqLfFreq, mEqLfQ,
    mEqLmfGain, mEqLmfFreq, mEqLmfQ,
    mEqHmfGain, mEqHmfFreq, mEqHmfQ,
    mEqHfGain, mEqHfFreq, mEqHfQ,
    mEqAmount;
  int mEqHpOrder;
  bool mEqLfIsBell, mEqHfIsBell;
  // COMP members
  double mCompPeakThresh, mCompPeakRatio, mCompPeakAttack, mCompPeakRelease,
    mCompRmsThresh, mCompRmsRatio, mCompRmsAttack, mCompRmsRelease,
    mCompRmsMakeup, mCompPeakMakeup, 
    mCompPeakRmsRatio, mCompDryWet,
    mCompPeakSidechainFilterFreq,
    mCompPeakKneeWidthDb, mCompRmsKneeWidthDb;
  bool mCompIsParallel, mCompPeakIsExtSc, mCompRmsIsExtSc, mCompPeakIsFeedback, mCompRmsIsFeedback;
  // DEESSER members
  double mDeesserFreq, mDeesserQ, mDeesserThresh, mDeesserRatio, mDeesserAttack, mDeesserRelease, mDeesserMakeup;
  // PAN members
  double mPan, mSafePanFreq, mWidth;
  bool mIsPanMonoLow;
  // CLIPPER members
  double mClipperThreshold;
  // LIMITER members
  double mLimiterThresh;
  // BYPASS members
  bool mInputBypass, mEqBypass, mCompBypass, mOutputBypass, mBypass;
  // AGC Members
  double mAutoGain;
  bool mAgc;

  // Comp
  double mCompRmsAutoMakeup, mCompPeakAutoMakeup;

  // AGC
  bool mAgcTrigger;

  // FILTERS

    // Gain Filters
  SR::DSP::SRGain fInputGain, fOutputGain, fAutoGain, fPan;

  // Spectral Filters
  SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS> fFilters[kNumFilters] = {};

  //// Extra Filters
  //SR::DSP::SRFiltersIIR<sample, MAXNUMOUTCHANNELS> fDcBlocker, fEqHpFilterOnepole, fEqLpFilterOnepole;

  // Dynamic Filters
  SR::DSP::SRCompressor fCompressorPeak;
  SR::DSP::SRCompressorRMS fCompressorRms;
  SR::DSP::SRLimiter fLimiter;
  SR::DSP::SRDeesser fDeesser;

  //SR::DSP::EnvelopeDetector fOutputVuMeterEnvelopeDetector1, fOutputVuMeterEnvelopeDetector2;
  //SR::DSP::AttRelEnvelope fOutputVuMeterEnvelope1, fOutputVuMeterEnvelope2;

  // Saturation
  SR::DSP::SRSaturation fInputSaturation[MAXNUMOUTCHANNELS] = {};
  OverSampler<sample> mOverSampler[MAXNUMOUTCHANNELS]{ OverSampler<sample>::EFactor::kNone };

  SR::Graphics::SRMeter<2, 1024>::SRMeterBallistics mInputMeterBallistics{ cInputMeter };
  SR::Graphics::SRMeter<3, 1024>::SRMeterBallistics mGrMeterBallistics{ cGrMeter };
  SR::Graphics::SRMeter<2, 1024>::SRMeterBallistics mOutputMeterBallistics{ cOutputMeter };
  IVScopeControl<2>::IVScopeBallistics mScopeBallistics{ cScope };

#if USEBUFFER == 1
  sample **mInMeterValues, **mOutMeterValues, **mGrMeterValues;
#elif USEBUFFER == 2
  SR::DSP::SRBuffer<sample, MAXNUMOUTCHANNELS, 1024> bInputMeter;
  SR::DSP::SRBuffer<sample, 3, 1024> bGrMeter;
  SR::DSP::SRBuffer<sample, MAXNUMOUTCHANNELS, 1024> bOutputMeter;
#endif // USEBUFFER



  double* mFreqMeterValues = new double[FREQUENCYRESPONSE];
};

#endif // SRCHANNEL_H