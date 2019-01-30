//#ifndef SRCHANNEL_H
#include "SRChannel.h"
//#endif // !SRCHANNEL_H

#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <functional>

SRChannel::SRChannel(IPlugInstanceInfo instanceInfo)
  : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
  , mEqHfQ(stQ)
  , mEqLfQ(stQ)
  , mEqAmount(1.)
  , mAutoGain(1.)
  , mCompPeakAutoMakeup(1.)
  , mCompRmsAutoMakeup(1.)
  , mAgcTrigger(false)
{
  // Name channels
  //if (GetAPI() == kAPIVST2) // for VST2 we name individual outputs
  //{
    SetChannelLabel(ERoute::kInput, 0, "In 1", true);
    SetChannelLabel(ERoute::kInput, 1, "In 2", true);
    SetChannelLabel(ERoute::kInput, 2, "SC 1", true);
    SetChannelLabel(ERoute::kInput, 3, "SC 2", true);
    SetChannelLabel(ERoute::kOutput, 0, "Out 1", true);
    SetChannelLabel(ERoute::kOutput, 1, "Out 2", true);
  //}
  //else {
  //  GetIOConfig(0)->mBusInfo->Empty();
  //  GetIOConfig(1)->mBusInfo->Empty();
  //  GetIOConfig(0)->AddBusInfo(ERoute::kInput, 2, "In");
  //  GetIOConfig(0)->AddBusInfo(ERoute::kOutput, 2, "Out");
  //  GetIOConfig(1)->AddBusInfo(ERoute::kInput, 2, "In");
  //  GetIOConfig(1)->AddBusInfo(ERoute::kInput, 2, "SC");
  //  GetIOConfig(1)->AddBusInfo(ERoute::kOutput, 2, "Out");
  //}

  // Initialize Parameters
  for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {
    //IParam* param = GetParam(paramIdx);												// ... for which we temporally create a pointer "param"
    const SRParamProperties &p = srchannelParamProperties[paramIdx];		// ... and a variable "p" pointing at the current parameters p
    switch (p.Type)
    {
    case IParam::EParamType::kTypeDouble:
      //InitDoubleShapeFromMiddlePosition(this, paramIdx, p.name, p.defaultVal, p.minVal, p.maxVal, p.stepValue, p.unit, 0, p.group, p.centerVal, p.centerPoint, IParam::kUnitCustom);
      GetParam(paramIdx)->InitDouble(p.name, p.defaultVal, p.minVal, p.maxVal, p.stepValue, p.unit, p.Flags, p.group, new ShapeFromMiddle(p.minVal, p.maxVal, p.centerVal, p.centerPoint), IParam::kUnitCustom);
      break;
    case IParam::EParamType::kTypeInt:
      GetParam(paramIdx)->InitInt(p.name, (int)p.defaultVal, (int)p.minVal, (int)p.maxVal, p.unit, 0, p.group);
      break;
    case IParam::EParamType::kTypeBool:
      GetParam(paramIdx)->InitBool(p.name, (bool)p.defaultVal, p.unit, 0, p.group, p.labelMin, p.labelMax);
      break;
    case IParam::EParamType::kTypeEnum:
      switch (paramIdx) {
      case kEqHpOrder: /*case kEqLpOrder:*/
        GetParam(paramIdx)->InitEnum(
          p.name,
          (int)p.defaultVal,
          EFilterSlope::kNumOrders,
          p.unit,
          0,
          p.group,
          "6 dB/oct",
          "12 dB/oct",
          "18 dB/oct",
          "24 dB/oct",
          "36 dB/oct",
          "48 dB/oct",
          "60 dB/oct",
          "72 dB/oct",
          "120 dB/oct"
        );
        break;
      case kSaturationType:
        GetParam(paramIdx)->InitEnum(p.name,
          (int)p.defaultVal,
          SR::DSP::SRSaturation::SaturationTypes::numTypes,
          p.unit,
          0,
          p.group,
          "MusicDSP",
          "Zoelzer",
          "Pirkle",
          "Pirkle Mod",
          "Half Rect",
          "Full Rect"
        ); break;
      case kOversamplingRate:
        GetParam(paramIdx)->InitEnum(
          p.name,
          (int)p.defaultVal,
          OverSampler<double>::kNumFactors,
          p.unit,
          0,
          p.group,
          "OS OFF",
          "OS 2X",
          "OS 4X",
          "OS 8X",
          "OS 16X"
        ); break;
      default: break;
      }
    default:
      break;
    }

    // Setup display texts
    switch (paramIdx)
    {
    case kLimiterThresh: GetParam(paramIdx)->SetDisplayText((int)p.maxVal, "Off"); break;
    case kEqHpFreq: case kCompPeakSidechainFilterFreq: GetParam(paramIdx)->SetDisplayText((int)p.minVal, "Off"); break;
    case kEqLpFreq: GetParam(paramIdx)->SetDisplayText((int)p.maxVal, "Off"); break;
    case kCompPeakRatio: GetParam(paramIdx)->SetDisplayText((int)p.maxVal, "inf"); break;
    default: break;
    }
    OnParamChange(paramIdx);
  }
  OnReset();

  MakePresets();


  // GRAPHICS func
#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {

    // CANVAS'
    const IRECT rectPlugin = pGraphics->GetBounds();      // Plug canvas
    const IRECT rectHeader = rectPlugin.GetFromTop(70.f);
    const IRECT rectFooter = rectPlugin.GetFromBottom(30.f);
    const IRECT rectControls = rectPlugin.GetReducedFromTop(70.f).GetReducedFromBottom(30.f); // Control canvas
    // Section rects
    const IRECT rectInput = rectControls.SubRectHorizontal(6, 0).GetPadded(-5.f);
    const IRECT rectEq = rectControls.SubRectHorizontal(6, 1).GetPadded(-5.f);
    const IRECT rectComp = rectControls.SubRectHorizontal(6, 2).GetPadded(-5.f);
    const IRECT rectPost = rectControls.SubRectHorizontal(6, 3).GetPadded(-5.f);
    const IRECT rectOutput = rectControls.SubRectHorizontal(6, 4).GetPadded(-5.f);
    const IRECT rectMeter = rectControls.SubRectHorizontal(6, 5).GetPadded(-5.f);

    const IPattern patternPanel = IPattern::CreateLinearGradient(rectControls.L, rectControls.T, rectControls.R, rectControls.B, { IColorStop(SRLayout.colorPanelBG, 0.2f), IColorStop(COLOR_BLACK, 0.5f) });

    // Set new RECTs on resize
    if (pGraphics->NControls()) {

      pGraphics->GetBackgroundControl()->SetTargetAndDrawRECTs(rectPlugin);
      // Resize logo
      //pGraphics->GetControlWithTag(cBitmapLogo)->SetTargetAndDrawRECTs(rectHeader.GetFromRight(300.f));
      pGraphics->GetControlWithTag(cSRPluginsLogo)->SetTargetAndDrawRECTs(rectHeader.SubRectVertical(2, 0).GetFromLeft(pGraphics->GetControlWithTag(cSRPluginsLogo)->GetRECT().W()));
      pGraphics->GetControlWithTag(cSRChannelLogo)->SetTargetAndDrawRECTs(rectHeader.SubRectVertical(2, 0).GetFromRight(pGraphics->GetControlWithTag(cSRChannelLogo)->GetRECT().W()));
      //pGraphics->GetControlWithTag(cVersion)->SetTargetAndDrawRECTs(pGraphics->GetControlWithTag(cBitmapLogo)->GetRECT().GetFromTop(SRLayout.textKnobLabel.mSize));
      // Resize section rect PANELS
      pGraphics->GetControlWithTag(cPanelInput)->SetTargetAndDrawRECTs(rectInput);
      pGraphics->GetControlWithTag(cPanelEq)->SetTargetAndDrawRECTs(rectEq);
      pGraphics->GetControlWithTag(cPanelComp)->SetTargetAndDrawRECTs(rectComp);
      pGraphics->GetControlWithTag(cPanelPost)->SetTargetAndDrawRECTs(rectPost);
      pGraphics->GetControlWithTag(cPanelOutput)->SetTargetAndDrawRECTs(rectOutput);
      pGraphics->GetControlWithTag(cPanelMeter)->SetTargetAndDrawRECTs(rectMeter);
      dynamic_cast<IPanelControl*>(pGraphics->GetControlWithTag(cPanelInput))->SetPattern(patternPanel);
      dynamic_cast<IPanelControl*>(pGraphics->GetControlWithTag(cPanelEq))->SetPattern(patternPanel);
      dynamic_cast<IPanelControl*>(pGraphics->GetControlWithTag(cPanelComp))->SetPattern(patternPanel);
      dynamic_cast<IPanelControl*>(pGraphics->GetControlWithTag(cPanelPost))->SetPattern(patternPanel);
      dynamic_cast<IPanelControl*>(pGraphics->GetControlWithTag(cPanelOutput))->SetPattern(patternPanel);
      dynamic_cast<IPanelControl*>(pGraphics->GetControlWithTag(cPanelMeter))->SetPattern(patternPanel);

      pGraphics->GetControlWithTag(cInputMeter)->SetTargetAndDrawRECTs(rectMeter.SubRectHorizontal(3, 0));
      pGraphics->GetControlWithTag(cGrMeter)->SetTargetAndDrawRECTs(rectMeter.SubRectHorizontal(3, 1));
      pGraphics->GetControlWithTag(cOutputMeter)->SetTargetAndDrawRECTs(rectMeter.SubRectHorizontal(3, 2));
      pGraphics->GetControlWithTag(cScope)->SetTargetAndDrawRECTs(rectHeader);

      pGraphics->GetControlWithTag(cPresetMenu)->SetTargetAndDrawRECTs(rectHeader.SubRectVertical(2, 0).GetReducedFromLeft(pGraphics->GetControlWithTag(cSRPluginsLogo)->GetRECT().W()).GetReducedFromRight(pGraphics->GetControlWithTag(cSRChannelLogo)->GetRECT().W()));
      pGraphics->GetControlWithTag(cFreqMeter)->SetTargetAndDrawRECTs(rectEq);


      for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {
        const IRECT* rect;
        const SRParamProperties& p = srchannelParamProperties[paramIdx];		// ... and a variable "p" pointing at the current parameters p
        const int ctrlIdx = p.ctrlTag;

        switch (p.position.AttachToControlPanel) {
        case RectHeader: rect = &rectHeader; break;
        case RectInput: rect = &rectInput; break;
        case RectEq: rect = &rectEq; break;
        case RectComp: rect = &rectComp; break;
        case RectPost: rect = &rectPost; break;
        case RectOutput: rect = &rectOutput; break;
        case RectFooter: rect = &rectFooter; break;
        default: break;
        }

        const IRECT rectCurrentControl = rect->GetGridCell(p.position.y, p.position.x, sectionRectGridCells[p.position.AttachToControlPanel][0], sectionRectGridCells[p.position.AttachToControlPanel][1]).FracRectHorizontal(p.position.w, false).FracRectVertical(p.position.h, true).GetPadded((p.Knobs == Button) ? -5.0f : 0.0f);


        switch (p.Type)
        {
        case IParam::EParamType::kTypeInt: // No int ctrl...
        case IParam::EParamType::kTypeDouble:
          switch (paramIdx)
          {
          case kInputGain:
          case kOutputGain:
            // Resize faders
            pGraphics->GetControlWithTag(ctrlIdx)->SetTargetAndDrawRECTs(rectCurrentControl);
            break;
          default:
            // Resize knobs
            pGraphics->GetControlWithTag(ctrlIdx)->SetTargetAndDrawRECTs(rectCurrentControl);
            break;
          }
          break;
          // Resize switches
        case IParam::EParamType::kTypeEnum:
        case IParam::EParamType::kTypeBool:
          pGraphics->GetControlWithTag(ctrlIdx)->SetTargetAndDrawRECTs(rectCurrentControl);
          break;
        default:
          break;
        }
      }
      return;
    } // End resize function

    // LOAD
    pGraphics->LoadFont(ROBOTTO_FN);                                        // Load std font
    //pGraphics->LoadFont(CENTURY_FN);
    //IBitmap bmpLogo = pGraphics->LoadBitmap(LOGO_FN);                       // Load logo bitmap
    IBitmap bmpSRPluginsLogo = pGraphics->LoadBitmap(SRPLUGINSLOGO_FN);                       // Load logo bitmap
    IBitmap bmpSRChannelLogo = pGraphics->LoadBitmap(SRCHANNELLOGO_FN);                       // Load logo bitmap
    //IBitmap bmpPanel = pGraphics->LoadBitmap(PANEL_FN);
    // SETUP
    if (!pGraphics->CanHandleMouseOver()) pGraphics->HandleMouseOver(true);                                       // Enable Mouseovers
    if (!pGraphics->TooltipsEnabled()) pGraphics->EnableTooltips(true);                                        // Enable Tooltips



    // ATTACH
    pGraphics->AttachPanelBackground(SRLayout.colorPluginBG);        // Attach Background
    pGraphics->AttachCornerResizer(kUIResizerSize, true);                 // Attach Resizer

    // Attach logo
    //pGraphics->AttachControl(new IBitmapControl(rectHeader.GetFromRight(bmpLogo.W()), bmpLogo, -1, kBlendNone), cBitmapLogo);
    pGraphics->AttachControl(new IBitmapControl(rectHeader.SubRectVertical(2, 0).GetFromLeft(float(bmpSRPluginsLogo.W())), bmpSRPluginsLogo, -1, EBlendType::kBlendNone), cSRPluginsLogo, "UI");
    pGraphics->AttachControl(new IBitmapControl(rectHeader.SubRectVertical(2, 0).GetFromRight(float(bmpSRChannelLogo.W())), bmpSRChannelLogo, -1, EBlendType::kBlendNone), cSRChannelLogo, "UI");
    //pGraphics->AttachControl(new ITextControl(pGraphics->GetControlWithTag(cBitmapLogo)->GetRECT().GetFromTop(SRLayout.textKnobLabel.mSize), "v" PLUG_VERSION_STR"-a", SRLayout.textVersionString), cVersion, "UI");
    // Attach section rect PANELS
    pGraphics->AttachControl(new IPanelControl(rectInput, patternPanel, true), cPanelInput, "UI");
    pGraphics->AttachControl(new IPanelControl(rectEq, patternPanel, true), cPanelEq, "UI");
    pGraphics->AttachControl(new IPanelControl(rectComp, patternPanel, true), cPanelComp, "UI");
    pGraphics->AttachControl(new IPanelControl(rectPost, patternPanel, true), cPanelPost, "UI");
    pGraphics->AttachControl(new IPanelControl(rectOutput, patternPanel, true), cPanelOutput, "UI");
    pGraphics->AttachControl(new IPanelControl(rectMeter, patternPanel, true), cPanelMeter, "UI");


    // METERS peak and gain reduction
    pGraphics->AttachControl(new SR::Graphics::SRMeter<2, 1024>(rectMeter.SubRectHorizontal(3, 0), false, false, -60.f, 12.f, (float)SR::Utils::SetShapeCentered(-60., 12., 0., .75), 1, 6, "In Left", "In Right"), cInputMeter, "Meter");
    pGraphics->AttachControl(new SR::Graphics::SRMeter<3, 1024>(rectMeter.SubRectHorizontal(3, 1), true, true, -18.f, 0.f, (float)SR::Utils::SetShapeCentered(-18., 0., -9., .5), 1, 3, "GR RMS", "GR Peak", "GR Deesser"), cGrMeter, "Meter");
    pGraphics->AttachControl(new SR::Graphics::SRMeter<2, 1024>(rectMeter.SubRectHorizontal(3, 2), false, false, -60.f, 12.f, (float)SR::Utils::SetShapeCentered(-60., 12., 0., .75), 1, 6, "Out Left", "Out Right"), cOutputMeter, "Meter");
    pGraphics->AttachControl(new IVScopeControl<2>(rectHeader, "Left", "Right"), cScope, "Meter");
    // Set Tooltip for these
    pGraphics->GetControlWithTag(cInputMeter)->SetTooltip("Input peak meter for left and right channel");
    pGraphics->GetControlWithTag(cGrMeter)->SetTooltip("Gain reduction meter for RMS, peak and deessing compressors");
    pGraphics->GetControlWithTag(cOutputMeter)->SetTooltip("Output peak meter for left and right channel");
    pGraphics->GetControlWithTag(cScope)->SetTooltip("Scope fpr left and right channel");

    // Preset Menu
    pGraphics->AttachControl(new SR::Graphics::SRPresetMenu(this, rectHeader.SubRectVertical(2, 0).GetReducedFromLeft(float(bmpSRPluginsLogo.W())).GetReducedFromRight(float(bmpSRChannelLogo.W())), SRLayout.textPresetMenu, namedParams), cPresetMenu, "UI");
    pGraphics->AttachControl(new SR::Graphics::SRFrequencyResponseMeter(rectEq, FREQUENCYRESPONSE, mFreqMeterValues, SR::Utils::SetShapeCentered(0.,22000., 1000., .5)), cFreqMeter, "Meter");

    for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {
      const IRECT *rect;
      const SRParamProperties &p = srchannelParamProperties[paramIdx];		// ... and a variable "p" pointing at the current parameters p
      const int ctrlIdx = p.ctrlTag;

      switch (p.position.AttachToControlPanel) {
      case RectHeader: rect = &rectHeader; break;
      case RectInput: rect = &rectInput; break;
      case RectEq: rect = &rectEq; break;
      case RectComp: rect = &rectComp; break;
      case RectPost: rect = &rectPost; break;
      case RectOutput: rect = &rectOutput; break;
      case RectFooter: rect = &rectFooter; break;
      default: break;
      }
      const IRECT rectCurrentControl = rect->GetGridCell(p.position.y, p.position.x, sectionRectGridCells[p.position.AttachToControlPanel][0], sectionRectGridCells[p.position.AttachToControlPanel][1]).FracRectHorizontal(p.position.w, false).FracRectVertical(p.position.h, true).GetPadded((p.Knobs == Button) ? -5.0f : 0.0f);

      IColor knobColor;									// We're pointing at the type of knob we want to add

      switch (p.Knobs) {						// "knob" is gonna be a pointer to IBitmap
      case EControlImages::SslBlue: knobColor = SRLayout.colorKnobSslBlue; break;
      case EControlImages::SslGreen: knobColor = SRLayout.colorKnobSslGreen; break;
      case EControlImages::SslRed: knobColor = SRLayout.colorKnobSslRed; break;
      case EControlImages::SslOrange: knobColor = SRLayout.colorKnobSslOrange; break;
      case EControlImages::SslYellow: knobColor = SRLayout.colorKnobSslYellow; break;
      case EControlImages::SslBlack: knobColor = SRLayout.colorKnobSslBlack; break;
      case EControlImages::SslWhite: knobColor = SRLayout.colorKnobSslWhite; break;
      default: knobColor = SRLayout.colorSpec.mFGColor; break;
      }

      switch (p.Type)
      {
      case IParam::EParamType::kTypeInt: // No int ctrl...
      case IParam::EParamType::kTypeDouble:
        switch (paramIdx)
        {
        case kInputGain:
        case kOutputGain:
          // Attach faders
          pGraphics->AttachControl(new IVSliderControl(rectCurrentControl, paramIdx, SRLayout.colorSpec, kVertical, true, 32.f, 2.f), ctrlIdx);
          break;
        default:
          // Attach knobs
          pGraphics->AttachControl(new SR::Graphics::SRVectorKnobText(
            rectCurrentControl,
            paramIdx,
            p.label,
            p.labelMin,
            p.labelMax,
            p.labelCtr,
            true,
            true,
            SRLayout.colorSpec,
            knobColor,
            SRLayout.textKnobLabel,
            SRLayout.textKnobValue,
            -135.f,
            135.f,
            (float)GetParam(paramIdx)->GetDefault(true),
            0.5f,
            kVertical,
            4.f
          ), ctrlIdx);
          break;
        }
        break;
        // Attach switches
      case IParam::EParamType::kTypeEnum:
      case IParam::EParamType::kTypeBool:
        pGraphics->AttachControl(new SR::Graphics::SRVectorSwitch(
          rectCurrentControl,
          paramIdx,
          FlashCircleClickActionFunc,
          p.label,
          DEFAULT_SPEC,
          GetParam(paramIdx)->NDisplayTexts()
        ), ctrlIdx);
        break;
      default:
        break;
      }
      pGraphics->GetControlWithTag(ctrlIdx)->SetMOWhenGrayed(true);
      pGraphics->GetControlWithTag(ctrlIdx)->SetTooltip(p.tooltip);
    }

    pGraphics->StyleAllVectorControls(true, true, false, 0.1f, 2.f, 3.f, SRLayout.colorSpec);
  }; // END LAYOUT function
}// END GRAPHICS functions



// Method to gray controls, which are currently bypassed. Thats why you have to test from top to bottom
void SRChannel::GrayOutControls()
{
  if (GetUI()) {
    for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {
      const SRParamProperties& p = srchannelParamProperties[paramIdx];
      const bool grayout =
        (mBypass == 1 && p.group != "Global") ? true
        : (mInputBypass == 1 && p.group == "Input") ? true
        : (mCompBypass == 1 && (p.group == "Compressor" || p.group == "Deesser")) ? true
        : (mEqBypass == 1 && p.group == "EQ") ? true
        : (mOutputBypass == 1 && p.group == "Output") ? true
        : (mCompIsParallel == 0 && paramIdx == kCompPeakRmsRatio) ? true
        //: (paramIdx == kTestParam1
        //  || paramIdx == kTestParam2
        //  || paramIdx == kTestParam3
        //  || paramIdx == kTestParam4
        //  || paramIdx == kTestParam5
        //  || paramIdx == kEqLpOrder ? true
        : false;
      GetUI()->GetControlWithTag(p.ctrlTag)->GrayOut(grayout);
    }
  }
}

void SRChannel::SetFreqMeterValues() {
  //if (GetUI()->GetControl(cFreqMeter)) {
      /*if (fEqHmfFilter) */
  const double shape = log(0.5 * mSampleRate);
  for (int i = 0; i < FREQUENCYRESPONSE; i++) {
    //double freq = 0.5 * mSampleRate * double(i) / double(FREQUENCYRESPONSE); // Linear shape
    double freq = std::pow((double(i) / double(FREQUENCYRESPONSE)), shape) * (0.5 * mSampleRate); // Pow shape
    mFreqMeterValue[i] = 0.;
    if (mEqLfGain != 0.0) mFreqMeterValues[i] += fEqLfFilter[0].GetFrequencyResponse(freq);
    if (mEqLmfGain != 0.0) mFreqMeterValues[i] += fEqLmfFilter[0].GetFrequencyResponse(freq);
    if (mEqHmfGain != 0.0) mFreqMeterValues[i] += fEqHmfFilter[0].GetFrequencyResponse(freq);
    if (mEqHfGain != 0.0) mFreqMeterValues[i] += fEqHfFilter[0].GetFrequencyResponse(freq);
  }
  if (GetUI()) dynamic_cast<SR::Graphics::SRFrequencyResponseMeter*>(GetUI()->GetControlWithTag(cFreqMeter))->UpdateValues(mFreqMeterValues);
  //}
}

void SRChannel::OnParamChangeUI(int paramIdx, EParamSource source)
{
  switch (paramIdx)
  {
  case kCompIsParallel:
  case kEqBypass:
  case kCompBypass:
  case kInputBypass:
  case kOutputBypass:
  case kBypass:
    GrayOutControls(); break;
  case kEqHfBell:
  case kEqLfBell:
  case kEqLfGain:
  case kEqLfFreq: 
  case kEqLmfGain:
  case kEqLmfFreq:
  case kEqLmfQ: 
  case kEqHmfGain: 
  case kEqHmfFreq:
  case kEqHmfQ: 
  case kEqHfGain: 
  case kEqHfFreq:
  case kEqAmount:
    SetFreqMeterValues(); break;
  default:
    break;
  }
}

#endif

// DSP func
#if IPLUG_DSP

void SRChannel::InitEffects() {
  // Get sample rate
  mSampleRate = GetSampleRate();

  // Init gain and pan
  fInputGain.initGain(mInputGain, mInputGain, double(mSampleRate) / 10., false);
  fOutputGain.initGain(mOutputGain, mOutputGain, double(mSampleRate) / 10., false);
  fAutoGain.initGain(mAutoGain, mAutoGain, double(mSampleRate) / 10., false);
  fPan.initPan(SR::DSP::typeSinusodial, mPan, true);

  for (int i = 0; i < NChannelsConnected(kOutput); i++) {
    // Init EQ
    fEqHpFilterOnepole[i].setFc(mEqHpFreq);
    fEqHpFilter1[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fEqHpFilter2[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fEqHpFilter3[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fEqHpFilter4[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fEqHpFilter5[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fEqHpFilter6[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fEqHpFilter7[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fEqHpFilter8[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fEqHpFilter9[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fEqHpFilter10[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fEqLpFilter1[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_lowpass, mEqLpFreq / mSampleRate, stQ, 0., mSampleRate);
    fEqHfFilter[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highshelf, mEqHfFreq / mSampleRate, mEqHfQ, mEqHfGain, mSampleRate);
    fEqHmfFilter[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_peak, mEqHmfFreq / mSampleRate, mEqHmfQ, mEqHmfGain, mSampleRate);
    fEqLmfFilter[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_peak, mEqLmfFreq / mSampleRate, mEqLmfQ, mEqLmfGain, mSampleRate);
    fEqLfFilter[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::biquad_lowshelf, mEqLfFreq / mSampleRate, mEqLfQ, mEqLfGain, mSampleRate);
    fDcBlocker[i].setFc(10. / mSampleRate);

    // Init safe pan filter
    fSafePanHp[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::iir_linkwitz_highpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);
    fSafePanLp[i].setFilter(SR::DSP::SRFiltersTwoPole::FilterType::iir_linkwitz_lowpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);

    // Init saturation
    fInputSaturation[i].setSaturation(SR::DSP::SRSaturation::SaturationTypes::typeMusicDSP, mSaturationDrive, mSaturationAmount, mSaturationHarmonics, false, mSaturationSkew, 1., mSampleRate);

    //... Commented out until implementation of oversampling
    //fOverSamplerProcessL = std::bind(&SR::DSP::SRSaturation::process, fInputSaturation[0].process, std::placeholders::_1);
    //fOverSamplerProcessR = std::bind(&SR::DSP::SRSaturation::process, fInputSaturation[1].process, std::placeholders::_1);

    // Oversampling
    mOverSampler[i].Reset();
  }

  // Init compressor
  fCompressorPeak.initCompressor(mCompPeakThresh, mCompPeakRatio, mCompPeakAttack, mCompPeakRelease, mCompPeakSidechainFilterFreq, mCompPeakKneeWidthDb, mCompPeakIsFeedback, mSampleRate);
  fCompressorPeak.initRuntime();

  // For sidechain filter frequency it requires an own knob later
  fCompressorRms.initCompressor(mCompRmsThresh, mCompRmsRatio, mCompRmsAttack, mCompRmsRelease, mCompPeakSidechainFilterFreq, mCompRmsKneeWidthDb, 300., mCompRmsIsFeedback, mSampleRate);
  fCompressorRms.initRuntime();

  // Init limiter
  fLimiter.setSampleRate(mSampleRate);
  fLimiter.setAttack(1.);
  fLimiter.setRelease(100.);
  fLimiter.setThresh(mLimiterThresh);
  fLimiter.initRuntime();

  // Init deesser
  fDeesser.setDeesser(mDeesserThresh, mDeesserRatio, mDeesserAttack, mDeesserRelease, mDeesserFreq / mSampleRate, mDeesserQ, 10., mSampleRate);
  fDeesser.initRuntime();

  // Meter

  // Circular Buffer can be uncommented if needed
  //mCircularBuffer[0].Resize(circularBufferLenght, false);
  //mCircularBuffer[1].Resize(circularBufferLenght, false);
  //mCircularBuffer[2].Resize(circularBufferLenght, false);
  //mCircularBuffer[3].Resize(circularBufferLenght, false);
}

void SRChannel::ProcessBlock(sample** inputs, sample** outputs, int nFrames) {

  //for (int s = 0; s < nFrames; s++) {
  //  for (int c = 0; c < nChans; c++) {
  //    outputs[c][s] = inputs[c][s];
  //  }
  //}

  sample* in1 = inputs[0];
  sample* in2 = inputs[1];
  sample* sc1 = inputs[2];
  sample* sc2 = inputs[3];
  sample* out1 = outputs[0];
  sample* out2 = outputs[1];

  sample* in1MeterValue = new sample[nFrames];
  sample* in2MeterValue = new sample[nFrames];
  sample* out1MeterValue = new sample[nFrames];
  sample* out2MeterValue = new sample[nFrames];
  sample* grPeakMeterValue = new sample[nFrames];
  sample* grRmsMeterValue = new sample[nFrames];
  sample* grDeessMeterValue = new sample[nFrames];

  sample* inMeterChannel[2] = { in1MeterValue, in2MeterValue };
  sample* outMeterChannel[2] = { out1MeterValue, out2MeterValue };
  sample* grMeterChannel[3] = { grPeakMeterValue, grRmsMeterValue, grDeessMeterValue };

  sample** inMeterValues = inMeterChannel;
  sample** outMeterValues = outMeterChannel;
  sample** grMeterValues = grMeterChannel;


  // Begin Processing per Frame

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2) {

    // Begin global bypass test
    // ------------------------

    if (mBypass == 1) {
      *out1 = *in1;
      *out2 = *in2;
    }
    else {

      // PRE SECTIONS
      // ------------

      // Adding DC Off
      *out1 = *in1 + dcoff;
      *out2 = *in2 + dcoff;


      // Input Gain
      if (mInputGain != 1.) {
        //*out1 *= mInputGain;
        //*out2 *= mInputGain;
      }
      fInputGain.process(*out1, *out2);

      // Fill circular buffer with input gain values
      //circularBufferInL[circularBufferPointer] = *out1;
      //circularBufferInR[circularBufferPointer] = *out2;



      // ----------------
      // End Pre Sections

      in1MeterValue[s] = *out1;
      in2MeterValue[s] = *out2;


      // INPUT SECTION
      // -------------
      if (mInputBypass != 1) {

        // Saturation
        if (mSaturationAmount != 0.) {
          // Non oversampled saturation processing
          //*out1 = fInputSaturationL.process(*out1);
          //*out2 = fInputSaturationR.process(*out2);

          // oversampled saturation processing
          //using namespace std::placeholders;
          *out1 = mOverSampler[0].Process(*out1, std::bind(&SR::DSP::SRSaturation::process, fInputSaturation[0], std::placeholders::_1));
          *out2 = mOverSampler[1].Process(*out2, std::bind(&SR::DSP::SRSaturation::process, fInputSaturation[1], std::placeholders::_1));

          // oversampled saturation processing if std::function<double(double)> is at hand
          //*out1 = mOverSamplerL.Process(*out1 * mSaturationDrive, [](sample input) {return std::tanh(input);});
          //*out2 = mOverSamplerR.Process(*out2 * mSaturationDrive, [](sample input) {return std::tanh(input);});
        }

      }
      //	----------------
      // End Input Section



      // EQ SECTION
      // ----------

      if (mEqBypass != 1) {

        // Filter

        // High Pass

        if (mEqHpFreq > 16.) {
          // one pole for 1st and 3rd (and further odd) orders
          if (mEqHpOrder == EFilterSlope::dbo6 || mEqHpOrder == dbo18) {
            *out1 = fEqHpFilterOnepole[0].process(*out1);
            *out2 = fEqHpFilterOnepole[1].process(*out2);
          }
          // two pole for 2nd order
          if (mEqHpOrder >= EFilterSlope::dbo12) {
            *out1 = fEqHpFilter1[0].process(*out1);
            *out2 = fEqHpFilter1[1].process(*out2);
          }
          // two pole for 4th order
          if (mEqHpOrder >= EFilterSlope::dbo24) {
            *out1 = fEqHpFilter2[0].process(*out1);
            *out2 = fEqHpFilter2[1].process(*out2);
          }
          // two pole for 6th order
          if (mEqHpOrder >= EFilterSlope::dbo36) {
            *out1 = fEqHpFilter3[0].process(*out1);
            *out2 = fEqHpFilter3[1].process(*out2);
          }
          // two pole for 8th order
          if (mEqHpOrder >= EFilterSlope::dbo48) {
            *out1 = fEqHpFilter4[0].process(*out1);
            *out2 = fEqHpFilter4[1].process(*out2);
          }
          // two pole for 10th order
          if (mEqHpOrder >= EFilterSlope::dbo60) {
            *out1 = fEqHpFilter5[0].process(*out1);
            *out2 = fEqHpFilter5[1].process(*out2);
          }
          // two pole for 12th order
          if (mEqHpOrder >= EFilterSlope::dbo72) {
            *out1 = fEqHpFilter6[0].process(*out1);
            *out2 = fEqHpFilter6[1].process(*out2);
          }
          // two pole for 20th order
          if (mEqHpOrder >= EFilterSlope::dbo120) {
            *out1 = fEqHpFilter7[0].process(*out1);
            *out2 = fEqHpFilter7[1].process(*out2);
            *out1 = fEqHpFilter8[0].process(*out1);
            *out2 = fEqHpFilter8[1].process(*out2);
            *out1 = fEqHpFilter9[0].process(*out1);
            *out2 = fEqHpFilter9[1].process(*out2);
            *out1 = fEqHpFilter10[0].process(*out1);
            *out2 = fEqHpFilter10[1].process(*out2);
          }
        }

        // Low Pass

        if (mEqLpFreq < 22000.0) { *out1 = fEqLpFilter1[0].process(*out1); *out2 = fEqLpFilter1[1].process(*out2); }

        // Parametric EQ

        if (mEqLfGain != 0.0) { *out1 = fEqLfFilter[0].process(*out1); *out2 = fEqLfFilter[1].process(*out2); }
        if (mEqLmfGain != 0.0) { *out1 = fEqLmfFilter[0].process(*out1); *out2 = fEqLmfFilter[1].process(*out2); }
        if (mEqHmfGain != 0.0) { *out1 = fEqHmfFilter[0].process(*out1); *out2 = fEqHmfFilter[1].process(*out2); }
        if (mEqHfGain != 0.0) { *out1 = fEqHfFilter[0].process(*out1); *out2 = fEqHfFilter[1].process(*out2); }
      }
      // --------------
      // End EQ Section



      // COMPRESSOR SECTION
      // ------------------

      if (mCompBypass != 1) {

        // Deesser

        if (mDeesserRatio != 1.0 && mDeesserThresh != 0.0) {
          fDeesser.process(*out1, *out2);
        }

        // Compressor

        sample vCompDry1 = *out1;
        sample vCompDry2 = *out2;

        if (mCompIsParallel == 1) {
          sample vCompRmsIn1 = *out1;
          sample vCompPeakIn1 = *out1;
          sample vCompRmsIn2 = *out2;
          sample vCompPeakIn2 = *out2;

          if (mCompRmsRatio != 1. && mCompRmsThresh != 0.) {
            (mCompRmsIsExtSc != 1) ? fCompressorRms.process(vCompRmsIn1, vCompRmsIn2) : fCompressorRms.process(vCompRmsIn1, vCompRmsIn2, *sc1, *sc2);
          }

          if (mCompPeakRatio != 1. && mCompPeakThresh != 0.) {
            (mCompPeakIsExtSc != 1) ? fCompressorPeak.process(vCompPeakIn1, vCompPeakIn2) : fCompressorPeak.process(vCompPeakIn1, vCompPeakIn2, *sc1, *sc2);
          }

          if ((mCompRmsRatio != 1 && mCompRmsThresh != 0) || (mCompPeakRatio != 1 && mCompPeakThresh != 0)) {
            *out1 = (1 - mCompPeakRmsRatio) * vCompRmsIn1 * mCompRmsMakeup * mCompRmsAutoMakeup + mCompPeakRmsRatio * vCompPeakIn1 * mCompPeakMakeup * mCompPeakAutoMakeup;
            *out2 = (1 - mCompPeakRmsRatio) * vCompRmsIn2 * mCompRmsMakeup * mCompRmsAutoMakeup + mCompPeakRmsRatio * vCompPeakIn2 * mCompPeakMakeup * mCompPeakAutoMakeup;
          }
        }
        else {
          if (mCompRmsRatio != 1. && mCompRmsThresh != 0.) {
            (mCompRmsIsExtSc != 1) ? fCompressorRms.process(*out1, *out2) : fCompressorRms.process(*out1, *out2, *sc1, *sc2);
            *out1 *= mCompRmsMakeup * mCompRmsAutoMakeup;
            *out2 *= mCompRmsMakeup * mCompRmsAutoMakeup;
          }

          if (mCompPeakRatio != 1. && mCompPeakThresh != 0.) {
            (mCompPeakIsExtSc != 1) ? fCompressorPeak.process(*out1, *out2) : fCompressorPeak.process(*out1, *out2, *sc1, *sc2);
            *out1 *= mCompPeakMakeup * mCompPeakAutoMakeup;
            *out2 *= mCompPeakMakeup * mCompPeakAutoMakeup;
          }
        }

        // Compressor's Dry/Wet
        if (mCompDryWet != 1.) {
          *out1 = (mCompDryWet * *out1) + (vCompDry1 * (1. - mCompDryWet));
          *out2 = (mCompDryWet * *out2) + (vCompDry2 * (1. - mCompDryWet));
        }

      }
      // ----------------------
      // End Compressor Section



      // OUTPUT SECTION
      // --------------

      if (mOutputBypass != 1) {

        // Pan

        if (mPan != .5 || mIsPanMonoLow == true) {
          sample vSafePanLowSignal1 = *out1;
          sample vSafePanHighSignal1 = *out1;
          sample vSafePanLowSignal2 = *out2;
          sample vSafePanHighSignal2 = *out2;

          vSafePanLowSignal1 = fSafePanLp[0].process(vSafePanLowSignal1);
          vSafePanLowSignal2 = fSafePanLp[1].process(vSafePanLowSignal2);
          vSafePanHighSignal1 = fSafePanHp[0].process(vSafePanHighSignal1);
          vSafePanHighSignal2 = fSafePanHp[1].process(vSafePanHighSignal2);

          fPan.process(vSafePanHighSignal1, vSafePanHighSignal2);
          if (mIsPanMonoLow) {
            vSafePanLowSignal1 = (vSafePanLowSignal1 + vSafePanLowSignal2) * 0.5;
            vSafePanLowSignal2 = (vSafePanLowSignal1 + vSafePanLowSignal2) * 0.5;
          }
          *out1 = vSafePanHighSignal1 - vSafePanLowSignal1;
          *out2 = vSafePanHighSignal2 - vSafePanLowSignal2;
        }


        // Clipper

        if (mClipperThreshold < 1.) {
          *out1 = (fmin(fabs(*out1), mClipperThreshold) / sqrt(mClipperThreshold)) * (fabs(*out1) / *out1);
          *out2 = (fmin(fabs(*out2), mClipperThreshold) / sqrt(mClipperThreshold)) * (fabs(*out2) / *out2);
        }


        // Limiter

        if (mLimiterThresh != 10.) {
          fLimiter.process(*out1, *out2);
        }

        // Soft Limiter

        if (*out1 > 1.) {
          *out1 = (1. - 4 / (*out1 + (4 - 1.))) * 4 + 1.;
        }
        else {
          if (*out1 < -1.) {
            *out1 = (1. + 4 / (*out1 - (4 - 1.))) * -4 - 1.;
          }
        }

        if (*out2 > 1.) {
          *out2 = (1. - 4 / (*out2 + (4 - 1.))) * 4 + 1.;
        }
        else {
          if (*out2 < -1.) {
            *out2 = (1. + 4 / (*out2 - (4 - 1.))) * -4 - 1.;
          }
        }

      }
      // ------------------
      // End Output Section



      // POST SECTIONS
      // -------------

      // Fill circular buffer with output gain values
      //circularBufferOutL[circularBufferPointer] = *out1;
      //circularBufferOutR[circularBufferPointer] = *out2;


      // Output Gain
      fOutputGain.process(*out1, *out2);
      fAutoGain.process(*out1, *out2);
      //if (mOutputGain != 1.) {
      //	*out1 *= mOutputGain;
      //	*out2 *= mOutputGain;
      //}

      *out1 = fDcBlocker[0].process(*out1);
      *out2 = fDcBlocker[1].process(*out2);
      *out1 -= dcoff;
      *out2 -= dcoff;

      // ----------------
      // End Post Section


    }
    // -------------------------
    // End of global bypass test

    out1MeterValue[s] = *out1;
    out2MeterValue[s] = *out2;
    grPeakMeterValue[s] = fCompressorPeak.getGrLin();
    grRmsMeterValue[s] = fCompressorRms.getGrLin();
    grDeessMeterValue[s] = fDeesser.getGrLin();

    (circularBufferPointer >= circularBufferLenght - 1) ? circularBufferPointer = 0 : circularBufferPointer++;


  }
  // ----------------------------------------------------------------
  // End processing per frame and again some processing per Framesize



  if (mAgc && mAgcTrigger) {
    double sumIn = 0.; double sumOut = 0.;
    for (int s = 0; s < nFrames; s++) {
      sumIn += std::fabs(inMeterValues[0][s]) + std::fabs(inMeterValues[1][s]);
      sumOut += std::fabs(outMeterValues[0][s]) + std::fabs(outMeterValues[1][s]);
    }
    double diff = sumIn / sumOut;
    if (mAgc) fAutoGain.setGain(diff);
    mAgcTrigger = false;
  }


  mInputMeterBallistics.ProcessBlock(inMeterValues, nFrames);
  mGrMeterBallistics.ProcessBlock(grMeterValues, nFrames);
  mOutputMeterBallistics.ProcessBlock(outMeterValues, nFrames);
  mScopeBallistics.ProcessBlock(outMeterValues, nFrames);

  delete[] grPeakMeterValue;
  delete[] grRmsMeterValue;
  delete[] grDeessMeterValue;
  delete[] in1MeterValue;
  delete[] in2MeterValue;
  delete[] out1MeterValue;
  delete[] out2MeterValue;

}


void SRChannel::OnIdle() {
  mInputMeterBallistics.TransmitData(*this);
  mGrMeterBallistics.TransmitData(*this);
  mOutputMeterBallistics.TransmitData(*this);
  mScopeBallistics.TransmitData(*this);
}

void SRChannel::OnReset() {
  mSampleRate = GetSampleRate();
  mNumInChannels = NChannelsConnected(kInput);
  mNumOutChannels = NChannelsConnected(kOutput);
  InitEffects();
  circularBufferPointer = 0;
}

void SRChannel::OnParamChange(int paramIdx) {
  mSampleRate = GetSampleRate();

  if (mAgc) mAgcTrigger = true;

  switch (paramIdx)
  {

    // INPUT AND OUTPUT STAGE
    // ----------------------

  case kInputGain:
    mInputGain = DBToAmp(GetParam(paramIdx)->Value());
    fInputGain.setGain(mInputGain);
    break;
  case kOutputGain:
    mOutputGain = DBToAmp(GetParam(paramIdx)->Value());
    fOutputGain.setGain(mOutputGain);
    break;
  case kSaturationDrive:
    mSaturationDrive = GetParam(paramIdx)->Value();
    fInputSaturation[0].setDrive(mSaturationDrive);
    fInputSaturation[1].setDrive(mSaturationDrive);
    break;
  case kSaturationAmount:
    mSaturationAmount = GetParam(paramIdx)->Value() / 100.;
    fInputSaturation[0].setAmount(mSaturationAmount);
    fInputSaturation[1].setAmount(mSaturationAmount);
    break;
  case kSaturationHarmonics:
    mSaturationHarmonics = GetParam(paramIdx)->Value() / 100.;
    fInputSaturation[0].setHarmonics(mSaturationHarmonics);
    fInputSaturation[1].setHarmonics(mSaturationHarmonics);
    break;
  case kSaturationSkew:
    mSaturationSkew = GetParam(paramIdx)->Value() * 0.05;
    fInputSaturation[0].setSkew(mSaturationSkew);
    fInputSaturation[1].setSkew(mSaturationSkew);
    break;
  case kSaturationType:
    mSaturationType = int(GetParam(paramIdx)->Int());
    fInputSaturation[0].setType(mSaturationType);
    fInputSaturation[1].setType(mSaturationType);
    break;
  case kOversamplingRate:
    mOversamplingRate = int(GetParam(paramIdx)->Int());
    mOverSampler[0].SetOverSampling((OverSampler<sample>::EFactor)mOversamplingRate);
    mOverSampler[1].SetOverSampling((OverSampler<sample>::EFactor)mOversamplingRate);
    break;
  case kClipperThreshold: mClipperThreshold = 1. - GetParam(paramIdx)->Value() / 100.; break;
  case kLimiterThresh:
    mLimiterThresh = GetParam(paramIdx)->Value();
    fLimiter.setThresh(mLimiterThresh);
    break;
  case kPan: mPan = (GetParam(paramIdx)->Value() + 100) / 200; fPan.setPanPosition(mPan); break;
  case kPanFreq:
    mSafePanFreq = GetParam(paramIdx)->Value();
    fSafePanHp[0].setFc(mSafePanFreq / mSampleRate);
    fSafePanHp[1].setFc(mSafePanFreq / mSampleRate);
    fSafePanLp[0].setFc(mSafePanFreq / mSampleRate);
    fSafePanLp[1].setFc(mSafePanFreq / mSampleRate);
    break;
  case kIsPanMonoLow: mIsPanMonoLow = GetParam(paramIdx)->Bool(); break;


    // FILTER
    // ------

    // High Pass
  case kEqHpFreq:
    mEqHpFreq = GetParam(paramIdx)->Value();
    if (mEqHpOrder == EFilterSlope::dbo6 || mEqHpOrder == EFilterSlope::dbo18) {
      fEqHpFilterOnepole[0].setFc(mEqHpFreq / mSampleRate); fEqHpFilterOnepole[1].setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo12) {
      fEqHpFilter1[0].setFc(mEqHpFreq / mSampleRate); fEqHpFilter1[1].setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo24) {
      fEqHpFilter2[0].setFc(mEqHpFreq / mSampleRate); fEqHpFilter2[1].setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo36) {
      fEqHpFilter3[0].setFc(mEqHpFreq / mSampleRate); fEqHpFilter3[1].setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo48) {
      fEqHpFilter4[0].setFc(mEqHpFreq / mSampleRate); fEqHpFilter4[1].setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo60) {
      fEqHpFilter5[0].setFc(mEqHpFreq / mSampleRate); fEqHpFilter5[1].setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo72) {
      fEqHpFilter6[0].setFc(mEqHpFreq / mSampleRate); fEqHpFilter6[1].setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo120) {
      fEqHpFilter7[0].setFc(mEqHpFreq / mSampleRate); fEqHpFilter7[1].setFc(mEqHpFreq / mSampleRate);
      fEqHpFilter8[0].setFc(mEqHpFreq / mSampleRate); fEqHpFilter8[1].setFc(mEqHpFreq / mSampleRate);
      fEqHpFilter9[0].setFc(mEqHpFreq / mSampleRate); fEqHpFilter9[1].setFc(mEqHpFreq / mSampleRate);
      fEqHpFilter10[0].setFc(mEqHpFreq / mSampleRate); fEqHpFilter10[1].setFc(mEqHpFreq / mSampleRate);
    }
    break;

  case kEqHpOrder:
    mEqHpOrder = int(GetParam(paramIdx)->Int());
    switch (mEqHpOrder) {
    case EFilterSlope::dbo6:									// 1st order, 6 dB/Oct
      break;
    case EFilterSlope::dbo12:
      fEqHpFilter1[0].setQ(mEqPassQ_O2_F1);	// 2nd order, 12 dB/Oct
      fEqHpFilter1[1].setQ(mEqPassQ_O2_F1);
      break;
    case EFilterSlope::dbo18:									// 3rd order, 18 dB/Oct
      fEqHpFilter1[0].setQ(mEqPassQ_O3_F1);
      fEqHpFilter1[1].setQ(mEqPassQ_O3_F1);
      break;
    case EFilterSlope::dbo24:									// 4th order, 24 dB/Oct
      fEqHpFilter1[0].setQ(mEqPassQ_O4_F1);
      fEqHpFilter1[1].setQ(mEqPassQ_O4_F1);
      fEqHpFilter2[0].setQ(mEqPassQ_O4_F2);
      fEqHpFilter2[1].setQ(mEqPassQ_O4_F2);
      break;
    case EFilterSlope::dbo36:									// 6th order, 36 dB/Oct
      fEqHpFilter1[0].setQ(mEqPassQ_O6_F1);
      fEqHpFilter1[1].setQ(mEqPassQ_O6_F1);
      fEqHpFilter2[0].setQ(mEqPassQ_O6_F2);
      fEqHpFilter2[1].setQ(mEqPassQ_O6_F2);
      fEqHpFilter3[0].setQ(mEqPassQ_O6_F3);
      fEqHpFilter3[1].setQ(mEqPassQ_O6_F3);
      break;
    case EFilterSlope::dbo48:									// 8th order, 48 dB/Oct
      fEqHpFilter1[0].setQ(mEqPassQ_O8_F1);
      fEqHpFilter1[1].setQ(mEqPassQ_O8_F1);
      fEqHpFilter2[0].setQ(mEqPassQ_O8_F2);
      fEqHpFilter2[1].setQ(mEqPassQ_O8_F2);
      fEqHpFilter3[0].setQ(mEqPassQ_O8_F3);
      fEqHpFilter3[1].setQ(mEqPassQ_O8_F3);
      fEqHpFilter4[0].setQ(mEqPassQ_O8_F4);
      fEqHpFilter4[1].setQ(mEqPassQ_O8_F4);
      break;
    case EFilterSlope::dbo60:									// 10th order, 48 dB/Oct
      fEqHpFilter1[0].setQ(mEqPassQ_O10_F1);
      fEqHpFilter1[1].setQ(mEqPassQ_O10_F1);
      fEqHpFilter2[0].setQ(mEqPassQ_O10_F2);
      fEqHpFilter2[1].setQ(mEqPassQ_O10_F2);
      fEqHpFilter3[0].setQ(mEqPassQ_O10_F3);
      fEqHpFilter3[1].setQ(mEqPassQ_O10_F3);
      fEqHpFilter4[0].setQ(mEqPassQ_O10_F4);
      fEqHpFilter4[1].setQ(mEqPassQ_O10_F4);
      fEqHpFilter5[0].setQ(mEqPassQ_O10_F5);
      fEqHpFilter5[1].setQ(mEqPassQ_O10_F5);
      break;
    case EFilterSlope::dbo72:									// 12th order, 72 db/Oct
      fEqHpFilter1[0].setQ(mEqPassQ_O12_F1);
      fEqHpFilter1[1].setQ(mEqPassQ_O12_F1);
      fEqHpFilter2[0].setQ(mEqPassQ_O12_F2);
      fEqHpFilter2[1].setQ(mEqPassQ_O12_F2);
      fEqHpFilter3[0].setQ(mEqPassQ_O12_F3);
      fEqHpFilter3[1].setQ(mEqPassQ_O12_F3);
      fEqHpFilter4[0].setQ(mEqPassQ_O12_F4);
      fEqHpFilter4[1].setQ(mEqPassQ_O12_F4);
      fEqHpFilter5[0].setQ(mEqPassQ_O12_F5);
      fEqHpFilter5[1].setQ(mEqPassQ_O12_F5);
      fEqHpFilter6[0].setQ(mEqPassQ_O12_F6);
      fEqHpFilter6[1].setQ(mEqPassQ_O12_F6);
      break;
    case EFilterSlope::dbo120:									// 20th order, 120 dB/Oct
      fEqHpFilter1[0].setQ(mEqPassQ_O20_F1);
      fEqHpFilter1[1].setQ(mEqPassQ_O20_F1);
      fEqHpFilter2[0].setQ(mEqPassQ_O20_F2);
      fEqHpFilter2[1].setQ(mEqPassQ_O20_F2);
      fEqHpFilter3[0].setQ(mEqPassQ_O20_F3);
      fEqHpFilter3[1].setQ(mEqPassQ_O20_F3);
      fEqHpFilter4[0].setQ(mEqPassQ_O20_F4);
      fEqHpFilter4[1].setQ(mEqPassQ_O20_F4);
      fEqHpFilter5[0].setQ(mEqPassQ_O20_F5);
      fEqHpFilter5[1].setQ(mEqPassQ_O20_F5);
      fEqHpFilter6[0].setQ(mEqPassQ_O20_F6);
      fEqHpFilter6[1].setQ(mEqPassQ_O20_F6);
      fEqHpFilter7[0].setQ(mEqPassQ_O20_F7);
      fEqHpFilter7[1].setQ(mEqPassQ_O20_F7);
      fEqHpFilter8[0].setQ(mEqPassQ_O20_F8);
      fEqHpFilter8[1].setQ(mEqPassQ_O20_F8);
      fEqHpFilter9[0].setQ(mEqPassQ_O20_F9);
      fEqHpFilter9[1].setQ(mEqPassQ_O20_F9);
      fEqHpFilter10[0].setQ(mEqPassQ_O20_F10);
      fEqHpFilter10[1].setQ(mEqPassQ_O20_F10);
      break;
    default:
      break;
    }
    OnParamChange(kEqHpFreq);
    break;

    // Low Pass
  case kEqLpFreq:
    mEqLpFreq = GetParam(paramIdx)->Value();
    fEqLpFilter1[0].setFc(mEqLpFreq / mSampleRate);
    fEqLpFilter1[1].setFc(mEqLpFreq / mSampleRate);
    break;
    //case kEqLpOrder:
//  break;

    // EQUALIZER
    // ---------

  case kEqHfBell:
    mEqHfIsBell = GetParam(paramIdx)->Bool();
    if (mEqHfIsBell == 1) { fEqHfFilter[0].setType(SR::DSP::SRFiltersTwoPole::FilterType::biquad_peak); fEqHfFilter[1].setType(SR::DSP::SRFiltersTwoPole::FilterType::biquad_peak); }
    else { fEqHfFilter[0].setType(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highshelf); fEqHfFilter[1].setType(SR::DSP::SRFiltersTwoPole::FilterType::biquad_highshelf); }
    break;

  case kEqLfBell:
    mEqLfIsBell = GetParam(paramIdx)->Bool();
    if (mEqLfIsBell == 1) { fEqLfFilter[0].setType(SR::DSP::SRFiltersTwoPole::FilterType::biquad_peak); fEqLfFilter[1].setType(SR::DSP::SRFiltersTwoPole::FilterType::biquad_peak); }
    else { fEqLfFilter[0].setType(SR::DSP::SRFiltersTwoPole::FilterType::biquad_lowshelf); fEqLfFilter[1].setType(SR::DSP::SRFiltersTwoPole::FilterType::biquad_lowshelf); }
    break;

  case kEqLfGain: mEqLfGain = GetParam(paramIdx)->Value() * mEqAmount; fEqLfFilter[0].setPeakGain(mEqLfGain); fEqLfFilter[1].setPeakGain(mEqLfGain); break;
  case kEqLfFreq: mEqLfFreq = GetParam(paramIdx)->Value(); fEqLfFilter[0].setFc(mEqLfFreq / mSampleRate); fEqLfFilter[1].setFc(mEqLfFreq / mSampleRate); break;
    //case kEqLfQ: mEqLfQ = GetParam(paramIdx)->Value(); fEqLfFilter[0].setQ(mEqLfQ); fEqLfFilter[1].setQ(mEqLfQ); break;

  case kEqLmfGain: mEqLmfGain = GetParam(paramIdx)->Value() * mEqAmount; fEqLmfFilter[0].setPeakGain(mEqLmfGain); fEqLmfFilter[1].setPeakGain(mEqLmfGain); break;
  case kEqLmfFreq: mEqLmfFreq = GetParam(paramIdx)->Value(); fEqLmfFilter[0].setFc(mEqLmfFreq / mSampleRate); fEqLmfFilter[1].setFc(mEqLmfFreq / mSampleRate); break;
  case kEqLmfQ: mEqLmfQ = GetParam(paramIdx)->Value(); fEqLmfFilter[0].setQ(mEqLmfQ); fEqLmfFilter[1].setQ(mEqLmfQ); break;

  case kEqHmfGain: mEqHmfGain = GetParam(paramIdx)->Value() * mEqAmount; fEqHmfFilter[0].setPeakGain(mEqHmfGain); fEqHmfFilter[1].setPeakGain(mEqHmfGain); break;
  case kEqHmfFreq: mEqHmfFreq = GetParam(paramIdx)->Value(); fEqHmfFilter[0].setFc(mEqHmfFreq / mSampleRate); fEqHmfFilter[1].setFc(mEqHmfFreq / mSampleRate); break;
  case kEqHmfQ: mEqHmfQ = GetParam(paramIdx)->Value(); fEqHmfFilter[0].setQ(mEqHmfQ); fEqHmfFilter[1].setQ(mEqHmfQ); break;

  case kEqHfGain: mEqHfGain = GetParam(paramIdx)->Value() * mEqAmount; fEqHfFilter[0].setPeakGain(mEqHfGain); fEqHfFilter[1].setPeakGain(mEqHfGain); break;
  case kEqHfFreq: mEqHfFreq = GetParam(paramIdx)->Value(); fEqHfFilter[0].setFc(mEqHfFreq / mSampleRate); fEqHfFilter[1].setFc(mEqHfFreq / mSampleRate); break;

  case kEqAmount: mEqAmount = GetParam(paramIdx)->Value() / 100.; OnParamChange(kEqHfGain); OnParamChange(kEqHmfGain); OnParamChange(kEqLmfGain); OnParamChange(kEqLfGain); break;
    //case kEqHfQ: mEqHfQ = GetParam(paramIdx)->Value(); fEqHfFilter[0].setQ(mEqHfQ); fEqHfFilter[1].setQ(mEqHfQ); break;


      // COMPRESSOR
      // ----------

      // Peak Compressor
  case kCompPeakRatio:
    mCompPeakRatio = (1. / GetParam(paramIdx)->Value());
    if (mCompPeakRatio <= 1. / 20.) {
      mCompPeakRatio = 0.;
    }
    fCompressorPeak.setRatio(mCompPeakRatio);
    //mCompPeakAutoMakeup = SR::Utils::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
    break;

  case kCompPeakThresh:
    mCompPeakThresh = GetParam(paramIdx)->Value();
    fCompressorPeak.setThresh(mCompPeakThresh);
    //mCompPeakAutoMakeup = SR::Utils::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
    break;

  case kCompPeakAttack:
    mCompPeakAttack = GetParam(paramIdx)->Value();
    fCompressorPeak.setAttack(mCompPeakAttack);
    //mCompPeakAutoMakeup = SR::Utils::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
    break;

  case kCompPeakRelease:
    mCompPeakRelease = GetParam(paramIdx)->Value();
    fCompressorPeak.setRelease(mCompPeakRelease);
    //mCompPeakAutoMakeup = SR::Utils::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
    break;

  case kCompPeakKneeWidthDb:
    mCompPeakKneeWidthDb = GetParam(paramIdx)->Value();
    fCompressorPeak.setKnee(mCompPeakKneeWidthDb);
    break;

  case kCompPeakSidechainFilterFreq:
    mCompPeakSidechainFilterFreq = GetParam(paramIdx)->Value();
    fCompressorPeak.setSidechainFilterFreq(mCompPeakSidechainFilterFreq / mSampleRate);
    fCompressorRms.setSidechainFilterFreq(mCompPeakSidechainFilterFreq / mSampleRate); // !!! This moves to own switch when RMS sidechain filter is implemented
    break;

  case kCompPeakMakeup: mCompPeakMakeup = DBToAmp(GetParam(paramIdx)->Value()); break;


    // RMS Compressor
  case kCompRmsRatio:
    mCompRmsRatio = (1 / GetParam(paramIdx)->Value());
    fCompressorRms.setRatio(mCompRmsRatio);
    //mCompRmsAutoMakeup = SR::Utils::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsThresh:
    mCompRmsThresh = GetParam(paramIdx)->Value();
    fCompressorRms.setThresh(mCompRmsThresh);
    //mCompRmsAutoMakeup = SR::Utils::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsAttack:
    mCompRmsAttack = GetParam(paramIdx)->Value();
    fCompressorRms.setAttack(mCompRmsAttack);
    //mCompRmsAutoMakeup = SR::Utils::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsRelease:
    mCompRmsRelease = GetParam(paramIdx)->Value();
    fCompressorRms.setRelease(mCompRmsRelease);
    //mCompRmsAutoMakeup = SR::Utils::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsKneeWidthDb: mCompRmsKneeWidthDb = GetParam(paramIdx)->Value(); fCompressorRms.setKnee(mCompRmsKneeWidthDb); break;
  case kCompRmsMakeup: mCompRmsMakeup = DBToAmp(GetParam(paramIdx)->Value()); break;

    // Both Compressors
  case kCompPeakRmsRatio: mCompPeakRmsRatio = GetParam(paramIdx)->Value() / 100; break;
  case kCompDryWet: mCompDryWet = GetParam(paramIdx)->Value() / 100; break;

    // Bool Switches
  case kCompIsParallel: mCompIsParallel = GetParam(paramIdx)->Bool(); break;
  case kCompPeakIsExtSc: mCompPeakIsExtSc = GetParam(paramIdx)->Bool(); break;
  case kCompRmsIsExrSc: mCompRmsIsExtSc = GetParam(paramIdx)->Bool(); break;
  case kCompPeakIsFeedback: mCompPeakIsFeedback = GetParam(paramIdx)->Bool(); fCompressorPeak.setTopologyFeedback(mCompPeakIsFeedback); break;
  case kCompRmsIsFeedback: mCompRmsIsFeedback = GetParam(paramIdx)->Bool(); fCompressorRms.setTopologyFeedback(mCompRmsIsFeedback); break;


    // GLOBAL BYPASS
    // -------------

  case kEqBypass: mEqBypass = GetParam(paramIdx)->Bool(); break;
  case kCompBypass: mCompBypass = GetParam(paramIdx)->Bool(); break;
  case kInputBypass: mInputBypass = GetParam(paramIdx)->Bool(); break;
  case kOutputBypass: mOutputBypass = GetParam(paramIdx)->Bool(); break;
  case kBypass: mBypass = GetParam(paramIdx)->Bool(); break;


    // AUTOMATIC GAIN CONTROL
    // ----------------------

  case kAgc:
    mAgc = GetParam(paramIdx)->Bool();
    //if (mCircularBuffer) {
    //  double sumIn = 0.;
    //  double sumOut = 0.;
    //  double aveIn = 0.;
    //  double aveOut = 0.;
    //  double diffInOut = 0.;
    //  for (int i = 0; i < circularBufferLenght; i++) {
    //    sumIn += fabs(mCircularBuffer[0][i]) + fabs(mCircularBuffer[1][i]);
    //    sumOut += fabs(mCircularBuffer[2][i]) + fabs(mCircularBuffer[3][i]);
    //  }
    //  aveIn = sumIn / (2. * (double(circularBufferLenght)));
    //  aveOut = sumOut / (2. * (double(circularBufferLenght)));
    //  diffInOut = aveIn / aveOut;

    //  SetParameterValue(kOutputGain, GetParam(kOutputGain)->ToNormalized(AmpToDB(diffInOut)));
    //  //GetParam(paramIdx)->Set(0.);
    //  //if (GetUI()) GetUI()->SetAllControlsDirty();
    //  //DirtyParametersFromUI();
    //}
    break;

    // DEESSER
    // -------

  case kDeesserFreq: mDeesserFreq = GetParam(paramIdx)->Value(); fDeesser.setFrequency(mDeesserFreq / mSampleRate); break;
  case kDeesserQ: mDeesserQ = GetParam(paramIdx)->Value(); fDeesser.setQ(mDeesserQ); break;
  case kDeesserThresh: mDeesserThresh = GetParam(paramIdx)->Value(); fDeesser.setThresh(mDeesserThresh); break;
  case kDeesserRatio: mDeesserRatio = (1. / GetParam(paramIdx)->Value()); fDeesser.setRatio(mDeesserRatio); break;
  case kDeesserAttack: mDeesserAttack = GetParam(paramIdx)->Value(); fDeesser.setAttack(mDeesserAttack); break;
  case kDeesserRelease: mDeesserRelease = GetParam(paramIdx)->Value(); fDeesser.setRelease(mDeesserRelease); break;
  case kDeesserMakeup: mDeesserMakeup = DBToAmp(GetParam(paramIdx)->Value()); break;

  default: break;
  }
}

void SRChannel::MakePresets() {
  // Make Presets
  MakeDefaultPreset("Default", 1);
  MakePresetFromNamedParams("Kick", 3,
    kEqLfFreq, 60.,
    kEqLfBell, true,
    kEqLfGain, 5.0);
  MakePresetFromNamedParams("Vocals Female", 62,
    kInputGain, 0.000000,
    kSaturationType, 3,
    kSaturationDrive, 20.434783,
    kSaturationAmount, 26.364001,
    kSaturationHarmonics, 26.811594,
    kSaturationSkew, 0.000000,
    kOversamplingRate, 0,
    kEqHpFreq, 79.560642,
    kEqLpFreq, 22000.000000,
    kEqHpOrder, 1,
    kEqHfGain, 3.043478,
    kEqHfFreq, 10511.467275,
    kEqHfBell, false,
    kEqHmfGain, 1.217391,
    kEqHmfFreq, 4628.704364,
    kEqHmfQ, 0.317899,
    kEqLmfGain, -1.652174,
    kEqLmfFreq, 846.566303,
    kEqLmfQ, 3.456853,
    kEqLfGain, 3.391304,
    kEqLfFreq, 143.126882,
    kEqLfBell, false,
    kEqAmount, 100.000000,
    kCompRmsThresh, -29.855072,
    kCompRmsRatio, 4.978269,
    kCompRmsAttack, 20.000000,
    kCompRmsRelease, 382.138638,
    kCompRmsKneeWidthDb, 10.115096,
    kCompRmsMakeup, 0.000000,
    kCompRmsIsFeedback, false,
    kCompRmsIsExrSc, false,
    kCompPeakThresh, -16.376812,
    kCompPeakRatio, 10.192125,
    kCompPeakAttack, 5.591731,
    kCompPeakRelease, 399.390679,
    kCompPeakKneeWidthDb, 11.654997,
    kCompPeakMakeup, 0.000000,
    kCompPeakSidechainFilterFreq, 156.069760,
    kCompPeakIsFeedback, false,
    kCompPeakIsExtSc, false,
    kCompIsParallel, false,
    kCompPeakRmsRatio, 50.000000,
    kCompDryWet, 82.246377,
    kDeesserFreq, 6024.196487,
    kDeesserQ, 2.164105,
    kDeesserThresh, -30.072464,
    kDeesserRatio, 7.050087,
    kDeesserAttack, 0.741868,
    kDeesserRelease, 16.422551,
    kDeesserMakeup, 0.000000,
    kPan, 0.000000,
    kPanFreq, 161.288652,
    kIsPanMonoLow, true,
    kLimiterThresh, 10.000000,
    kClipperThreshold, 0.000000,
    kAgc, false,
    kOutputGain, -6.720000,
    kInputBypass, false,
    kEqBypass, false,
    kCompBypass, false,
    kOutputBypass, false,
    kBypass, false);
}
#endif
