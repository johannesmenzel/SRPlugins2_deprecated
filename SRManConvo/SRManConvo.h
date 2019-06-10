#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include <vector>
#define NUMCONVOSAMPLES 20
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
  kSpl10,
  kSpl11,
  kSpl12,
  kSpl13,
  kSpl14,
  kSpl15,
  kSpl16,
  kSpl17,
  kSpl18,
  kSpl19,
  kIsWindowing,
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

  double GetRightHalfOfHannWindow(int n, int windowWidth) {
    return 0.5 * (1 + std::cos((2. * PI * n) / (2 * windowWidth - 1)));
  }


private:
  int mConvoCounter;
  double mConvoMatrix[NUMCONVOSAMPLES];
  double mLastSamples[2][NUMCONVOSAMPLES];
  bool mIsWindowing;
  //std::vector<double> mLastSamples[2];

#endif
};
