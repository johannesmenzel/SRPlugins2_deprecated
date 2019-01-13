#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "../SRClasses/SRConstants.h"
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
#endif

private:
  IVMeterControl<2>::IVMeterBallistics mInputMeterBallistics{ cInputMeter };
  //IVMeterControl<3>::IVMeterBallistics mGrMeterBallistics{ cGrMeter };
  IVMeterControl<2>::IVMeterBallistics mOutputMeterBallistics{ cOutputMeter };
  IVScopeControl<2>::IVScopeBallistics mScopeBallistics{ cScope };
};
