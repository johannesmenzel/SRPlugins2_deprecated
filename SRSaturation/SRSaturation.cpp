#include "SRSaturation.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "DSP/SRDynamics.h"

#define DCOFF 0.0000000000000001

SRSaturation::SRSaturation(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
, mCurrentEnvelopeState(0.0)
, fLowPass(SR::DSP::SRFilterIIR<sample, 2>::EFilterType::OnepoleLowpass, 100. / 441., 0.717, 0., 44100.)
, fEnvelope(10. ,100., 44100.)
{
  GetParam(kDrive)->InitDouble("Drive", 0., 0., 60., 0.01, "dB");
  GetParam(kSaturation)->InitDouble("Saturation", 0., 0., 100.0, 0.01, "%");
  GetParam(kOutputGain)->InitDouble("Output Gain", 0., -24., 24., 0.01, "dB");

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new IVMeterControl<1>(b.GetGridCell(3, 1, 4, iplug::igraphics::EDirection::Horizontal), "Energy"));
    pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(0, 1, 4, iplug::igraphics::EDirection::Horizontal), kDrive, GetParam(kDrive)->GetNameForHost()));
    pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(1, 1, 4, iplug::igraphics::EDirection::Horizontal), kSaturation, GetParam(kSaturation)->GetNameForHost()));
    pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(2, 1, 4, iplug::igraphics::EDirection::Horizontal), kOutputGain, GetParam(kOutputGain)->GetNameForHost()));
  };
#endif
}

#if IPLUG_DSP
void SRSaturation::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double drive = DBToAmp(GetParam(kDrive)->Value());
  const double saturation = GetParam(kSaturation)->Value() * 0.01;
  const double outputGain = DBToAmp(GetParam(kOutputGain)->Value());
  const int nChans = NOutChansConnected();
  sample **envelope = new sample*[1];
  envelope[0] = new sample[nFrames];

  
  for (int s = 0; s < nFrames; s++) {
    double tempEnvelope = fabs(inputs[0][s] + inputs[1][s]) * 0.5;
    fEnvelope.process(tempEnvelope, mCurrentEnvelopeState);
    envelope[0][s] = tempEnvelope;
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s] + DCOFF;

      // low pass
      outputs[c][s] = fLowPass.Process(outputs[c][s], c);

      // apply pre gain
      outputs[c][s] = outputs[c][s] * drive;


      // saturation stage
      if (outputs[c][s] >= 0.)
        outputs[c][s] = sqrt(fabs(outputs[c][s]));
      else
        outputs[c][s] = -sqrt(fabs(outputs[c][s]));

      outputs[c][s] *= sqrt(mCurrentEnvelopeState + DCOFF);

      // apply reversed post gain
      outputs[c][s] *= outputGain / drive;

      // dry/wet
      outputs[c][s] = outputs[c][s] * saturation + inputs[c][s] * (1. - saturation);

      outputs[c][s] -= DCOFF;
    }
  }
    mEnergyMeter.ProcessBlock(envelope, nFrames);
  delete[] envelope[0];
  delete[] envelope;
}
void SRSaturation::OnReset()
{
  fEnvelope = SR::DSP::SRDynamicsDetector(10., 100., GetSampleRate());
  fLowPass.SetFilter(SR::DSP::SRFilterIIR<sample, 2>::EFilterType::OnepoleLowpass, 10000. / GetSampleRate(), 0.717, 0., GetSampleRate());
}
void SRSaturation::OnIdle()
{
  mEnergyMeter.TransmitData(*this);
}
#endif
