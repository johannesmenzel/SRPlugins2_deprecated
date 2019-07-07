#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include <vector>
#define NUMCONVOSAMPLES 100
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
  kSpl20,
  kSpl21,
  kSpl22,
  kSpl23,
  kSpl24,
  kSpl25,
  kSpl26,
  kSpl27,
  kSpl28,
  kSpl29,
  kSpl30,
  kSpl31,
  kSpl32,
  kSpl33,
  kSpl34,
  kSpl35,
  kSpl36,
  kSpl37,
  kSpl38,
  kSpl39,
  kSpl40,
  kSpl41,
  kSpl42,
  kSpl43,
  kSpl44,
  kSpl45,
  kSpl46,
  kSpl47,
  kSpl48,
  kSpl49,
  kSpl50,
  kSpl51,
  kSpl52,
  kSpl53,
  kSpl54,
  kSpl55,
  kSpl56,
  kSpl57,
  kSpl58,
  kSpl59,
  kSpl60,
  kSpl61,
  kSpl62,
  kSpl63,
  kSpl64,
  kSpl65,
  kSpl66,
  kSpl67,
  kSpl68,
  kSpl69,
  kSpl70,
  kSpl71,
  kSpl72,
  kSpl73,
  kSpl74,
  kSpl75,
  kSpl76,
  kSpl77,
  kSpl78,
  kSpl79,
  kSpl80,
  kSpl81,
  kSpl82,
  kSpl83,
  kSpl84,
  kSpl85,
  kSpl86,
  kSpl87,
  kSpl88,
  kSpl89,
  kSpl90,
  kSpl91,
  kSpl92,
  kSpl93,
  kSpl94,
  kSpl95,
  kSpl96,
  kSpl97,
  kSpl98,
  kSpl99,
  kHannRatio,
  kWindowSize,
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
    return 0.5 * (1. + std::cos((2. * PI * n) / (2. * windowWidth - 1.)));
  }


private:
  int mConvoCounter;
  double mConvoMatrix[NUMCONVOSAMPLES];
  double mLastSamples[2][NUMCONVOSAMPLES];
  double mHannRatio;
  int mWindowSize;
  //std::vector<double> mLastSamples[2];

#endif
};
