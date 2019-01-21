#pragma once

// IPlug2 source
#include "IPlug_include_in_plug_hdr.h"
// Extra IPlug2 libs
#include "IVMeterControl.h"
#include "IVScopeControl.h"
// SRClasses
#include "../SRClasses/SRConstants.h"
#include "../SRClasses/SRGain.h"
#include "../SRClasses/SRFilters.h"
#include "../SRClasses/SRDynamics.h"
#include "../SRClasses/SRSaturation.h"
#include "../SRClasses/SRHelpers.h"
#include "../SRClasses/SRControls.h"
// Plugin constants
#include "SRChannelConstants.h"
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
  void OnIdle();
  double circularBufferInL[circularBufferLenght], circularBufferInR[circularBufferLenght], circularBufferOutL[circularBufferLenght], circularBufferOutR[circularBufferLenght];
#endif

private:
  // User Functions
  void InitEffects();
  void GrayOutControls();

  // GLOBAL members
  double mSampleRate;
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
  bool mAgc;

  // CIRCULAR BUFFER
  unsigned short int circularBufferPointer;

  // FILTERS
    // Gain Filters
  SRPlugins::SRGain::SRGain fInputGain, fOutputGain;
  SRPlugins::SRGain::SRPan fPan;

  // Spectral Filters
  SRPlugins::SRFilters::SRFiltersTwoPole fEqHpFilter1[2], fEqHpFilter2[2], fEqHpFilter3[2], fEqHpFilter4[2], fEqHpFilter5[2], fEqHpFilter6[2], fEqHpFilter7[2], fEqHpFilter8[2], fEqHpFilter9[2], fEqHpFilter10[2],
    fEqLpFilter1[2],
    fEqLfFilter[2], fEqLmfFilter[2], fEqHmfFilter[2], fEqHfFilter[2];
  // Safe-Pan
  SRPlugins::SRFilters::SRFiltersTwoPole fSafePanHp[2], fSafePanLp[2];
  // Deesser
  SRPlugins::SRFilters::SRFiltersTwoPole fDeesserSidechainBandpassFilter[2], fDeesserReductionPeakFilter[2];
  // Extra Filters
  SRPlugins::SRFilters::SRFiltersOnePole fDcBlocker[2], fEqHpFilterOnepole[2], fEqLpFilterOnepole[2];

  // Dynamic Filters
  SRPlugins::SRDynamics::SRCompressor fCompressorPeak;
  SRPlugins::SRDynamics::SRCompressorRMS fCompressorRms;
  SRPlugins::SRDynamics::SRLimiter fLimiter;
  SRPlugins::SRDynamics::SRDeesser fDeesser;

  //SRPlugins::SRDynamics::EnvelopeDetector fOutputVuMeterEnvelopeDetector1, fOutputVuMeterEnvelopeDetector2;
  //SRPlugins::SRDynamics::AttRelEnvelope fOutputVuMeterEnvelope1, fOutputVuMeterEnvelope2;

  // Saturation
  SRPlugins::SRSaturation::SRSaturation fInputSaturation[2];
  OverSampler<sample> mOverSampler[2]{ OverSampler<sample>::kNone };


  // TESTVARS
  double sumIn;
  double sumOut;
  double aveIn;
  double aveOut;
  double diffInOut;

  IVMeterControl<2>::IVMeterBallistics mInputMeterBallistics{ cInputMeter };
  SRPlugins::SRControls::SRMeter<3>::SRMeterBallistics mGrMeterBallistics{ cGrMeter };
  IVMeterControl<2>::IVMeterBallistics mOutputMeterBallistics{ cOutputMeter };
  IVScopeControl<2>::IVScopeBallistics mScopeBallistics{ cScope };
};
