#ifndef SRCHANNEL_H
#define SRCHANNEL_H

// IPlug2 source
#include "IPlug_include_in_plug_hdr.h"
// Extra IPlug2 libs
#include "IVMeterControl.h"
#include "IVScopeControl.h"
#include "IPlugQueue.h"

// SRClasses
#include "../SRClasses/SRConstants.h"
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

// ... Commented out until implementation of oversampling
#include "Oversampler.h"
#include <functional>



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

  enum EFiltersTwoPole {
    kHp1 = 0,
    kHp2,
    kHp3,
    kHp4,
    kHp5,
    kHp6,
    kHp7,
    kHp8,
    kHp9,
    kHp10,
    kLp,
    kLf,
    kLmf,
    kHmf,
    kHf,
    kPanHp,
    kPanLp,
    kNumFiltersTwoPole
  };

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

  //// TESTVARS
  //double sumIn;
  //double sumOut;
  //double aveIn;
  //double aveOut;
  //double diffInOut;

  SR::Graphics::SRMeter<2, 1024>::SRMeterBallistics mInputMeterBallistics{ cInputMeter };
  SR::Graphics::SRMeter<3, 1024>::SRMeterBallistics mGrMeterBallistics{ cGrMeter };
  SR::Graphics::SRMeter<2, 1024>::SRMeterBallistics mOutputMeterBallistics{ cOutputMeter };
  IVScopeControl<2>::IVScopeBallistics mScopeBallistics{ cScope };

  //IPlugQueue<sample> *circularBufferInL, *circularBufferInR, *circularBufferOutL, *circularBufferOutR;
  //WDL_TypedBuf<sample> mCircularBuffer[4];

  //sample circularBufferInL[circularBufferLenght], circularBufferInR[circularBufferLenght], circularBufferOutL[circularBufferLenght], circularBufferOutR[circularBufferLenght];
  //sample* circularBuffer[4] = { circularBufferInL, circularBufferInR, circularBufferOutL, circularBufferOutR };
  //sample** mCircularBuffer = circularBuffer;

  double mFreqMeterValue[FREQUENCYRESPONSE];
  double* mFreqMeterValues = mFreqMeterValue;
};

#endif // SRCHANNEL_H