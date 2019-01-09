#include "SRChannel.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

SRChannel::SRChannel(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  //GetParam(kGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(kUIResizerScale, false);     // Resizer
    pGraphics->AttachPanelBackground(COLOR_GRAY);               // Background
    pGraphics->LoadFont(ROBOTTO_FN);                            // Font
    const IRECT rectPlugin = pGraphics->GetBounds();      // Canvas
    // Control Canvas
    const IRECT rectControls = IRECT(10, 70, PLUG_WIDTH - 10, PLUG_HEIGHT - 10);
    pGraphics->AttachControl(new ITextControl(*this, rectPlugin, "Hello iPlug 2!", IText(50)));
    //pGraphics->AttachControl(new IVKnobControl(*this, rectPluginWindow.GetCentredInside(100).GetVShifted(-100), kGain));
  };
#endif
}

#if IPLUG_DSP
void SRChannel::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s];
    }
  }
}
void SRChannel::OnReset()
{
}
void SRChannel::OnParamChange(int paramIdx)
{
}
void SRChannel::OnIdle()
{
}
#endif
