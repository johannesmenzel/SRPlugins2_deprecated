#include "SRCompressor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

SRCompressor::SRCompressor(IPlugInstanceInfo instanceInfo)
  : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  //GetParam(kOutputGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
  GetParam(kOutputGain)->InitDouble("Output Gain", 0., -20., 20., 0.01, "dB", IParam::kFlagStepped, "Gain", new IParam::ShapeLinear, IParam::EParamUnit::kUnitDB);
  GetParam(kCompression)->InitDouble("Compression", 0., 0., 10., 0.01, "", IParam::kFlagStepped, "Compression", new IParam::ShapeLinear, IParam::EParamUnit::kUnitCustom);

  fCompressor.initCompressor(0., 1. / 4., 7., 80., 100., 10., false, GetSampleRate());
  fCompressor.initRuntime();
  fInputSaturation.setSaturation(SRPlugins::SRSaturation::typePirkleModified, 10., 0.25, 0.5, false, 0., 1.);
  fOutputSaturation.setSaturation(SRPlugins::SRSaturation::typeZoelzer, 10., 0.25, 0.5, false, 0., 1.);


#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(kUIResizerScale, false);
    pGraphics->AttachPanelBackground(COLOR_DARK_GRAY);
    pGraphics->HandleMouseOver(true);
    pGraphics->EnableTooltips(true);
    pGraphics->LoadFont(ROBOTTO_FN);
    IText tTitle = IText(50, COLOR_WHITE, DEFAULT_FONT, IText::kStyleItalic, IText::kAlignFar, IText::kVAlignMiddle, 0, IText::kQualityNonAntiAliased, DEFAULT_TEXTENTRY_BGCOLOR);
    IText tKnobLabel = IText(DEFAULT_TEXT_SIZE + 5, COLOR_WHITE, DEFAULT_FONT, IText::kStyleBold, IText::kAlignCenter, IText::kVAlignTop, 0, IText::kQualityNonAntiAliased);
    IText tKnobValue = IText(DEFAULT_TEXT_SIZE + 5, COLOR_WHITE, DEFAULT_FONT, IText::kStyleBold, IText::kAlignCenter, IText::kVAlignBottom, 0, IText::kQualityNonAntiAliased);
    IVColorSpec colorspecKnobs = IVColorSpec();
    colorspecKnobs.mFRColor = COLOR_BLACK;
    const IBitmap bmpSslBlue64 = pGraphics->LoadBitmap(KNOB_SSL_BLUE_64, 128, false, 0);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new ITextControl(*this, b.GetFromTRHC(PLUG_WIDTH, 70), "SRPlugins Compressor", tTitle), cTitle, "GUI");
    const IRECT rectControlCanvas = IRECT(10.f, 80.f, PLUG_WIDTH - 10.f, PLUG_HEIGHT - 10.f);
    pGraphics->AttachControl(new IPanelControl(*this, rectControlCanvas, COLOR_GRAY, true), cControlCanvas, "GUI");
    pGraphics->AttachControl(new IVKnobControl(*this, rectControlCanvas.GetGridCell(1, 4, 3, 6), kOutputGain, GetParam(kOutputGain)->GetNameForHost(), true, colorspecKnobs, tKnobLabel, tKnobValue, -150.f, 150.f, 0.5f, kVertical, 5.), cOutputGain, "Knobs");
    //pGraphics->AttachControl(new IVKnobControl(*this,  kCompression, GetParam(kCompression)->GetNameForHost(), true), cCompression, "Knobs");
    pGraphics->AttachControl(new IBKnobControl(*this, rectControlCanvas.GetGridCell(1, 1, 3, 6), bmpSslBlue64, kCompression, kVertical, 5.));
    pGraphics->AttachControl(new IVMeterControl<2>(*this, rectControlCanvas.GetGridCell(1, 1, 3, 3), "In L", "In R"/*, "GR L", "GR R", "Out L", "Out R"*/), cMeter, "Meters");
  };
#endif
}

#if IPLUG_DSP
void SRCompressor::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  //const double mOutputGain = DBToAmp(GetParam(kOutputGain)->Value());
  const int nChans = NOutChansConnected();

  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s];
      fInputSaturation.process(outputs[0][s], outputs[1][s]);
      outputs[c][s] = outputs[c][s] * mCompression;
      fCompressor.process(outputs[0][s], outputs[1][s]);
      outputs[c][s] *= 1. / mCompression;
      outputs[c][s] *= mOutputGain;
      fOutputSaturation.process(outputs[0][s], outputs[1][s]);
    }
  }
  mMeterBallistics.ProcessBlock(outputs, nFrames);
}

void SRCompressor::OnIdle()
{
  mMeterBallistics.TransmitData(*this);
}

void SRCompressor::OnReset()
{
}
void SRCompressor::OnParamChange(int paramIdx)
{
  switch (paramIdx)
  {
  case kOutputGain:
    mOutputGain = DBToAmp(GetParam(paramIdx)->Value()); break;
  case kCompression:
    mCompression = DBToAmp(GetParam(paramIdx)->Value() * 2.); break;

  default:
    break;
  }
}

#endif
