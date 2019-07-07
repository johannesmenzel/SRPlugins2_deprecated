#include "SRManConvo.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include <cassert>
SRManConvo::SRManConvo(IPlugInstanceInfo instanceInfo)
  : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
  , mConvoMatrix()
  , mConvoCounter(0)
  , mLastSamples()
  , mHannRatio(0.0)
  , mWindowSize(0)
{
  for (int i = 0; i < NUMCONVOSAMPLES; i++) {
    mLastSamples[0][i] = 0.0;
    mLastSamples[1][i] = 0.0;
    mConvoMatrix[i] = 0.0;
  }

  //mLastSamples[0].resize(NUMCONVOSAMPLES);
  //mLastSamples[1].resize(NUMCONVOSAMPLES);
  GetParam(kSpl0)->InitDouble("Spl0", 1., -1., 1., 0.0000001, "");
  for (int paramIdx = 1; paramIdx < NUMCONVOSAMPLES; paramIdx++) {
    GetParam(paramIdx)->InitDouble("n+x", 0., -1., 1., 0.0000001, "");
  }

  GetParam(kHannRatio)->InitDouble("Hann Ratio", 0., 0., 100., 0.0001, "%");
  GetParam(kWindowSize)->InitInt("Window Size", 0, 0, NUMCONVOSAMPLES);

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
    pGraphics->AttachControl(new IVKnobControl(rectSettings.GetGridCell(0, 1, 4), kHannRatio, "Hann Ratio", DEFAULT_STYLE, true));
    pGraphics->AttachControl(new IVKnobControl(rectSettings.GetGridCell(1, 1, 4), kWindowSize, "Window Size", DEFAULT_STYLE, true));
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
      for (int i = 0; i < mWindowSize; i++) {
        const int currentCircularCounter = (NUMCONVOSAMPLES + mConvoCounter - i) % NUMCONVOSAMPLES;
        assert(currentCircularCounter >= 0);
        double currentSample = mConvoMatrix[i];
        if (mHannRatio > 0.0) {
          currentSample *= GetRightHalfOfHannWindow(i, mWindowSize) * mHannRatio + (1. - mHannRatio);
        }
        outputs[c][s] += mLastSamples[c][currentCircularCounter] * currentSample;
      }
      //outputs[c][s] = std::tanh(outputs[c][s]) / std::tanh(1.); // just for the fun.
      //outputs[c][s] /= NUMCONVOSAMPLES;
    }
    mConvoCounter++;
    mConvoCounter %= NUMCONVOSAMPLES;
  }
}
void SRManConvo::OnParamChange(int paramIdx)
{
  if (paramIdx < NUMCONVOSAMPLES) {
    mConvoMatrix[paramIdx] = GetParam(paramIdx)->Value();
  }

  switch (paramIdx)
  {
  case kHannRatio:
    mHannRatio = GetParam(paramIdx)->Value() / 100.;
    break;
  case kWindowSize:
    mWindowSize = GetParam(paramIdx)->Int();
    //for (int i = 0; i < NUMCONVOSAMPLES; i++) {
    //  GetUI()->GetControlWithTag(i)->Hide(i < mWindowSize);
    //}
    // <-- rubbish, because theres only one control for tabs
    break;
  default:
    break;
  }
}
void SRManConvo::OnReset()
{
}
#endif
