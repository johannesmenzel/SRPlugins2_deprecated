#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "../SRClasses/SRConstants.h"
#include "../SRClasses/SRGain.h"
#include "../SRClasses/SRFilters.h"
#include "../SRClasses/SRDynamics.h"
#include "../SRClasses/SRSaturation.h"
#include "../SRClasses/SRHelpers.h"
#include "../SRClasses/SRControls.h"
#include "SRChannelConstants.h"
#include "IVMeterControl.h"
#include "IVScopeControl.h"

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
  void InitEffects();
  void GrayOutControls();
  double mSampleRate,
    // GAIN + PAN
    mInputGain, mOutputGain, mInputDrive,
    mPan, mSafePanFreq,
    vSafePanLowSignal1, vSafePanHighSignal1, vSafePanLowSignal2, vSafePanHighSignal2,

    // CLIP + LIMIT
    mClipperThreshold, mSaturationAmount, mSaturationHarmonics, mLimiterThresh, mSaturationSkew,

    //mSatLfGain, mSatLfFreq, mSatMfGain, mSatMfFreq, mSatHfGain, mSatHfFreq, mSatHpFreq, mSatLpFreq,

    // EQ
    mEqHpFreq, mEqLpFreq,
    mEqLfGain, mEqLfFreq, mEqLfQ,
    mEqLmfGain, mEqLmfFreq, mEqLmfQ,
    mEqHmfGain, mEqHmfFreq, mEqHmfQ,
    mEqHfGain, mEqHfFreq, mEqHfQ,

    // COMP
    mCompPeakThresh, mCompPeakRatio, mCompPeakAttack, mCompPeakRelease,
    mCompRmsThresh, mCompRmsRatio, mCompRmsAttack, mCompRmsRelease,
    mCompRmsMakeup, mCompPeakMakeup, mCompRmsAutoMakeup, mCompPeakAutoMakeup,
    mCompPeakRmsRatio, mCompDryWet,
    mCompPeakSidechainFilterFreq,
    mCompPeakKneeWidthDb, mCompRmsKneeWidthDb,

    // DEESSER
    mDeesserFreq, mDeesserQ,
    mDeesserThresh, mDeesserRatio, mDeesserAttack, mDeesserRelease, mDeesserMakeup;

    // TESTPARAM
    //mTestParam1, mTestParam2, mTestParam3, mTestParam4, mTestParam5,

// BOOL VARS
  bool mEqLfIsBell, mEqHfIsBell, mCompIsParallel, mCompPeakIsExtSc, mCompRmsIsExtSc, mCompPeakIsFeedback, mCompRmsIsFeedback, mEqBypass, mCompBypass, mInputBypass, mOutputBypass, mBypass, mIsPanMonoLow,
    mAgc;

  // INT VARS
  int mEqHpOrder, mSaturationType;

    //// METERS
    //cInputPeakMeter1, cInputPeakMeter2, cOutputPeakMeter1, cOutputPeakMeter2,
    //cRmsGrMeter, cPeakGrMeter, cDeesserMeter, /*cFreqRespGraph, */cOutputVuMeter1, cOutputVuMeter2;


  // CIRCULAR BUFFER
  unsigned short int circularBufferPointer;

  // FILTERS
    // Gain Filters
  SRPlugins::SRGain::SRGain fInputGain, fOutputGain;
  SRPlugins::SRGain::SRPan fPan;

  // Spectral Filters
  SRPlugins::SRFilters::SRFiltersTwoPole fEqHpFilter1L, fEqHpFilter2L, fEqHpFilter3L, fEqHpFilter4L, fEqHpFilter5L, fEqHpFilter6L, fEqHpFilter7L, fEqHpFilter8L, fEqHpFilter9L, fEqHpFilter10L,
    fEqLpFilter1L,
    fEqLfFilterL, fEqLmfFilterL, fEqHmfFilterL, fEqHfFilterL,
    fEqHpFilter1R, fEqHpFilter2R, fEqHpFilter3R, fEqHpFilter4R, fEqHpFilter5R, fEqHpFilter6R, fEqHpFilter7R, fEqHpFilter8R, fEqHpFilter9R, fEqHpFilter10R,
    fEqLpFilter1R,
    fEqLfFilterR, fEqLmfFilterR, fEqHmfFilterR, fEqHfFilterR,
    // Safe-Pan
    fSafePanHpL, fSafePanLpL, fSafePanHpR, fSafePanLpR,

    // Deesser
    fDeesserSidechainBandpassFilterL, fDeesserSidechainBandpassFilterR, fDeesserReductionPeakFilterL, fDeesserReductionPeakFilterR;

  SRPlugins::SRFilters::SRFiltersOnePole fDcBlockerL, fDcBlockerR, fEqHpFilterOnepoleL, fEqHpFilterOnepoleR, fEqLpFilterOnepoleL, fEqLpFilterOnepoleR;

  // Dynamic Filters
  SRPlugins::SRDynamics::SRCompressor fCompressorPeak;
  SRPlugins::SRDynamics::SRCompressorRMS fCompressorRms;
  SRPlugins::SRDynamics::SRLimiter fLimiter;
  SRPlugins::SRDynamics::SRDeesser fDeesser;

  SRPlugins::SRDynamics::EnvelopeDetector fOutputVuMeterEnvelopeDetector1, fOutputVuMeterEnvelopeDetector2;
  SRPlugins::SRDynamics::AttRelEnvelope fOutputVuMeterEnvelope1, fOutputVuMeterEnvelope2;

  // Saturation
  SRPlugins::SRSaturation::SRSaturation fInputSaturation;

  // TESTVARS
  double sumIn;
  double sumOut;
  double aveIn;
  double aveOut;
  double diffInOut;

  IVMeterControl<2>::IVMeterBallistics mInputMeterBallistics{ cInputMeter };
  //IVMeterControl<3>::IVMeterBallistics mGrMeterBallistics{ cGrMeter };
  IVMeterControl<2>::IVMeterBallistics mOutputMeterBallistics{ cOutputMeter };
  IVScopeControl<2>::IVScopeBallistics mScopeBallistics{ cScope };
};
