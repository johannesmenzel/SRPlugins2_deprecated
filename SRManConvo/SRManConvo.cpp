#include "SRManConvo.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include <cassert>
SRManConvo::SRManConvo(IPlugInstanceInfo instanceInfo)
  : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
  , mConvoMatrix()
  , mConvoCounter(0)
  , mLastSamples()
{
  for (int i = 0; i < NUMCONVOSAMPLES; i++) {
    mLastSamples[0][i] = 0.0;
    mLastSamples[1][i] = 0.0;
    mConvoMatrix[i] = 0.0;
  }

  //mLastSamples[0].resize(NUMCONVOSAMPLES);
  //mLastSamples[1].resize(NUMCONVOSAMPLES);
  GetParam(kSpl0)->InitDouble("Spl0", 1., -1., 1., 0.0000001, "");
  GetParam(kSpl1)->InitDouble("Spl1", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl2)->InitDouble("Spl2", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl3)->InitDouble("Spl3", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl4)->InitDouble("Spl4", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl5)->InitDouble("Spl5", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl6)->InitDouble("Spl6", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl7)->InitDouble("Spl7", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl8)->InitDouble("Spl8", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl9)->InitDouble("Spl9", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl10)->InitDouble("Spl10", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl11)->InitDouble("Spl11", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl12)->InitDouble("Spl12", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl13)->InitDouble("Spl13", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl14)->InitDouble("Spl14", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl15)->InitDouble("Spl15", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl16)->InitDouble("Spl16", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl17)->InitDouble("Spl17", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl18)->InitDouble("Spl18", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl19)->InitDouble("Spl19", 0., -1., 1., 0.0000001, "");
  GetParam(kIsWindowing)->InitBool("Windowing", 0, "", 0, "", "WND off", "WND on");

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    const IRECT rectSettings = b.GetFromBottom(100.f);
    const IRECT rectControls = b.GetReducedFromBottom(100.f);
    pGraphics->AttachControl(new IVSwitchControl(rectSettings, kIsWindowing, "", DEFAULT_STYLE, true));
    pGraphics->AttachControl(new IVMultiSliderControl<NUMCONVOSAMPLES>(rectControls, "Sample Response", DEFAULT_STYLE, kSpl0, EDirection::Vertical, 0.f, 1.f), kNoTag);
  };
#endif
}

#if IPLUG_DSP
void SRManConvo::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = NOutChansConnected();

  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      mLastSamples[c][mConvoCounter] = inputs[c][s];
      outputs[c][s] = 0.0;
      for (int i = 0; i < NUMCONVOSAMPLES; i++) {
        const int currentCircularCounter = (NUMCONVOSAMPLES + mConvoCounter - i) % NUMCONVOSAMPLES;
        assert(currentCircularCounter >= 0);
        outputs[c][s] += mLastSamples[c][currentCircularCounter] * mConvoMatrix[i] * GetRightHalfOfHannWindow(i, NUMCONVOSAMPLES);
      }
      outputs[c][s] = std::tanh(outputs[c][s]) / std::tanh(1.); // just for the fun.
      //outputs[c][s] /= NUMCONVOSAMPLES;
    }
    mConvoCounter++;
    mConvoCounter %= NUMCONVOSAMPLES;
  }
}
void SRManConvo::OnParamChange(int paramIdx)
{
  if (paramIdx < NUMCONVOSAMPLES - 1) {
    mConvoMatrix[paramIdx] = GetParam(paramIdx)->Value();
  }
  switch (paramIdx)
  {
  case kIsWindowing:
    mIsWindowing = GetParam(paramIdx)->Bool();
    break;
  default:
    break;
  }
}
void SRManConvo::OnReset()
{
}
#endif
