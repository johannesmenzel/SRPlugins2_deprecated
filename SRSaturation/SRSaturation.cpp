#include "SRSaturation.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

SRSaturation::SRSaturation(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
, mEnvelope(0.0)
{
  GetParam(kDrive)->InitDouble("Drive", 0., 0., 60., 0.01, "dB");
  GetParam(kOutputGain)->InitDouble("Output Gain", 0., -24., 24., 0.01, "dB");
  GetParam(kSaturation)->InitDouble("Saturation", 0., 0., 100.0, 0.01, "%");

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(0, 1, 3, iplug::igraphics::EDirection::Horizontal), kDrive, GetParam(kDrive)->GetNameForHost()));
    pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(1, 1, 3, iplug::igraphics::EDirection::Horizontal), kSaturation, GetParam(kSaturation)->GetNameForHost()));
    pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(2, 1, 3, iplug::igraphics::EDirection::Horizontal), kOutputGain, GetParam(kOutputGain)->GetNameForHost()));
  };
#endif
}

#if IPLUG_DSP
void SRSaturation::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double drive = DBToAmp(GetParam(kDrive)->Value());
  const double saturation = GetParam(kSaturation)->Value() / 100.;
  const double outputGain = DBToAmp(GetParam(kOutputGain)->Value());
  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s] * drive;
    }
  }
}
#endif
