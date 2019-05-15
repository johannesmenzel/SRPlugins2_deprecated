#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include <vector>
#define NUMCONVOSAMPLES 10
const int kNumPrograms = 1;

enum EParams
{
  kSpl0 = 0,
  kSpl1,
  kSpl2,
  kSpl3,
  kSpl4,
  kSpl5,
  kSpl6,
  kSpl7,
  kSpl8,
  kSpl9,
  kNumParams
};

class SRManConvo : public IPlug
{
public:
  SRManConvo(IPlugInstanceInfo instanceInfo);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnParamChange(int paramIdx) override;
  void OnReset() override;


private:
  int mConvoCounter;
  double mConvoMatrix[NUMCONVOSAMPLES];
  double mLastSamples[2][NUMCONVOSAMPLES];

#endif
};
