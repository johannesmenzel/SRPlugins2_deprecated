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
  , mIsBypassed(false)
{
  // double parameters
  GetParam(kThresh)->InitDouble("Threshold", 0., -50., 0., 0.1, "dB");
  GetParam(kRatio)->InitDouble("Ratio", 5., 1., 100., 0.1, ":1", 0, "", IParam::ShapePowCurve(SR::Utils::SetShapeCentered(1., 100., 3., .5)));
  GetParam(kAttack)->InitDouble("Attack", 10., 0.02, 200., 0.01, "ms", 0, "", IParam::ShapePowCurve(SR::Utils::SetShapeCentered(0.02, 200, 10., .5)));
  GetParam(kRelease)->InitDouble("Release", 100., 30., 2000., 1., "ms", 0, "", IParam::ShapePowCurve(SR::Utils::SetShapeCentered(30., 2000., 100., .5)));
  GetParam(kKneeWidth)->InitDouble("Knee", 10., 0., 30., 0.1, "dB", 0, "", IParam::ShapePowCurve(SR::Utils::SetShapeCentered(0., 30., 10., .5)));
  GetParam(kSidechainFc)->InitDouble("SC Filter", 10., 10., 5000., 1., "Hz", 0, "", IParam::ShapePowCurve(SR::Utils::SetShapeCentered(10., 5000., 200., .5)));
  GetParam(kMaxGr)->InitDouble("GR Limiter", -50., -50, 0., 0.1, "dB", 0, "", IParam::ShapePowCurve(SR::Utils::SetShapeCentered(-50., 0., -10., .5)));
  GetParam(kMakeup)->InitDouble("Makeup", 0., 0., 20., 0.1, "dB");
  GetParam(kReference)->InitDouble("Reference", -18., -50., 0., 1., "dB", 0, "", IParam::ShapePowCurve(SR::Utils::SetShapeCentered(-50., 0., -18., .5)));
  GetParam(kMix)->InitDouble("Mix", 100., 0., 100., 0.1, "%");
  // bolean parameters
  GetParam(kIsMaxGrRatioDependent)->InitBool("GR Lmt RatDep", true, "", 0, "", "Rt-Ind", "Rt-Dep");
  GetParam(kIsFeedback)->InitBool("Topology", false, "FF/FB", 0, "", "FF", "FB");
  GetParam(kIsAutoMakeup)->InitBool("Auto Makeup", true, "", 0, "", "Man Make", "Auto Make");
  GetParam(kIsBypassed)->InitBool("Bypassed", false, "", 0, "", "Active", "Bypass");


#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {

    
    // TODO: Set room informations, doesn't work on resize by now
    mRoomInfo.Reset(
      GetEditorWidth() * 0.5f,
      GetEditorHeight() * 0.5f,
      (GetEditorHeight() + GetEditorWidth()) * 0.5f,
      GetEditorHeight(),
      GetEditorWidth()
    );

    pGraphics->HandleMouseOver(true);
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(SR::Graphics::Layout::layout.GetColor(SR::Graphics::Layout::SRCustomColors::ECustomColors::kPanelBg));
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    const IRECT rectPlug = pGraphics->GetBounds();
    // Knob
    //pGraphics->AttachControl(new IVKnobControl(rectPlug.GetGridCell(2, 1, 6, 11).FracRectHorizontal(4.f, false).FracRectVertical(4.f, true), kThresh, "Thresh", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kKnob)), cThresh);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(2, 1, 6, 11).FracRectHorizontal(4.f, false).FracRectVertical(4.f, true), kThresh, "Thresh", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kKnob), mRoomInfo, SR::Graphics::Layout::layout.GetColor(SR::Graphics::Layout::SRCustomColors::ECustomColors::kRed), false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kThresh)->GetDefault(true)), cThresh);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(2, 6, 6, 11).FracRectHorizontal(4.f, false).FracRectVertical(4.f, true), kRatio, "Ratio", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kKnob), mRoomInfo, SR::Graphics::Layout::layout.GetColor(SR::Graphics::Layout::SRCustomColors::ECustomColors::kRed), false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kRatio)->GetDefault(true)), cRatio);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(0, 1, 6, 11).FracRectHorizontal(2.f, false).FracRectVertical(2.f, true), kSidechainFc, "SC Filter", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kKnob), mRoomInfo, SR::Graphics::Layout::layout.GetColor(SR::Graphics::Layout::SRCustomColors::ECustomColors::kRed), false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kSidechainFc)->GetDefault(true)), cSidechainFc);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(0, 3, 6, 11).FracRectHorizontal(2.f, false).FracRectVertical(2.f, true), kAttack, "Attack", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kKnob), mRoomInfo, SR::Graphics::Layout::layout.GetColor(SR::Graphics::Layout::SRCustomColors::ECustomColors::kRed), false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kAttack)->GetDefault(true)), cAttack);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(0, 6, 6, 11).FracRectHorizontal(2.f, false).FracRectVertical(2.f, true), kRelease, "Release", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kKnob), mRoomInfo, SR::Graphics::Layout::layout.GetColor(SR::Graphics::Layout::SRCustomColors::ECustomColors::kRed), false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kRelease)->GetDefault(true)), cRelease);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(0, 8, 6, 11).FracRectHorizontal(2.f, false).FracRectVertical(2.f, true), kKneeWidth, "Knee", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kKnob), mRoomInfo, SR::Graphics::Layout::layout.GetColor(SR::Graphics::Layout::SRCustomColors::ECustomColors::kRed), false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kKneeWidth)->GetDefault(true)), cKneeWidth);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorKnob(rectPlug.GetGridCell(0, 5, 6, 11).FracRectHorizontal(1.f, false).FracRectVertical(2.f, true), kMix, "Mix", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kKnob), mRoomInfo, SR::Graphics::Layout::layout.GetColor(SR::Graphics::Layout::SRCustomColors::ECustomColors::kRed), false, false, -155.f, 155.f, -155.f + 310.f * (float)GetParam(kMix)->GetDefault(true)), cMix);

// Fader
    pGraphics->AttachControl(new IVSliderControl(rectPlug.GetGridCell(2, 5, 6, 11).FracRectHorizontal(1.f, false).FracRectVertical(4.f, true), kMakeup, "Makeup", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kFader)), cMakeup);
    pGraphics->AttachControl(new IVSliderControl(rectPlug.GetGridCell(3, 0, 6, 11).FracRectHorizontal(1.f, false).FracRectVertical(3.f, true), kReference, "Reference", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kFader)), cReference);
    pGraphics->AttachControl(new IVSliderControl(rectPlug.GetGridCell(3, 10, 6, 11).FracRectHorizontal(1.f, false).FracRectVertical(3.f, true), kMaxGr, "Max GR", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kFader)), cMaxGr);
    // Switch
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorSwitch(rectPlug.GetGridCell(0, 0, 6, 11).FracRectHorizontal(1.f, false).FracRectVertical(1.f, true), kIsBypassed, "Bypass", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kButton), mRoomInfo, true), cIsBypassed);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorSwitch(rectPlug.GetGridCell(0, 10, 6, 11).FracRectHorizontal(1.f, false).FracRectVertical(1.f, true), kIsFeedback, "Topo", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kButton), mRoomInfo, true), cIsFeedback);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorSwitch(rectPlug.GetGridCell(2, 0, 6, 11).FracRectHorizontal(1.f, false).FracRectVertical(1.f, true), kIsAutoMakeup, "Auto", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kButton), mRoomInfo, true), cIsAutoMakeup);
    pGraphics->AttachControl(new SR::Graphics::Controls::SRVectorSwitch(rectPlug.GetGridCell(2, 10, 6, 11).FracRectHorizontal(1.f, false).FracRectVertical(1.f, true), kIsMaxGrRatioDependent, "R-Dep", SR::Graphics::Layout::layout.GetStyle(SR::Graphics::Layout::SRCustomStyles::ECustomStyles::kButton), mRoomInfo, true), cIsMaxGrRatioDependent);
  };
#endif
}

#if IPLUG_DSP
void SRCompressor::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = NOutChansConnected();

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

      fCompressor.Process(outputs[0][s], outputs[1][s]);

      for (int c = 0; c < nChans; c++) {
        fSoftSatOutput[c].Process(outputs[c][s]);
        outputs[c][s] = ((1. - mMix) * inputs[c][s]) + mMix * outputs[c][s];
      }
    }
  }
}


void SRCompressor::OnReset()
{
  mSamplerate = GetSampleRate();
  fCompressor.InitCompressor(mThresh, mRatio, mAttack, mRelease, mSidechainFc / mSamplerate, mKneeWidth, mIsFeedback, mIsAutoMakeup, mReference, mSamplerate);
  fCompressor.InitSidechainFilter(mSidechainFc / mSamplerate);
  fCompressor.Reset();
  fCompressor.SetMaxGrDb(mMaxGr, mIsMaxGrRatioDependent);
  for (int c = 0; c < 2; c++) {
    fSoftSatInput[c].SetSaturation(SR::DSP::SRSaturation::kSoftSat, 0., 1., 1., false, 0., 1., mSamplerate);
    fSoftSatOutput[c].SetSaturation(SR::DSP::SRSaturation::kSoftSat, 0., 1., 1., false, 0., 1., mSamplerate);
  }
}

void SRCompressor::OnParamChange(int paramIdx)
{
  switch (paramIdx)
  {
  case kThresh:
    mThresh = GetParam(paramIdx)->Value();
    fCompressor.SetThresh(mThresh);
    break;
  case kRatio:
    mRatio = 1. / GetParam(paramIdx)->Value();
    fCompressor.SetRatio(mRatio);
    break;
  case kAttack:
    mAttack = GetParam(paramIdx)->Value();
    fCompressor.SetAttack(mAttack);
    break;
  case kRelease:
    mRelease = GetParam(paramIdx)->Value();
    fCompressor.SetRelease(mRelease);
    break;
  case kMakeup:
    mMakeup = GetParam(paramIdx)->Value();
    fCompressor.SetMakeup(mMakeup);
    break;
  case kKneeWidth:
    mKneeWidth = GetParam(paramIdx)->Value();
    fCompressor.SetKnee(mKneeWidth);
    break;
  case kReference:
    mReference = GetParam(paramIdx)->Value();
    fCompressor.SetReference(mReference);
    break;
  case kMix:
    mMix = GetParam(paramIdx)->Value() * 0.01;
    break;
  case kSidechainFc:
    mSidechainFc = GetParam(paramIdx)->Value();
    fCompressor.SetSidechainFilterFreq(mSidechainFc / mSamplerate);
    break;
  case kMaxGr:
  case kIsMaxGrRatioDependent:
    mMaxGr = GetParam(kMaxGr)->Value();
    mIsMaxGrRatioDependent = GetParam(kIsMaxGrRatioDependent)->Bool();
    fCompressor.SetMaxGrDb(mMaxGr, mIsMaxGrRatioDependent);
    break;
  case kIsBypassed:
    mIsBypassed = GetParam(paramIdx)->Bool();
    break;
  case kIsAutoMakeup:
    mIsAutoMakeup = GetParam(paramIdx)->Bool();
    fCompressor.SetIsAutoMakeup(mIsAutoMakeup);
    break;
  case kIsFeedback:
    mIsFeedback = GetParam(paramIdx)->Bool();
    fCompressor.SetTopologyFeedback(mIsFeedback);
    break;
  default:
    break;
  }
}
#endif
