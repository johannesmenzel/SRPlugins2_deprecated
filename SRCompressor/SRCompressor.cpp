#include "SRCompressor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "Utils/SRHelpers.h"
#include "Graphics/SRControls.h"

SRCompressor::SRCompressor(const InstanceInfo& info)
  : Plugin(info, MakeConfig(kNumParams, kNumPrograms))
  , mSamplerate(44100.0)
  , mThresh(0.0)
  , mRatio(5.0)
  , mAttack(10.0)
  , mRelease(100.0)
  , mKneeWidth(10.0)
  , mSidechainFc(10.0)
  , mMaxGr(-50.)
  , mMakeup(0.0)
  , mReference(-18.0)
  , mMix(1.0)
  , mIsMaxGrRatioDependent(true)
  , mIsFeedback(false)
  , mIsAutoMakeup(true)
  , mOutGain(0.0)
  , mIsBypassed(false)
{
  // double parameters
  GetParam(kThresh)->InitDouble("Threshold", 0., 0., 10., 1., "", 0, "", IParam::ShapeLinear());
  GetParam(kRatio)->InitDouble("Ratio", 5., 0., 10., 1., "", 0, "", IParam::ShapeLinear());
  GetParam(kAttack)->InitDouble("Attack", 5., 0., 10., 1., "", 0, "", IParam::ShapeLinear());
  GetParam(kRelease)->InitDouble("Release", 5., 0., 10., 1., "", 0, "", IParam::ShapeLinear());
  GetParam(kKneeWidth)->InitDouble("Knee", 10., 0., 30., 0.1, "dB", 0, "", IParam::ShapePowCurve(SR::Utils::SetShapeCentered(0., 30., 10., .5)));
  GetParam(kSidechainFc)->InitDouble("SC Filter", 10., 10., 5000., 1., "Hz", 0, "", IParam::ShapePowCurve(SR::Utils::SetShapeCentered(10., 5000., 200., .5)));
  GetParam(kMaxGr)->InitDouble("GR Limiter", -50., -50, 0., 0.1, "dB", 0, "", IParam::ShapePowCurve(SR::Utils::SetShapeCentered(-50., 0., -10., .5)));
  GetParam(kMakeup)->InitDouble("Makeup", 0., -20., 20., 0.1, "dB");
  GetParam(kReference)->InitDouble("Reference", -18., -50., 0., 1., "dB", 0, "", IParam::ShapePowCurve(SR::Utils::SetShapeCentered(-50., 0., -18., .5)));
  GetParam(kMix)->InitDouble("Mix", 100., 0., 100., 0.1, "%");
  GetParam(kOutGain)->InitDouble("Output Gain", 0., -12., 12., 0.1, "dB");
  // bolean parameters
  GetParam(kIsMaxGrRatioDependent)->InitBool("GR-Dep", true, "GR-Dep", 0, "", "Off", "Ratio");
  GetParam(kIsFeedback)->InitBool("Topology", false, "Topology", 0, "", "FF", "FB");
  GetParam(kIsAutoMakeup)->InitBool("Auto-Makeup", true, "Makeup", 0, "", "Man", "Auto");
  GetParam(kIsBypassed)->InitBool("Bypassed", false, "Bypassed", 0, "", "Active", "Bypass");

  for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {
    OnParamChange(paramIdx);
  }
  OnReset();


#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {


    // TODO: Set room informations, doesn't work on resize by now
    mRoomInfo.Reset(
      GetEditorWidth() * 0.33f,
      GetEditorHeight() * 0.33f,
      (GetEditorHeight() + GetEditorWidth()) * 0.5f,
      GetEditorHeight(),
      GetEditorWidth()
    );

    const IPattern mPattern = IPattern::CreateRadialGradient(
      mRoomInfo.GetLightPositionX(),
      mRoomInfo.GetLightPositionY(),
      mRoomInfo.GetLightPositionZ(),
      {
        IColorStop(SR::Graphics::Layout::SR_DEFAULT_COLOR_CUSTOM_PANEL_BG, 0.0f),
        IColorStop(SR::Graphics::Layout::SR_DEFAULT_COLOR_CUSTOM_PLUGIN_BG, 1.0f)
      }
    );

    pGraphics->EnableMouseOver(true);
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(mPattern);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    const IRECT rectPlug = pGraphics->GetBounds();

    const int gridX = 13;
    const int gridY = 8;

    pGraphics->AttachControl(new IPanelControl(rectPlug.GetGridCell(0, 0, gridY, 1), COLOR_BLACK.WithOpacity(.5f), false));
    pGraphics->AttachControl(new SR::Graphics::Controls::SRMeter<5, 1024>(rectPlug.GetGridCell(4, 6, gridY, gridX).FracRectHorizontal(1.f, false).FracRectVertical(4.f, true), true, true, -18.f, 0.f, (float)SR::Utils::SetShapeCentered(-18., 0., -6., .5), 1, 3, "Lvl", "Opt", "Ds", "FET", "Lim"), cGrMeter, "Meter");
    pGraphics->AttachControl(new SR::Graphics::Controls::SRMeter<2, 1024>(rectPlug.GetGridCell(4, 5, gridY, gridX).FracRectHorizontal(1.f, false).FracRectVertical(4.f, true), false, true, -6.f, 6.f, 1.f, 1, 1), cInputRMSMeter, "Meter");

    // Knob
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(4, 2, gridY, gridX).FracRectHorizontal(3.f, false).FracRectVertical(3.f, true), kThresh, "Thresh", SR::Graphics::Layout::SR_DEFAULT_STYLE_KNOB, mRoomInfo, SR::Graphics::Layout::SR_DEFAULT_COLOR_CUSTOM_RED, false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kThresh)->GetDefault(true)), cThresh);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(4, 8, gridY, gridX).FracRectHorizontal(3.f, false).FracRectVertical(3.f, true), kRatio, "Ratio", SR::Graphics::Layout::SR_DEFAULT_STYLE_KNOB, mRoomInfo, SR::Graphics::Layout::SR_DEFAULT_COLOR_CUSTOM_RED, false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kRatio)->GetDefault(true)), cRatio);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(1, 1, gridY, gridX).FracRectHorizontal(2.f, false).FracRectVertical(2.f, true), kSidechainFc, "SC Filter", SR::Graphics::Layout::SR_DEFAULT_STYLE_KNOB, mRoomInfo, SR::Graphics::Layout::SR_DEFAULT_COLOR_CUSTOM_ORANGE, false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kSidechainFc)->GetDefault(true)), cSidechainFc);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(1, 3, gridY, gridX).FracRectHorizontal(2.f, false).FracRectVertical(2.f, true), kAttack, "Attack", SR::Graphics::Layout::SR_DEFAULT_STYLE_KNOB, mRoomInfo, SR::Graphics::Layout::SR_DEFAULT_COLOR_CUSTOM_BLUE, false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kAttack)->GetDefault(true)), cAttack);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(1, 8, gridY, gridX).FracRectHorizontal(2.f, false).FracRectVertical(2.f, true), kRelease, "Release", SR::Graphics::Layout::SR_DEFAULT_STYLE_KNOB, mRoomInfo, SR::Graphics::Layout::SR_DEFAULT_COLOR_CUSTOM_BLUE, false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kRelease)->GetDefault(true)), cRelease);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(1, 10, gridY, gridX).FracRectHorizontal(2.f, false).FracRectVertical(2.f, true), kKneeWidth, "Knee", SR::Graphics::Layout::SR_DEFAULT_STYLE_KNOB, mRoomInfo, SR::Graphics::Layout::SR_DEFAULT_COLOR_CUSTOM_GREEN, false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kKneeWidth)->GetDefault(true)), cKneeWidth);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(1, 6, gridY, gridX).FracRectHorizontal(1.f, false).FracRectVertical(2.f, true), kMix, "Mix", SR::Graphics::Layout::SR_DEFAULT_STYLE_KNOB, mRoomInfo, SR::Graphics::Layout::SR_DEFAULT_COLOR_CUSTOM_BLUE, false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kMix)->GetDefault(true)), cMix);

    // Fader
    pGraphics->AttachControl(new IVSliderControl(rectPlug.GetGridCell(5, 0, gridY, gridX).FracRectHorizontal(1.f, false).FracRectVertical(3.f, true), kReference, "Reference", SR::Graphics::Layout::SR_DEFAULT_STYLE_FADER), cReference);
    pGraphics->AttachControl(new IVSliderControl(rectPlug.GetGridCell(4, 1, gridY, gridX).FracRectHorizontal(1.f, false).FracRectVertical(4.f, true), kMakeup, "Makeup", SR::Graphics::Layout::SR_DEFAULT_STYLE_FADER), cMakeup);
    pGraphics->AttachControl(new IVSliderControl(rectPlug.GetGridCell(4, 11, gridY, gridX).FracRectHorizontal(1.f, false).FracRectVertical(4.f, true), kOutGain, "Output", SR::Graphics::Layout::SR_DEFAULT_STYLE_FADER), cOutGain);
    pGraphics->AttachControl(new IVSliderControl(rectPlug.GetGridCell(5, 12, gridY, gridX).FracRectHorizontal(1.f, false).FracRectVertical(3.f, true), kMaxGr, "Max GR", SR::Graphics::Layout::SR_DEFAULT_STYLE_FADER), cMaxGr);
    // Switch
    pGraphics->AttachControl(new IVSlideSwitchControl(rectPlug.GetGridCell(1, 0, gridY, gridX).FracRectHorizontal(1.f, false).FracRectVertical(1.f, true), kIsBypassed, "Bypass", SR::Graphics::Layout::SR_DEFAULT_STYLE_BUTTON, true, EDirection::Vertical), cIsBypassed);
    pGraphics->AttachControl(new IVSlideSwitchControl(rectPlug.GetGridCell(1, 12, gridY, gridX).FracRectHorizontal(1.f, false).FracRectVertical(1.f, true), kIsFeedback, "Topology", SR::Graphics::Layout::SR_DEFAULT_STYLE_BUTTON, true, EDirection::Vertical), cIsFeedback);
    pGraphics->AttachControl(new IVSlideSwitchControl(rectPlug.GetGridCell(4, 0, gridY, gridX).FracRectHorizontal(1.f, false).FracRectVertical(1.f, true), kIsAutoMakeup, "Makeup", SR::Graphics::Layout::SR_DEFAULT_STYLE_BUTTON, true, EDirection::Vertical), cIsAutoMakeup);
    pGraphics->AttachControl(new IVSlideSwitchControl(rectPlug.GetGridCell(4, 12, gridY, gridX).FracRectHorizontal(1.f, false).FracRectVertical(1.f, true), kIsMaxGrRatioDependent, "GR-Dep", SR::Graphics::Layout::SR_DEFAULT_STYLE_BUTTON, true, EDirection::Vertical), cIsMaxGrRatioDependent);
  };
#endif
}


#if IPLUG_DSP
const double SRCompressor::GetCompressorValues(int stage, int param, double value)
{
switch (param) {
  case 1: // Threshold
    switch (stage) {
    case 1: return -value * 48.; break; // Level
    case 2: return -value * 36.; break; // Opto
    case 4: return -value * 24.; break; // FET
    case 5: return -value * 12.; break; // Limit
    default: break;
    }
  case 2: // Ratio
    switch (stage) {
    case 1: return 1. / (1. + value * 1); break;
    case 2: return 1. / (1. + value * 4.); break;
    case 4: return 1. / (1. + value * 19.); break;
    case 5: return 1. / (1. + value * 99.); break;
    default: break;
    }
  case 3: // Attack
    switch (stage) {
    case 1: return (20. + value * 80.); break;
    case 2: return (5. + value * 20.); break;
    case 4: return (1. + value * 4.); break;
    case 5: return (0.02 + value * 0.08); break;
    default: break;
    }
  case 4: // Release
    switch (stage) {
    case 1: return (500. + value * 4500.); break;
    case 2: return (200. + value * 1800.); break;
    case 4: return (40. + value * 360.); break;
    case 5: return (20. + value * 180.); break;
    default: break;
    }
  default:
    break;
    return 0;
  }
}

void SRCompressor::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = NOutChansConnected();
  bGrMeter.SetNumFrames(nFrames);


  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s];
    }
  }

  if (!mIsBypassed) {
    for (int s = 0; s < nFrames; s++) {
      for (int c = 0; c < nChans; c++) {
        fSoftSatInput[c].Process(outputs[c][s]);
      }

      fCompLevel.Process(outputs[0][s], outputs[1][s]);
      fCompOpto.Process(outputs[0][s], outputs[1][s]);
      fCompFet.Process(outputs[0][s], outputs[1][s]);
      fCompLim.Process(outputs[0][s], outputs[1][s]);

      for (int c = 0; c < nChans; c++) {
        fSoftSatOutput[c].Process(outputs[c][s]);
        outputs[c][s] = ((1. - mMix) * inputs[c][s]) + mMix * outputs[c][s];
      }

      fOutGain.Process(outputs[0][s], outputs[1][s]);

      bGrMeter.ProcessBuffer(fCompLevel.GetGrLin(), 0, s);
      bGrMeter.ProcessBuffer(fCompOpto.GetGrLin(), 1, s);
      bGrMeter.ProcessBuffer(0.0, 2, s);
      bGrMeter.ProcessBuffer(fCompFet.GetGrLin(), 3, s);
      bGrMeter.ProcessBuffer(fCompLim.GetGrLin(), 4, s);
    }
    mGrMeterBallistics.ProcessBlock(bGrMeter.GetBuffer(), nFrames);
  }

}


void SRCompressor::OnReset()
{
  mSamplerate = GetSampleRate();

  fOutGain.InitGain(100);

  fCompLevel.InitCompressor(
    GetCompressorValues(1, 1, mThresh),
    GetCompressorValues(1, 2, mRatio),
    GetCompressorValues(1, 3, mAttack),
    GetCompressorValues(1, 4, mRelease),
    mSidechainFc / mSamplerate, mKneeWidth, mIsFeedback, mIsAutoMakeup, mReference, mSamplerate);
  fCompLevel.SetMaxGrDb(mMaxGr, mIsMaxGrRatioDependent);

  fCompOpto.InitCompressor(
    GetCompressorValues(2, 1, mThresh),
    GetCompressorValues(2, 2, mRatio),
    GetCompressorValues(2, 3, mAttack),
    GetCompressorValues(2, 4, mRelease),
    mSidechainFc / mSamplerate, mKneeWidth, mIsFeedback, mIsAutoMakeup, mReference, mSamplerate);
  fCompOpto.SetMaxGrDb(mMaxGr, mIsMaxGrRatioDependent);

  fDeesser.SetDeesser(
    GetCompressorValues(3, 1, mThresh),
    GetCompressorValues(3, 2, mRatio),
    GetCompressorValues(3, 3, mAttack),
    GetCompressorValues(3, 4, mRelease),
    mSidechainFc / mSamplerate, 0.5, mKneeWidth, mSamplerate);
  fDeesser.Reset();

  fCompFet.InitCompressor(
    GetCompressorValues(4, 1, mThresh),
    GetCompressorValues(4, 2, mRatio),
    GetCompressorValues(4, 3, mAttack),
    GetCompressorValues(4, 4, mRelease),
    mSidechainFc / mSamplerate, mKneeWidth, mIsFeedback, mIsAutoMakeup, mReference, mSamplerate);
  fCompFet.SetMaxGrDb(mMaxGr, mIsMaxGrRatioDependent);

  fCompLim.InitCompressor(
    GetCompressorValues(5, 1, mThresh),
    GetCompressorValues(5, 2, mRatio),
    GetCompressorValues(5, 3, mAttack),
    GetCompressorValues(5, 4, mRelease),
    mSidechainFc / mSamplerate, mKneeWidth, mIsFeedback, mIsAutoMakeup, mReference, mSamplerate);
  fCompLim.SetMaxGrDb(mMaxGr, mIsMaxGrRatioDependent);

  for (int c = 0; c < 2; c++) {
    fSoftSatInput[c].SetSaturation(SR::DSP::SRSaturation::kSoftSat, 0., 1.0, 1., false, 0., 1., mSamplerate);
    fSoftSatOutput[c].SetSaturation(SR::DSP::SRSaturation::kSoftSat, 0., 1.0, 1., false, 0., 1., mSamplerate);
  }
  bGrMeter.ResetBuffer(5, GetBlockSize());
}

void SRCompressor::OnIdle() {
  mGrMeterBallistics.TransmitData(*this);
}

void SRCompressor::OnParamChange(int paramIdx)
{
  switch (paramIdx)
  {
  case kThresh:
    mThresh = GetParam(paramIdx)->Value() * 0.1;
    fCompLevel.SetThresh(GetCompressorValues(1, 1, mThresh));
    fCompOpto.SetThresh(GetCompressorValues(2, 1, mThresh));
    fCompFet.SetThresh(GetCompressorValues(4, 1, mThresh));
    fCompLim.SetThresh(GetCompressorValues(5, 1, mThresh));
    break;
  case kRatio:
    mRatio = GetParam(paramIdx)->Value() * 0.1;
    fCompLevel.SetRatio(GetCompressorValues(1, 2, mRatio));
    fCompOpto.SetRatio(GetCompressorValues(2, 2, mRatio));
    fCompFet.SetRatio(GetCompressorValues(4, 2, mRatio));
    fCompLim.SetRatio(GetCompressorValues(5, 2, mRatio));
    break;
  case kAttack:
    mAttack = GetParam(paramIdx)->Value() * 0.1;
    fCompLevel.SetAttack(GetCompressorValues(1, 3, mAttack));
    fCompOpto.SetAttack(GetCompressorValues(2, 3, mAttack));
    fCompFet.SetAttack(GetCompressorValues(4, 3, mAttack));
    fCompLim.SetAttack(GetCompressorValues(5, 3, mAttack));
    break;
  case kRelease:
    mRelease = GetParam(paramIdx)->Value() * 0.1;
    fCompLevel.SetRelease(GetCompressorValues(1, 4, mRelease));
    fCompOpto.SetRelease(GetCompressorValues(2, 4, mRelease));
    fCompFet.SetRelease(GetCompressorValues(4, 4, mRelease));
    fCompLim.SetRelease(GetCompressorValues(5, 4, mRelease));
    break;
  case kMakeup:
    mMakeup = GetParam(paramIdx)->Value();
    fCompLevel.SetMakeup(mMakeup);
    fCompOpto.SetMakeup(mMakeup);
    fCompFet.SetMakeup(mMakeup);
    fCompLim.SetMakeup(mMakeup);
    break;
  case kKneeWidth:
    mKneeWidth = GetParam(paramIdx)->Value();
    fCompLevel.SetKnee(mKneeWidth);
    fCompOpto.SetKnee(mKneeWidth);
    fCompFet.SetKnee(mKneeWidth);
    fCompLim.SetKnee(mKneeWidth);
    break;
  case kReference:
    mReference = GetParam(paramIdx)->Value();
    fCompLevel.SetReference(mReference);
    fCompOpto.SetReference(mReference);
    fCompFet.SetReference(mReference);
    fCompLim.SetReference(mReference);
    break;
  case kMix:
    mMix = GetParam(paramIdx)->Value() * 0.01;
    break;
  case kSidechainFc:
    mSidechainFc = GetParam(paramIdx)->Value();
    fCompLevel.SetSidechainFilterFreq(mSidechainFc / mSamplerate);
    fCompOpto.SetSidechainFilterFreq(mSidechainFc / mSamplerate);
    fCompFet.SetSidechainFilterFreq(mSidechainFc / mSamplerate);
    fCompLim.SetSidechainFilterFreq(mSidechainFc / mSamplerate);
    break;
  case kMaxGr:
  case kIsMaxGrRatioDependent:
    mMaxGr = GetParam(kMaxGr)->Value();
    mIsMaxGrRatioDependent = GetParam(kIsMaxGrRatioDependent)->Bool();
    fCompLevel.SetMaxGrDb(mMaxGr, mIsMaxGrRatioDependent);
    fCompOpto.SetMaxGrDb(mMaxGr, mIsMaxGrRatioDependent);
    fCompFet.SetMaxGrDb(mMaxGr, mIsMaxGrRatioDependent);
    fCompLim.SetMaxGrDb(mMaxGr, mIsMaxGrRatioDependent);
    break;
  case kIsBypassed:
    mIsBypassed = GetParam(paramIdx)->Bool();
    break;
  case kIsAutoMakeup:
    mIsAutoMakeup = GetParam(paramIdx)->Bool();
    fCompLevel.SetIsAutoMakeup(mIsAutoMakeup);
    fCompOpto.SetIsAutoMakeup(mIsAutoMakeup);
    fCompFet.SetIsAutoMakeup(mIsAutoMakeup);
    fCompLim.SetIsAutoMakeup(mIsAutoMakeup);
    break;
  case kOutGain:
    mOutGain = DBToAmp(GetParam(paramIdx)->Value());
    fOutGain.SetGain(mOutGain);
  case kIsFeedback:
    mIsFeedback = GetParam(paramIdx)->Bool();
    fCompLevel.SetTopologyFeedback(mIsFeedback);
    fCompOpto.SetTopologyFeedback(mIsFeedback);
    fCompFet.SetTopologyFeedback(mIsFeedback);
    fCompLim.SetTopologyFeedback(mIsFeedback);
    break;
  default:
    break;
  }
}

#endif
