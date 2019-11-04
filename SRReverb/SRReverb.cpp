#include "SRReverb.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "SRConstants.h"

#ifndef WDL_SIMPLEPITCHSHIFT_IMPLEMENT
#define WDL_SIMPLEPITCHSHIFT_IMPLEMENT
#endif // !WDL_SIMPLEPITCHSHIFT_IMPLEMENT


SRReverb::SRReverb(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
  , mWidth(1.)
  , mDamp(.5)
  , mSize(.5)
  , mWet(1.)
  , mDry(0.)
  , mHighpass(10.)
  , mLowpass(22000.)
  , mCompress(0.0)
  , mCompThresh(0.0)
  , mCompRatio(0.25)
  , mCompAttack(20.)
  , mCompRelease(500.)
  , mPitchShift(0.)
  , mPitchTempo(0.)
{
  GetParam(kDry)->InitDouble("Dry", 0., -100., 24., 0.01, "dB");
  GetParam(kWet)->InitDouble("Wet", -100., -100., 24., 0.01, "dB");
  GetParam(kWidth)->InitDouble("Width", 100., 0., 400., 0.01, "%");
  GetParam(kDamp)->InitDouble("Damp", 50., 0., 100., 0.01, "%");
  GetParam(kSize)->InitDouble("Size", 50., 0., 100., 0.01, "%");
  GetParam(kHighpass)->InitDouble("Highpass", 10., 10., 5000., 1., "Hz");
  GetParam(kLowpass)->InitDouble("Lowpass", 22000., 1000., 22000., 1., "Hz");
  GetParam(kCompress)->InitDouble("Compress", 0.0, 0.0, 100.0, 0.01, "%");
  GetParam(kPitchShift)->InitDouble("P-Shift", 0.0, -1.0, 1.0, 0.01, "?");
  GetParam(kPitchTempo)->InitDouble("P-Tempo", 0.0, -1.0, 1.0, 0.01, "?");


#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    const IRECT rectReverb = IRECT(b.GetGridCell(0, 0, 2, 2).GetPadded(-5.f));
    const IRECT rectFilter = IRECT(b.GetGridCell(0, 1, 2, 2).GetPadded(-5.f));
    const IRECT rectPitch = IRECT(b.GetGridCell(1, 0, 2, 2).GetPadded(-5.f));
    const IRECT rectOutput = IRECT(b.GetGridCell(1, 1, 2, 2).GetPadded(-5.f));

    pGraphics->AttachControl(new IPanelControl(rectReverb, IColor(100, 20, 40, 70)));
    pGraphics->AttachControl(new IPanelControl(rectFilter, IColor(100, 20, 40, 70)));
    pGraphics->AttachControl(new IPanelControl(rectPitch, IColor(100, 20, 40, 70)));
    pGraphics->AttachControl(new IPanelControl(rectOutput, IColor(100, 20, 40, 70)));

    pGraphics->AttachControl(new IVKnobControl(rectReverb.GetGridCell(0, 0, 1, 3).GetCentredInside(100.f), kSize, "Size", DEFAULT_STYLE, true, false));
    pGraphics->AttachControl(new IVKnobControl(rectReverb.GetGridCell(0, 1, 1, 3).GetCentredInside(100.f), kDamp, "Damp", DEFAULT_STYLE, true, false));
    pGraphics->AttachControl(new IVKnobControl(rectReverb.GetGridCell(0, 2, 1, 3).GetCentredInside(100.f), kWidth, "Width", DEFAULT_STYLE, true, false));
    pGraphics->AttachControl(new IVKnobControl(rectFilter.GetGridCell(0, 0, 1, 3).GetCentredInside(100.f), kHighpass, "HP", DEFAULT_STYLE, true, false));
    pGraphics->AttachControl(new IVKnobControl(rectFilter.GetGridCell(0, 1, 1, 3).GetCentredInside(100.f), kLowpass, "LP", DEFAULT_STYLE, true, false));

    pGraphics->AttachControl(new IVKnobControl(rectPitch.GetGridCell(0, 0, 1, 3).GetCentredInside(100.f), kPitchShift, "P-Shift", DEFAULT_STYLE, true, false));
    pGraphics->AttachControl(new IVKnobControl(rectPitch.GetGridCell(0, 1, 1, 3).GetCentredInside(100.f), kPitchTempo, "P-Tempo", DEFAULT_STYLE, true, false));


    pGraphics->AttachControl(new IVKnobControl(rectOutput.GetGridCell(0, 0, 1, 3).GetCentredInside(100.f), kDry, "Dry", DEFAULT_STYLE, true, false));
    pGraphics->AttachControl(new IVKnobControl(rectOutput.GetGridCell(0, 1, 1, 3).GetCentredInside(100.f), kWet, "Wet", DEFAULT_STYLE, true, false));
    pGraphics->AttachControl(new IVKnobControl(rectOutput.GetGridCell(0, 2, 1, 3).GetCentredInside(100.f), kCompress, "Compress", DEFAULT_STYLE, true, false));
  };
#endif
}

#if IPLUG_DSP
void SRReverb::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = NOutChansConnected();

  fReverb.ProcessSampleBlock(inputs[0], inputs[1], outputs[0], outputs[1], nFrames);

  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = fHighpass.Process(outputs[c][s], c);
      outputs[c][s] = fLowpass.Process(outputs[c][s], c);
    }

    fCompressor.Process(outputs[0][s], outputs[1][s]);

    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = mWet * outputs[c][s] + mDry * inputs[c][s];
    }
  }
}

void SRReverb::OnReset()
{
  fReverb.SetSampleRate(GetSampleRate());
  fReverb.Reset();

  fPitch.Reset();
  fPitch.set_srate(GetSampleRate());
  fPitch.set_nch(2);

  fHighpass.SetFilter(SR::DSP::SRFilterIIR<sample, 2>::EFilterType::BiquadHighpass, mHighpass / GetSampleRate(), sqrt(0.5), 0.0, GetSampleRate());
  fLowpass.SetFilter(SR::DSP::SRFilterIIR<sample, 2>::EFilterType::BiquadLowpass, mLowpass / GetSampleRate(), sqrt(0.5), 0.0, GetSampleRate());

  fCompressor.InitCompressor(mCompThresh, mCompRatio, mCompAttack, mCompRelease, 50., 10., 10., true, false, GetSampleRate());
}

void SRReverb::OnParamChange(int paramIdx)
{
  IParam* p = GetParam(paramIdx);

  switch (paramIdx)
  {
  case kDry:
    mDry = DBToAmp(p->Value());
    break;
  case kWet:
    mWet = DBToAmp(p->Value());
    break;
  case kDamp:
    mDamp = p->Value() * 0.01;
    fReverb.SetDampening(mDamp);
    fReverb.Reset();
    break;
  case kWidth:
    mWidth = p->Value() * 0.01;
    fReverb.SetWidth(mWidth);
    break;
  case kSize:
    mSize = p->Value() * 0.01;
    fReverb.SetRoomSize(mSize);
    fReverb.Reset();
    break;
  case kPitchShift:
    mPitchShift = p->Value();
    fPitch.set_shift(mPitchShift);
    break;
  case kPitchTempo:
    mPitchTempo = p->Value();
    fPitch.set_tempo(mPitchTempo);
    break;
  case kHighpass:
    mHighpass = p->Value();
    fHighpass.SetFreq(mHighpass / GetSampleRate());
    break;
  case kLowpass:
    mLowpass = p->Value();
    fLowpass.SetFreq(mLowpass / GetSampleRate());
    break;
  case kCompress:
    mCompress = p->Value() * 0.01;
    mCompThresh = -18. - 36. * mCompress;
    mCompRatio = 1. / (1. + 3. * mCompress);
    fCompressor.SetThresh(mCompThresh);
    fCompressor.SetRatio(mCompRatio);
    fCompressor.SetMakeup((mCompRatio - 1.) * (-18. - mCompThresh));
    break;
  default:
    break;
  }
}
#endif
