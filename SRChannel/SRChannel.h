#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPrograms = 1;

enum EParams
{
  kInputGain = 0,
  kEqHpFreq,
  kEqLpFreq,
  kEqHfGain,
  kEqHfFreq,
  kEqHfQ,
  kEqHfBell,
  kEqHmfGain,
  kEqHmfFreq,
  kEqHmfQ,
  kEqLmfGain,
  kEqLmfFreq,
  kEqLmfQ,
  kEqLfGain,
  kEqLfFreq,
  kEqLfQ,
  kEqLfBell,
  kCompRmsThresh,
  kCompRmsRatio,
  kCompRmsAttack,
  kCompRmsRelease,
  kCompRmsMakeup,
  kCompPeakThresh,
  kCompPeakRatio,
  kCompPeakAttack,
  kCompPeakRelease,
  kCompPeakMakeup,
  kCompPeakRmsRatio,
  kCompDryWet,
  kSaturationAmount,
  kClipperThreshold,
  kOutputGain,
  kPan,
  kPanFreq,
  kLimiterThresh,
  kCompIsParallel,
  kEqBypass,
  kCompBypass,
  kOutputBypass,
  kBypass,
  kSaturationHarmonics,
  kEqHpOrder,
  kEqLpOrder,
  kTestParam1,
  kTestParam2,
  kTestParam3,
  kTestParam4,
  kTestParam5,
  kInputDrive,
  kAgc,
  kCompPeakSidechainFilterFreq,
  kDeesserFreq,
  kDeesserQ,
  kDeesserThresh,
  kDeesserRatio,
  kDeesserAttack,
  kDeesserRelease,
  kDeesserMakeup,
  kCompPeakKneeWidthDb,
  kCompRmsKneeWidthDb,
  kInputBypass,
  kCompPeakIsExtSc,
  kCompRmsIsExrSc,
  kSaturationSkew,
  kIsPanMonoLow,
  kSaturationType,
  kCompPeakIsFeedback,
  kCompRmsIsFeedback,
  kNumParams
};

enum ECtrlTags {
  cInputGain = 0,
  cEqHpFreq,
  cEqLpFreq,
  cEqHfGain,
  cEqHfFreq,
  cEqHfQ,
  cEqHfBell,
  cEqHmfGain,
  cEqHmfFreq,
  cEqHmfQ,
  cEqLmfGain,
  cEqLmfFreq,
  cEqLmfQ,
  cEqLfGain,
  cEqLfFreq,
  cEqLfQ,
  cEqLfBell,
  cCompRmsThresh,
  cCompRmsRatio,
  cCompRmsAttack,
  cCompRmsRelease,
  cCompRmsMakeup,
  cCompPeakThresh,
  cCompPeakRatio,
  cCompPeakAttack,
  cCompPeakRelease,
  cCompPeakMakeup,
  cCompPeakRmsRatio,
  cCompDryWet,
  cSaturationAmount,
  cClipperThreshold,
  cOutputGain,
  cPan,
  cPanFreq,
  cLimiterThresh,
  cCompIsParallel,
  cEqBypass,
  cCompBypass,
  cOutputBypass,
  cBypass,
  cSaturationHarmonics,
  cEqHpOrder,
  cEqLpOrder,
  cTestParam1,
  cTestParam2,
  cTestParam3,
  cTestParam4,
  cTestParam5,
  cInputDrive,
  cAgc,
  cCompPeakSidechainFilterFreq,
  cDeesserFreq,
  cDeesserQ,
  cDeesserThresh,
  cDeesserRatio,
  cDeesserAttack,
  cDeesserRelease,
  cDeesserMakeup,
  cCompPeakKneeWidthDb,
  cCompRmsKneeWidthDb,
  cInputBypass,
  cCompPeakIsExtSc,
  cCompRmsIsExrSc,
  cSaturationSkew,
  cIsPanMonoLow,
  cSaturationType,
  cCompPeakIsFeedback,
  cCompRmsIsFeedback,
  cNumParams
};

class SRChannel : public IPlug
{
public:
  SRChannel(IPlugInstanceInfo instanceInfo);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  void OnIdle();
#endif
private:

};
