#include "SRManConvo.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

SRManConvo::SRManConvo(IPlugInstanceInfo instanceInfo)
  : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
  , mConvoMatrix()
  , mConvoCounter(0)
  , mLastSamples()
{
  GetParam(kSpl0)->InitDouble("Spl0", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl1)->InitDouble("Spl1", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl2)->InitDouble("Spl2", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl3)->InitDouble("Spl3", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl4)->InitDouble("Spl4", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl5)->InitDouble("Spl5", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl6)->InitDouble("Spl6", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl7)->InitDouble("Spl7", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl8)->InitDouble("Spl8", 0., -1., 1., 0.0000001, "");
  GetParam(kSpl9)->InitDouble("Spl9", 0., -1., 1., 0.0000001, "");

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(kUIResizerScale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new IVSliderControl(b.GetGridCell(0, 0, 1, 10), kSpl0));
    pGraphics->AttachControl(new IVSliderControl(b.GetGridCell(0, 1, 1, 10), kSpl1));
    pGraphics->AttachControl(new IVSliderControl(b.GetGridCell(0, 2, 1, 10), kSpl2));
    pGraphics->AttachControl(new IVSliderControl(b.GetGridCell(0, 3, 1, 10), kSpl3));
    pGraphics->AttachControl(new IVSliderControl(b.GetGridCell(0, 4, 1, 10), kSpl4));
    pGraphics->AttachControl(new IVSliderControl(b.GetGridCell(0, 5, 1, 10), kSpl5));
    pGraphics->AttachControl(new IVSliderControl(b.GetGridCell(0, 6, 1, 10), kSpl6));
    pGraphics->AttachControl(new IVSliderControl(b.GetGridCell(0, 7, 1, 10), kSpl7));
    pGraphics->AttachControl(new IVSliderControl(b.GetGridCell(0, 8, 1, 10), kSpl8));
    pGraphics->AttachControl(new IVSliderControl(b.GetGridCell(0, 9, 1, 10), kSpl9));
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
      outputs[c][s] = inputs[c][s];
      for (int i = 0; i < NUMCONVOSAMPLES; i++) {
        const int currentCounter = (NUMCONVOSAMPLES + mConvoCounter - i) % NUMCONVOSAMPLES;
        outputs[c][s] += mLastSamples[c][currentCounter] * mConvoMatrix[currentCounter];
      }
      //outputs[c][s] *= 1. / NUMCONVOSAMPLES;
    }
    mConvoCounter++;
    mConvoCounter %= NUMCONVOSAMPLES;
  }
}
void SRManConvo::OnParamChange(int paramIdx)
{
  if (paramIdx < 10) {
    mConvoMatrix[paramIdx] = GetParam(paramIdx)->Value();
  }
}
void SRManConvo::OnReset()
{
}
#endif
