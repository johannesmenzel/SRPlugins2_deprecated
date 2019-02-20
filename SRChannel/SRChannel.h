#ifndef SRCHANNEL_H
#define SRCHANNEL_H

/*
Which current test buffer implementation:
1 - ordinary sample** buffer
2 - SRBuffer2 class (T**)
3 - SRBuffer class (Ptrlist of WDL_Typedbuf)
*/
#define USEBUFFER 3

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
  //double circularBufferInL[circularBufferLenght], circularBufferInR[circularBufferLenght], circularBufferOutL[circularBufferLenght], circularBufferOutR[circularBufferLenght];
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
    mCompRmsMakeup, mCompPeakMakeup, mCompRmsAutoMakeup, mCompPeakAutoMakeup,
    mCompPeakRmsRatio, mCompDryWet,
    mCompPeakSidechainFilterFreq,
    mCompPeakKneeWidthDb, mCompRmsKneeWidthDb;
  bool mCompIsParallel, mCompPeakIsExtSc, mCompRmsIsExtSc, mCompPeakIsFeedback, mCompRmsIsFeedback;
  // DEESSER members
  double mDeesserFreq, mDeesserQ, mDeesserThresh, mDeesserRatio, mDeesserAttack, mDeesserRelease, mDeesserMakeup;
  // TESTPARAM
  // double mTestParam1, mTestParam2, mTestParam3, mTestParam4, mTestParam5;
  // PAN members
  double mPan, mSafePanFreq;
  bool mIsPanMonoLow;
  // CLIPPER members
  double mClipperThreshold;
  // LIMITER members
  double mLimiterThresh;
  // BYPASS members
  bool mInputBypass, mEqBypass, mCompBypass, mOutputBypass, mBypass;
  // AGC Members
  double mAutoGain;
  bool mAgc, mAgcTrigger;

  // CIRCULAR BUFFER
  unsigned short int circularBufferPointer;

  // FILTERS

    // Gain Filters
  SR::DSP::SRGain fInputGain, fOutputGain, fAutoGain;
  SR::DSP::SRPan fPan;

  // Spectral Filters
  SR::DSP::SRFiltersTwoPole fFilterTwoPole[kNumFiltersTwoPole][2];

  // Extra Filters
  SR::DSP::SRFiltersOnePole fDcBlocker[2], fEqHpFilterOnepole[2], fEqLpFilterOnepole[2];

  // Dynamic Filters
  SR::DSP::SRCompressor fCompressorPeak;
  SR::DSP::SRCompressorRMS fCompressorRms;
  SR::DSP::SRLimiter fLimiter;
  SR::DSP::SRDeesser fDeesser;

  //SR::DSP::EnvelopeDetector fOutputVuMeterEnvelopeDetector1, fOutputVuMeterEnvelopeDetector2;
  //SR::DSP::AttRelEnvelope fOutputVuMeterEnvelope1, fOutputVuMeterEnvelope2;

  // Saturation
  SR::DSP::SRSaturation fInputSaturation[2];
  OverSampler<sample> mOverSampler[2]{ OverSampler<sample>::EFactor::kNone };

  SR::Graphics::SRMeter<2, 1024>::SRMeterBallistics mInputMeterBallistics{ cInputMeter };
  SR::Graphics::SRMeter<3, 1024>::SRMeterBallistics mGrMeterBallistics{ cGrMeter };
  SR::Graphics::SRMeter<2, 1024>::SRMeterBallistics mOutputMeterBallistics{ cOutputMeter };
  IVScopeControl<2>::IVScopeBallistics mScopeBallistics{ cScope };

#if USEBUFFER == 1
  sample **mInMeterValues, **mOutMeterValues, **mGrMeterValues;
#elif USEBUFFER == 2
  SR::DSP::SRBuffer2<sample, 2> bInputMeter;
  SR::DSP::SRBuffer2<sample, 2> bOutputMeter;
  SR::DSP::SRBuffer2<sample, 3> bGrMeter;
#elif USEBUFFER == 3
  SR::DSP::SRBuffer<sample, 2, 1024> bInputMeter;
  SR::DSP::SRBuffer<sample, 3, 1024> bGrMeter;
  SR::DSP::SRBuffer<sample, 2, 1024> bOutputMeter;
#endif // USEBUFFER



  double* mFreqMeterValues = new double[FREQUENCYRESPONSE];
};

#endif // SRCHANNEL_H