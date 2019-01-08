#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "../SRClasses/SRDynamics.h"
#include "../SRClasses/SRSaturation.h"
#include "IVMeterControl.h"


const int kNumPrograms = 1;

enum EParams
{
  kOutputGain = 0,
  kCompression,
  kNumParams
};

enum ECtrlTags {
  cTitle = 0,
  cControlCanvas,
  cOutputGain,
  cCompression,
  cMeter,
  cNumCtrlTags
};

class SRCompressor : public IPlug
{
public:
  SRCompressor(IPlugInstanceInfo instanceInfo);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  void OnIdle() override;
#endif

private:
  //double mCompRatio, mCompThresh, mCompAttack, mCompRelease;
  double mOutputGain, mCompression;

  sample sampleMeterValues;
  
  SRPlugins::SRDynamics::SRCompressor fCompressor;
  SRPlugins::SRSaturation::SRSaturation fInputSaturation, fOutputSaturation;

  IVMeterControl<2>::IVMeterBallistics mMeterBallistics{ cMeter };

};
