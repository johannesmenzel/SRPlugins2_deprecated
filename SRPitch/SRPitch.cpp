#include "SRPitch.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

SRPitch::SRPitch(const InstanceInfo& info)
  : Plugin(info, MakeConfig(kNumParams, kNumPrograms))
  , mWet(0.)
  , mHpFreq(200.)
  , mLpFreq(5000.)
{
  GetParam(kWet)->InitDouble("Dry/Wet", 0., 0., 100.0, 0.01, "%");
  GetParam(kHp)->InitFrequency("HP", 200., 10., 2000., 0.01);
  GetParam(kLp)->InitFrequency("LP", 5000., 100., 20000., 0.01);

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    const IRECT rectHeader = pGraphics->GetBounds().GetFromTop(100.);
    const IRECT rectMain = pGraphics->GetBounds().GetFromBottom(300.);
    pGraphics->AttachControl(new ITextControl(rectHeader, "SRPitch", IText(50)));
    pGraphics->AttachControl(new IVKnobControl(rectMain.GetGridCell(0, 1, 3, 3), kWet));
    pGraphics->AttachControl(new IVKnobControl(rectMain.GetGridCell(0, 0, 3, 3), kHp));
    pGraphics->AttachControl(new IVKnobControl(rectMain.GetGridCell(0, 2, 3, 3), kLp));
  };


#endif

  OnReset();
}

#if IPLUG_DSP
void SRPitch::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = NOutChansConnected();

  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      //if (s % 2 == 0) // even
      //  outputs[c][s / 2] = inputs[c][s];
      //else // odd
      //  outputs[c][nFrames / 2 + (int)(s / 2)] = inputs[c][s];

      outputs[c][s] = fabs(inputs[c][s]);
      fDCBlocker.Process(outputs[c][s], c);
      fLowpass.Process(outputs[c][s], c);
      outputs[c][s] = inputs[c][s] * (1. - mWet) + outputs[c][s] * mWet;

    }
  }
}
void SRPitch::OnReset()
{
  mSampleRate = GetSampleRate();
  fDCBlocker = SR::DSP::SRFilterIIR<sample, 2>(SR::DSP::SRFilterIIR<sample, 2>::EFilterType::OnepoleLowpass, 200. / GetSampleRate(), 0.707, 0., GetSampleRate());
  fLowpass = SR::DSP::SRFilterIIR<sample, 2>(SR::DSP::SRFilterIIR<sample, 2>::EFilterType::OnepoleLowpass, 5000./GetSampleRate(), 0.707, 0., GetSampleRate());
}
void SRPitch::OnParamChange(int paramIdx)
{
  switch (paramIdx)
  {
  case kWet:
    mWet = GetParam(paramIdx)->Value() / 100.;
    break;
  case kLp:
    mLpFreq = GetParam(paramIdx)->Value();
    fLowpass.SetFreq(mLpFreq / mSampleRate);
    break;
  case kHp:
    mHpFreq = GetParam(paramIdx)->Value();
    fDCBlocker.SetFreq(mHpFreq / mSampleRate);
    break;
  default:
    break;
  }
}
#endif
