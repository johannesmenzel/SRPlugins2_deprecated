#include "SRTestBed.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

SRTestBed::SRTestBed(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
, mLastSamplesAverage()
{
  GetParam(kParam)->InitDouble("Param", 0., 0., 100.0, 0.01, "%");

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new IVKnobControl(b, kParam));
  };
#endif
}

#if IPLUG_DSP
void SRTestBed::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double param = GetParam(kParam)->Value() / 100.;
  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      //outputs[c][s] = iplug::Clip(inputs[c][s], -1., 1.);
      //outputs[c][s] = tanh(inputs[c][s]) / tanh(1.);
      outputs[c][s] = inputs[c][s];
      double invSample;
      if (outputs[c][s] > mLastSamplesAverage[c])
        invSample = asin(outputs[c][s]); // if rising
      else
        invSample = asin(-outputs[c][s]) + iplug::PI; // if falling

      mLastSamplesAverage[c] = (inputs[c][s]);

      invSample += param * 2. * iplug::PI;

      outputs[c][s] = sin(invSample);
    }
  }
  
}
#endif
