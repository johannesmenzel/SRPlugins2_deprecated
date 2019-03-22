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

#pragma mark - Plugin connector

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
  //// Name channels:
  // // for VST2 we name individual outputs
  //if (GetAPI() == kAPIVST2) {

    //SetChannelLabel(ERoute::kInput, 0, "In 1", false);
    //SetChannelLabel(ERoute::kInput, 1, "In 2", false);
    //SetChannelLabel(ERoute::kInput, 2, "SC 1", false);
    //SetChannelLabel(ERoute::kInput, 3, "SC 2", false);
    //SetChannelLabel(ERoute::kOutput, 0, "Out 1", false);
    //SetChannelLabel(ERoute::kOutput, 1, "Out 2", false);

  //  //SetChannelLabel(ERoute::kOutput, 2, "SC Out 1", true);
  //  //SetChannelLabel(ERoute::kOutput, 3, "SC Out 2", true);

  //}
  //else {
  //  //GetIOConfig(0)->GetBusInfo(ERoute::kInput, 0)->mLabel.Set("In", MAX_CHAN_NAME_LEN);
  //  //GetIOConfig(0)->GetBusInfo(ERoute::kOutput, 0)->mLabel.Set("Out", MAX_CHAN_NAME_LEN);
  //  GetIOConfig(0)->GetBusInfo(ERoute::kInput, 0)->mLabel.Set("In", MAX_CHAN_NAME_LEN);
  //  GetIOConfig(0)->GetBusInfo(ERoute::kInput, 1)->mLabel.Set("SC", MAX_CHAN_NAME_LEN);
  //  GetIOConfig(0)->GetBusInfo(ERoute::kOutput, 0)->mLabel.Set("Out", MAX_CHAN_NAME_LEN);
  //  //GetIOConfig(0)->GetBusInfo(ERoute::kOutput, 1)->mLabel.Set("SC Out", MAX_CHAN_NAME_LEN);
  //}


  // Initialize Parameters:
  for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {
    const SRParamProperties &p = srchannelParamProperties[paramIdx];		// ... and a variable "p" pointing at the current parameters p
    IParam* thisParameter = GetParam(paramIdx);												          // ... for which we temporally create a pointer "param"
    switch (p.Type)
    {
    case IParam::EParamType::kTypeDouble:
      thisParameter->InitDouble(p.name, p.defaultVal, p.minVal, p.maxVal, p.stepValue, p.unit, p.Flags, p.group, ShapeFromMiddle(p.minVal, p.maxVal, p.centerVal, p.centerPoint), IParam::kUnitCustom);
      break;
    case IParam::EParamType::kTypeInt:
      thisParameter->InitInt(p.name, (int)p.defaultVal, (int)p.minVal, (int)p.maxVal, p.unit, 0, p.group);
      break;
    case IParam::EParamType::kTypeBool:
      thisParameter->InitBool(p.name, (bool)p.defaultVal, p.unit, 0, p.group, p.labelMin, p.labelMax);
      break;
    case IParam::EParamType::kTypeEnum:
      switch (paramIdx) {
      case kEqHpOrder: /*case kEqLpOrder:*/
        thisParameter->InitEnum(
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
        thisParameter->InitEnum(p.name,
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
        thisParameter->InitEnum(
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
    case kLimiterThresh: thisParameter->SetDisplayText((int)p.maxVal, "Off"); break;
    case kEqHpFreq: case kCompPeakSidechainFilterFreq: thisParameter->SetDisplayText((int)p.minVal, "Off"); break;
    case kEqLpFreq: thisParameter->SetDisplayText((int)p.maxVal, "Off"); break;
    case kCompPeakRatio: thisParameter->SetDisplayText((int)p.maxVal, "inf"); break;
    case kPanFreq: thisParameter->SetDisplayText((int)p.minVal, "Off"); break;
    default: break;
    }

    // To set default parameter member variable:
    OnParamChange(paramIdx);
  }

  // Why?
  OnReset();

  // Make presets:
  MakePresets();


  // GRAPHICS functions:
#if IPLUG_EDITOR

  // Create graphics context:
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  // Create layout:
  mLayoutFunc = [&](IGraphics* pGraphics) {

    // Make RECTs:
    const IRECT rectPlugin = pGraphics->GetBounds();      // Plug canvas
    const IRECT rectHeader = rectPlugin.GetFromTop(80.f);
    const IRECT rectFooter = rectPlugin.GetFromBottom(30.f);
    const IRECT rectControls = rectPlugin.GetReducedFromTop(70.f).GetReducedFromBottom(30.f); // Control canvas
    // Make section RECTs:
    const IRECT rectInput = rectControls.SubRectHorizontal(6, 0).GetPadded(-5.f);
    const IRECT rectEq = rectControls.SubRectHorizontal(6, 1).GetPadded(-5.f);
    const IRECT rectComp = rectControls.SubRectHorizontal(6, 2).GetPadded(-5.f);
    const IRECT rectPost = rectControls.SubRectHorizontal(6, 3).GetPadded(-5.f);
    const IRECT rectOutput = rectControls.SubRectHorizontal(6, 4).GetPadded(-5.f);
    const IRECT rectMeter = rectControls.SubRectHorizontal(6, 5).GetPadded(-5.f);

    // Make PATTERNS:
    const IPattern patternPanel = IPattern::CreateLinearGradient(rectControls.L, rectControls.T, rectControls.R, rectControls.B, { IColorStop(SRLayout.colorPanelBG, 0.2f), IColorStop(COLOR_BLACK, 0.5f) });

    // Resize loop:
    if (pGraphics->NControls()) {

      // Rezize BACKGROUND:
      pGraphics->GetBackgroundControl()->SetTargetAndDrawRECTs(rectPlugin);
      // Resize LOGO and VERSION STRING:
      pGraphics->GetControlWithTag(cSRPluginsLogo)->SetTargetAndDrawRECTs(rectHeader.SubRectVertical(2, 0).GetFromLeft(pGraphics->GetControlWithTag(cSRPluginsLogo)->GetRECT().W()));
      pGraphics->GetControlWithTag(cSRChannelLogo)->SetTargetAndDrawRECTs(rectHeader.SubRectVertical(2, 0).GetFromRight(pGraphics->GetControlWithTag(cSRChannelLogo)->GetRECT().W()));
      //pGraphics->GetControlWithTag(cVersion)->SetTargetAndDrawRECTs(pGraphics->GetControlWithTag(cBitmapLogo)->GetRECT().GetFromTop(SRLayout.textKnobLabel.mSize));

      // Resize section rect PANELS and patterns:
      pGraphics->GetControlWithTag(cPanelInput)->SetTargetAndDrawRECTs(rectInput);
      dynamic_cast<SR::Graphics::SRPanel*>(pGraphics->GetControlWithTag(cPanelInput))->SetPattern(patternPanel);
      pGraphics->GetControlWithTag(cPanelEq)->SetTargetAndDrawRECTs(rectEq);
      dynamic_cast<SR::Graphics::SRPanel*>(pGraphics->GetControlWithTag(cPanelEq))->SetPattern(patternPanel);
      pGraphics->GetControlWithTag(cPanelComp)->SetTargetAndDrawRECTs(rectComp);
      dynamic_cast<SR::Graphics::SRPanel*>(pGraphics->GetControlWithTag(cPanelComp))->SetPattern(patternPanel);
      pGraphics->GetControlWithTag(cPanelPost)->SetTargetAndDrawRECTs(rectPost);
      dynamic_cast<SR::Graphics::SRPanel*>(pGraphics->GetControlWithTag(cPanelPost))->SetPattern(patternPanel);
      pGraphics->GetControlWithTag(cPanelOutput)->SetTargetAndDrawRECTs(rectOutput);
      dynamic_cast<SR::Graphics::SRPanel*>(pGraphics->GetControlWithTag(cPanelOutput))->SetPattern(patternPanel);
      pGraphics->GetControlWithTag(cPanelMeter)->SetTargetAndDrawRECTs(rectMeter);
      dynamic_cast<SR::Graphics::SRPanel*>(pGraphics->GetControlWithTag(cPanelMeter))->SetPattern(patternPanel);

      // Resize METERS:
      pGraphics->GetControlWithTag(cInputMeter)->SetTargetAndDrawRECTs(rectMeter.SubRectHorizontal(3, 0));
      pGraphics->GetControlWithTag(cGrMeter)->SetTargetAndDrawRECTs(rectMeter.SubRectHorizontal(3, 1));
      pGraphics->GetControlWithTag(cOutputMeter)->SetTargetAndDrawRECTs(rectMeter.SubRectHorizontal(3, 2));
      pGraphics->GetControlWithTag(cScope)->SetTargetAndDrawRECTs(rectHeader.SubRectHorizontal(6, 0).GetVPadded(-5.f));
      pGraphics->GetControlWithTag(cFreqMeter)->SetTargetAndDrawRECTs(rectHeader.SubRectHorizontal(6, 1).GetVPadded(-5.f));

      // Resize PRESET menu:
      pGraphics->GetControlWithTag(cPresetMenu)->SetTargetAndDrawRECTs(rectHeader.SubRectHorizontal(6, 2).SubRectVertical(4, 1).FracRectVertical(2.f, true).GetPadded(-5.f));

      // Resize: Loop through parameters:
      for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {

        const IRECT* rect;
        const SRParamProperties& p = srchannelParamProperties[paramIdx];		// ... and a variable "p" pointing at the current parameters p
        const int ctrlIdx = p.ctrlTag;
        IControl* thisControl = pGraphics->GetControlWithTag(ctrlIdx);												          // ... for which we temporally create a pointer "thisControl"

        // Resize: Choose RECT for parameter control:
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

        // Place parameter controls here:
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
            thisControl->SetTargetAndDrawRECTs(rectCurrentControl); break;

          default:
            // Resize knobs
            thisControl->SetTargetAndDrawRECTs(rectCurrentControl); break;
          }

          break;

        case IParam::EParamType::kTypeEnum:
        case IParam::EParamType::kTypeBool:
          // Resize switches
          thisControl->SetTargetAndDrawRECTs(rectCurrentControl); break;

        default:
          break;

        }

      }

      // If resizing finished:
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

    // Attach logos and version string:
    pGraphics->AttachControl(new IBitmapControl(rectHeader.SubRectVertical(2, 0).GetFromLeft(float(bmpSRPluginsLogo.W())), bmpSRPluginsLogo, -1, EBlendType::kBlendNone), cSRPluginsLogo, "UI");
    pGraphics->AttachControl(new IBitmapControl(rectHeader.SubRectVertical(2, 0).GetFromRight(float(bmpSRChannelLogo.W())), bmpSRChannelLogo, -1, EBlendType::kBlendNone), cSRChannelLogo, "UI");
    //pGraphics->AttachControl(new ITextControl(pGraphics->GetControlWithTag(cBitmapLogo)->GetRECT().GetFromTop(SRLayout.textKnobLabel.mSize), "v" PLUG_VERSION_STR"-a", SRLayout.textVersionString), cVersion, "UI");

    // Attach section rect PANELS
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectInput, patternPanel, true), cPanelInput, "UI");
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectEq, patternPanel, true), cPanelEq, "UI");
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectComp, patternPanel, true), cPanelComp, "UI");
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectPost, patternPanel, true), cPanelPost, "UI");
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectOutput, patternPanel, true), cPanelOutput, "UI");
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectMeter, patternPanel, true), cPanelMeter, "UI");


    // Attach meters
    pGraphics->AttachControl(new SR::Graphics::SRMeter<2, 1024>(rectMeter.SubRectHorizontal(3, 0), false, false, -60.f, 12.f, (float)SR::Utils::SetShapeCentered(-60., 12., 0., .75), 1, 6, "In Left", "In Right"), cInputMeter, "Meter");
    pGraphics->AttachControl(new SR::Graphics::SRMeter<3, 1024>(rectMeter.SubRectHorizontal(3, 1), true, true, -18.f, 0.f, (float)SR::Utils::SetShapeCentered(-18., 0., -9., .5), 1, 3, "GR RMS", "GR Peak", "GR Deesser"), cGrMeter, "Meter");
    pGraphics->AttachControl(new SR::Graphics::SRMeter<2, 1024>(rectMeter.SubRectHorizontal(3, 2), false, false, -60.f, 12.f, (float)SR::Utils::SetShapeCentered(-60., 12., 0., .75), 1, 6, "Out Left", "Out Right"), cOutputMeter, "Meter");
    pGraphics->AttachControl(new IVScopeControl<2>(rectHeader.SubRectHorizontal(6, 0).GetVPadded(-5.f), "Left", "Right"), cScope, "Meter");
    //pGraphics->AttachControl(new SR::Graphics::SRFrequencyResponseMeter(rectHeader.SubRectHorizontal(6, 1).GetVPadded(-5.f), FREQUENCYRESPONSE, mFreqMeterValues, SR::Utils::SetShapeCentered(0., 22000., 1000., .5), SRLayout.colorSpec), cFreqMeter, "Meter");
    pGraphics->AttachControl(new SR::Graphics::SRGraphBase(rectHeader.SubRectHorizontal(6, 1).GetVPadded(-5.f), FREQUENCYRESPONSE, mFreqMeterValues, SRLayout.colorSpec), cFreqMeter, "Meter");

    // Attach preset menu:
    pGraphics->AttachControl(new SR::Graphics::SRPresetMenu(this, rectHeader.SubRectHorizontal(6, 2).SubRectVertical(4, 1).FracRectVertical(2.f, true).GetPadded(-5.f), SRLayout.textPresetMenu, namedParams), cPresetMenu, "UI");

    // Set Tooltip for non-parameter controls:
    pGraphics->GetControlWithTag(cInputMeter)->SetTooltip("Input peak meter for left and right channel");
    pGraphics->GetControlWithTag(cGrMeter)->SetTooltip("Gain reduction meter for RMS, peak and deessing compressors");
    pGraphics->GetControlWithTag(cOutputMeter)->SetTooltip("Output peak meter for left and right channel");
    pGraphics->GetControlWithTag(cScope)->SetTooltip("Scope for left and right channel");
    pGraphics->GetControlWithTag(cFreqMeter)->SetTooltip("Frequency response of EQ");

    // Loop through parameters:
    for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {
      const IRECT* rect;
      const SRParamProperties &p = srchannelParamProperties[paramIdx];		// ... and a variable "p" pointing at the current parameters p
      const int ctrlIdx = p.ctrlTag;


      // Choose RECT for parameter control:
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

      // Place parameter controls here:
      const IRECT rectCurrentControl = rect->GetGridCell(p.position.y, p.position.x, sectionRectGridCells[p.position.AttachToControlPanel][0], sectionRectGridCells[p.position.AttachToControlPanel][1]).FracRectHorizontal(p.position.w, false).FracRectVertical(p.position.h, true).GetPadded((p.Knobs == Button) ? -5.0f : 0.0f);

      IColor knobColor;               // "knobColor" gets what IColor is stated in the param struct
      switch (p.Knobs) {
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
          // Attach imput and output faders
          pGraphics->AttachControl(new IVSliderControl(rectCurrentControl, paramIdx, SRLayout.colorSpec, kVertical, true, 32.f, 2.f), ctrlIdx);
          break;

        default:
          // Attach knobs for all other double
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
            //(float)GetParam(paramIdx)->GetDefault(true),
            0.5f,
            kVertical,
            4.f
          ), ctrlIdx);
          break;
        }
        break;

      case IParam::EParamType::kTypeEnum:
      case IParam::EParamType::kTypeBool:
        // Attach switches for bool and enum
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

      // Other setup for EACH parameter control
      pGraphics->GetControlWithTag(ctrlIdx)->SetMOWhenGrayed(true);
      pGraphics->GetControlWithTag(ctrlIdx)->SetTooltip(p.tooltip);
    }

    // Other setup for ALL parameter controls
    pGraphics->StyleAllVectorControls(true, true, false, 0.1f, 2.f, 3.f, SRLayout.colorSpec);

  }; // END LAYOUT function

}// END GRAPHICS functions

#pragma mark - User methods

  // Method to gray controls, which are currently bypassed. Thats why you have to test from top to bottom
void SRChannel::GrayOutControls() {
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
        //: (paramIdx == kEqLpOrder ? true
        : false;

      GetUI()->GetControlWithTag(p.ctrlTag)->GrayOut(grayout);
    }
  }
}

void SRChannel::SetFreqMeterValues() {
  const double shape = log10(mSampleRate);
  for (int i = 0; i < FREQUENCYRESPONSE; i++) {
    // If linear shape
    //double freq = 0.5 * mSampleRate * double(i) / double(FREQUENCYRESPONSE);
    // If pow shape
    double freq = std::pow((double(i) / double(FREQUENCYRESPONSE)), shape) * (0.5 * mSampleRate);
    mFreqMeterValues[i] = 0.;
    if (mEqHpFreq > 16.) {
      mFreqMeterValues[i] += fFilters[EFilters::kHp1].GetFrequencyResponse(freq / mSampleRate, 12., false);
      mFreqMeterValues[i] += fFilters[EFilters::kHp2].GetFrequencyResponse(freq / mSampleRate, 12., false);
      mFreqMeterValues[i] += fFilters[EFilters::kHp3].GetFrequencyResponse(freq / mSampleRate, 12., false);
      mFreqMeterValues[i] += fFilters[EFilters::kHp4].GetFrequencyResponse(freq / mSampleRate, 12., false);
      mFreqMeterValues[i] += fFilters[EFilters::kHp5].GetFrequencyResponse(freq / mSampleRate, 12., false);
      mFreqMeterValues[i] += fFilters[EFilters::kHp6].GetFrequencyResponse(freq / mSampleRate, 12., false);
      mFreqMeterValues[i] += fFilters[EFilters::kHp7].GetFrequencyResponse(freq / mSampleRate, 12., false);
      mFreqMeterValues[i] += fFilters[EFilters::kHp8].GetFrequencyResponse(freq / mSampleRate, 12., false);
      mFreqMeterValues[i] += fFilters[EFilters::kHp9].GetFrequencyResponse(freq / mSampleRate, 12., false);
      mFreqMeterValues[i] += fFilters[EFilters::kHp10].GetFrequencyResponse(freq / mSampleRate, 12., false);
    }
    if (mEqLfGain != 0.0) mFreqMeterValues[i] += fFilters[EFilters::kLf].GetFrequencyResponse(freq / mSampleRate, 12., false);
    if (mEqLmfGain != 0.0) mFreqMeterValues[i] += fFilters[EFilters::kLmf].GetFrequencyResponse(freq / mSampleRate, 12., false);
    if (mEqHmfGain != 0.0) mFreqMeterValues[i] += fFilters[EFilters::kHmf].GetFrequencyResponse(freq / mSampleRate, 12., false);
    if (mEqHfGain != 0.0) mFreqMeterValues[i] += fFilters[EFilters::kHf].GetFrequencyResponse(freq / mSampleRate, 12., false);
    if (mEqLpFreq < 22000.) mFreqMeterValues[i] += fFilters[EFilters::kLp].GetFrequencyResponse(freq / mSampleRate, 12., false);
    if (mDeesserThresh < 0.) mFreqMeterValues[i] += fDeesser.fDynamicEqFilter.GetFrequencyResponse(freq / mSampleRate, 12., false);
  }
  //if (GetUI()) dynamic_cast<SR::Graphics::SRFrequencyResponseMeter*>(GetUI()->GetControlWithTag(cFreqMeter))->Process(mFreqMeterValues);
  if (GetUI()) dynamic_cast<SR::Graphics::SRGraphBase*>(GetUI()->GetControlWithTag(cFreqMeter))->Process(mFreqMeterValues);
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

  case kEqHpFreq:
  case kEqHpOrder:
  case kEqLpFreq:
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

#pragma mark - User init methods

// DSP func
#if IPLUG_DSP

void SRChannel::InitEffects() {

  // Init gain and pan
  fInputGain.InitGain(int(mSampleRate * 0.1));
  fOutputGain.InitGain(int(mSampleRate * 0.1));
  fAutoGain.InitGain(int(mSampleRate * 0.1));
  fPan.InitGain(int(mSampleRate * 0.1), SR::DSP::SRGain::PanType::kSinusodial);

  for (int i = 0; i < mNumOutChannels; i++) {
    // Init EQ
    fFilters[EFilters::kOpHp].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::OnepoleHighpass, mEqHpFreq / mSampleRate, 0.0, 0.0, mSampleRate);
    fFilters[EFilters::kHp1].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fFilters[EFilters::kHp2].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fFilters[EFilters::kHp3].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fFilters[EFilters::kHp4].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fFilters[EFilters::kHp5].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fFilters[EFilters::kHp6].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fFilters[EFilters::kHp7].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fFilters[EFilters::kHp8].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fFilters[EFilters::kHp9].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fFilters[EFilters::kHp10].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
    fFilters[EFilters::kLp].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadLowpass, mEqLpFreq / mSampleRate, stQ, 0., mSampleRate);
    fFilters[EFilters::kHf].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighshelf, mEqHfFreq / mSampleRate, mEqHfQ, mEqHfGain, mSampleRate);
    fFilters[EFilters::kHmf].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadPeak, mEqHmfFreq / mSampleRate, mEqHmfQ, mEqHmfGain, mSampleRate);
    fFilters[EFilters::kLmf].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadPeak, mEqLmfFreq / mSampleRate, mEqLmfQ, mEqLmfGain, mSampleRate);
    fFilters[EFilters::kLf].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadLowshelf, mEqLfFreq / mSampleRate, mEqLfQ, mEqLfGain, mSampleRate);
    fFilters[EFilters::kDC].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::OnepoleHighpass, 10.0 / mSampleRate, 0.0, 0.0, mSampleRate);

    // Init safe pan filter
    fFilters[EFilters::kPanHp].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadLinkwitzHighpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);
    fFilters[EFilters::kPanLp].SetFilter(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadLinkwitzLowpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);

    // Init saturation
    fInputSaturation[i].setSaturation(SR::DSP::SRSaturation::SaturationTypes::typeMusicDSP, mSaturationDrive, mSaturationAmount, mSaturationHarmonics, false, mSaturationSkew, 1., mSampleRate);

    //... Commented out until implementation of oversampling
    //fOverSamplerProcessL = std::bind(&SR::DSP::SRSaturation::process, fInputSaturation[0].process, std::placeholders::_1);
    //fOverSamplerProcessR = std::bind(&SR::DSP::SRSaturation::process, fInputSaturation[1].process, std::placeholders::_1);

    // Oversampling
    mOverSampler[i].Reset();
  }

  // Init compressor
  fCompressorPeak.InitCompressor(mCompPeakThresh, mCompPeakRatio, mCompPeakAttack, mCompPeakRelease, mCompPeakSidechainFilterFreq, mCompPeakKneeWidthDb, mCompPeakIsFeedback, false, mSampleRate);
  fCompressorPeak.Reset();

  // For sidechain filter frequency it requires an own knob later
  fCompressorRms.InitCompressor(mCompRmsThresh, mCompRmsRatio, mCompRmsAttack, mCompRmsRelease, mCompPeakSidechainFilterFreq, mCompRmsKneeWidthDb, 300., mCompRmsIsFeedback, false, mSampleRate);
  fCompressorRms.Reset();

  // Init limiter
  fLimiter.SetSampleRate(mSampleRate);
  fLimiter.SetAttack(1.);
  fLimiter.SetRelease(100.);
  fLimiter.SetThresh(mLimiterThresh);
  fLimiter.Reset();

  // Init deesser
  fDeesser.SetDeesser(mDeesserThresh, mDeesserRatio, mDeesserAttack, mDeesserRelease, mDeesserFreq / mSampleRate, mDeesserQ, 10., mSampleRate);
  fDeesser.Reset();

  // Meter
#if USEBUFFER == 2
  bInputMeter.ResetBuffer(2, GetBlockSize());
  bGrMeter.ResetBuffer(3, GetBlockSize());
  bOutputMeter.ResetBuffer(2, GetBlockSize());
#endif // USEBUFFER
}

#pragma mark - Process block

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

#if USEBUFFER == 1
  mInMeterValues = new sample*[2];
  for (int ch = 0; ch < 2; ch++) {
    mInMeterValues[ch] = new sample[nFrames];
  }
  mOutMeterValues = new sample*[2];
  for (int ch = 0; ch < 2; ch++) {
    mOutMeterValues[ch] = new sample[nFrames];
  }
  mGrMeterValues = new sample*[3];
  for (int ch = 0; ch < 3; ch++) {
    mGrMeterValues[ch] = new sample[nFrames];
  }
#elif USEBUFFER == 2
  bInputMeter.SetNumFrames(nFrames);
  bOutputMeter.SetNumFrames(nFrames);
  bGrMeter.SetNumFrames(nFrames);
#endif



  // Begin Processing per Frame

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++sc1, ++sc2, ++out1, ++out2) {

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
      fInputGain.Process(*out1, *out2);

      // Fill input meter values
#if USEBUFFER == 1
      mInMeterValues[0][s] = *out1;
      mInMeterValues[1][s] = *out2;
#elif USEBUFFER == 2
      bInputMeter.ProcessBuffer(*out1, 0, s);
      bInputMeter.ProcessBuffer(*out2, 1, s);
#endif


      // ----------------
      // End Pre Sections



      // INPUT SECTION
      // -------------
      if (mInputBypass != 1) {

        // Saturation
        if (mSaturationAmount != 0.) {
          if (mOversamplingRate == OverSampler<sample>::EFactor::kNone) {
            // Non oversampled saturation processing
            *out1 = fInputSaturation[0].process(*out1);
            *out2 = fInputSaturation[1].process(*out2);
          }
          else {
            // oversampled saturation processing
            *out1 = mOverSampler[0].Process(*out1, std::bind(&SR::DSP::SRSaturation::process, fInputSaturation[0], std::placeholders::_1));
            *out2 = mOverSampler[1].Process(*out2, std::bind(&SR::DSP::SRSaturation::process, fInputSaturation[1], std::placeholders::_1));
          }


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
            *out1 = fFilters[EFilters::kOpHp].Process(*out1, 0);
            *out2 = fFilters[EFilters::kOpHp].Process(*out2, 1);
          }
          // two pole for 2nd order
          if (mEqHpOrder >= EFilterSlope::dbo12) {
            *out1 = fFilters[EFilters::kHp1].Process(*out1, 0);
            *out2 = fFilters[EFilters::kHp1].Process(*out2, 1);
          }
          // two pole for 4th order
          if (mEqHpOrder >= EFilterSlope::dbo24) {
            *out1 = fFilters[EFilters::kHp2].Process(*out1, 0);
            *out2 = fFilters[EFilters::kHp2].Process(*out2, 1);
          }
          // two pole for 6th order
          if (mEqHpOrder >= EFilterSlope::dbo36) {
            *out1 = fFilters[EFilters::kHp3].Process(*out1, 0);
            *out2 = fFilters[EFilters::kHp3].Process(*out2, 1);
          }
          // two pole for 8th order
          if (mEqHpOrder >= EFilterSlope::dbo48) {
            *out1 = fFilters[EFilters::kHp4].Process(*out1, 0);
            *out2 = fFilters[EFilters::kHp4].Process(*out2, 1);
          }
          // two pole for 10th order
          if (mEqHpOrder >= EFilterSlope::dbo60) {
            *out1 = fFilters[EFilters::kHp5].Process(*out1, 0);
            *out2 = fFilters[EFilters::kHp5].Process(*out2, 1);
          }
          // two pole for 12th order
          if (mEqHpOrder >= EFilterSlope::dbo72) {
            *out1 = fFilters[EFilters::kHp6].Process(*out1, 0);
            *out2 = fFilters[EFilters::kHp6].Process(*out2, 1);
          }
          // two pole for 20th order
          if (mEqHpOrder >= EFilterSlope::dbo120) {
            *out1 = fFilters[EFilters::kHp7].Process(*out1, 0);
            *out2 = fFilters[EFilters::kHp7].Process(*out2, 1);
            *out1 = fFilters[EFilters::kHp8].Process(*out1, 0);
            *out2 = fFilters[EFilters::kHp8].Process(*out2, 1);
            *out1 = fFilters[EFilters::kHp9].Process(*out1, 0);
            *out2 = fFilters[EFilters::kHp9].Process(*out2, 1);
            *out1 = fFilters[EFilters::kHp10].Process(*out1, 0);
            *out2 = fFilters[EFilters::kHp10].Process(*out2, 1);
          }
        }

        // Low Pass

        if (mEqLpFreq < 22000.0) {
          *out1 = fFilters[EFilters::kLp].Process(*out1, 0);
          *out2 = fFilters[EFilters::kLp].Process(*out2, 1);
        }

        // Parametric EQ

        if (mEqLfGain != 0.0) {
          *out1 = fFilters[EFilters::kLf].Process(*out1, 0);
          *out2 = fFilters[EFilters::kLf].Process(*out2, 1);
        }
        if (mEqLmfGain != 0.0) {
          *out1 = fFilters[EFilters::kLmf].Process(*out1, 0);
          *out2 = fFilters[EFilters::kLmf].Process(*out2, 1);
        }
        if (mEqHmfGain != 0.0) {
          *out1 = fFilters[EFilters::kHmf].Process(*out1, 0);
          *out2 = fFilters[EFilters::kHmf].Process(*out2, 1);
        }
        if (mEqHfGain != 0.0) {
          *out1 = fFilters[EFilters::kHf].Process(*out1, 0);
          *out2 = fFilters[EFilters::kHf].Process(*out2, 1);
        }
      }
      // --------------
      // End EQ Section



      // COMPRESSOR SECTION
      // ------------------

      if (mCompBypass != 1) {

        // DEESSER
        if (mDeesserRatio != 1.0 && mDeesserThresh != 0.0)
          fDeesser.Process(*out1, *out2);

        // COMPRESSOR
        // Create dry sample first
        sample vCompDry[2] = { *out1, *out2 };

        // Process parallel compression
        if (mCompIsParallel) {

          // Create samples to process of each compressor
          sample vCompRmsIn[2] = { *out1, *out2 };
          sample vCompPeakIn[2] = { *out1, *out2 };

          if (mCompRmsRatio != 1. && mCompRmsThresh != 0.) {
            if (!mCompRmsIsExtSc)
              fCompressorRms.Process(vCompRmsIn[0], vCompRmsIn[1]);
            else
              fCompressorRms.Process(vCompRmsIn[0], vCompRmsIn[1], *sc1, *sc2);
          }

          if (mCompPeakRatio != 1. && mCompPeakThresh != 0.) {
            if (!mCompPeakIsExtSc)
              fCompressorPeak.Process(vCompPeakIn[0], vCompPeakIn[1]);
            else
              fCompressorPeak.Process(vCompPeakIn[0], vCompPeakIn[1], *sc1, *sc2);
          }

          if ((mCompRmsRatio != 1. && mCompRmsThresh != 0.) || (mCompPeakRatio != 1. && mCompPeakThresh != 0.)) {
            *out1 = (1. - mCompPeakRmsRatio) * vCompRmsIn[0] + mCompPeakRmsRatio * vCompPeakIn[0];
            *out2 = (1. - mCompPeakRmsRatio) * vCompRmsIn[1] + mCompPeakRmsRatio * vCompPeakIn[1];
          }
        }
        else {
          // Process SERIAL RMS compression
          if (mCompRmsRatio != 1. && mCompRmsThresh != 0.) {
            if (!mCompRmsIsExtSc)
              fCompressorRms.Process(*out1, *out2);
            else
              fCompressorRms.Process(*out1, *out2, *sc1, *sc2);
          }
          // Process SERIAL Peak compression
          if (mCompPeakRatio != 1. && mCompPeakThresh != 0.) {
            if (!mCompPeakIsExtSc)
              fCompressorPeak.Process(*out1, *out2);
            else
              fCompressorPeak.Process(*out1, *out2, *sc1, *sc2);
          }
        }

        // Apply compressor's Dry/Wet afterwards
        if (mCompDryWet != 1.) {
          *out1 = (mCompDryWet * *out1) + (vCompDry[0] * (1. - mCompDryWet));
          *out2 = (mCompDryWet * *out2) + (vCompDry[1] * (1. - mCompDryWet));
        }

      }
      // ----------------------
      // End Compressor Section



      // OUTPUT SECTION
      // --------------

      if (mOutputBypass != 1) {

        // Pan

        if (mSafePanFreq > 16.) {
          // Multiply signal...
          sample vSafePanLowSignal[MAXNUMOUTCHANNELS] = { *out1, *out2 };
          sample vSafePanHighSignal[MAXNUMOUTCHANNELS] = { *out1, *out2 };

          // Filter low and high split signal:
          vSafePanLowSignal[0] = fFilters[EFilters::kPanLp].Process(vSafePanLowSignal[0], 0);
          vSafePanLowSignal[1] = fFilters[EFilters::kPanLp].Process(vSafePanLowSignal[1], 1);
          vSafePanHighSignal[0] = fFilters[EFilters::kPanHp].Process(vSafePanHighSignal[0], 0);
          vSafePanHighSignal[1] = fFilters[EFilters::kPanHp].Process(vSafePanHighSignal[1], 1);

          // Pan high signal:
          fPan.Process(vSafePanHighSignal[0], vSafePanHighSignal[1]);

          // Mono low signal:
          if (mIsPanMonoLow) {
            vSafePanLowSignal[0] = (vSafePanLowSignal[0] + vSafePanLowSignal[1]) * 0.5;
            vSafePanLowSignal[1] = (vSafePanLowSignal[0] + vSafePanLowSignal[1]) * 0.5;
          }

          // Apply mixed signals (flip phase of one):
          // TODO: Allpass instead of flipped phase
          *out1 = vSafePanHighSignal[0] - vSafePanLowSignal[0];
          *out2 = vSafePanHighSignal[1] - vSafePanLowSignal[1];
        }
        else {
          fPan.Process(*out1, *out2);
        }
        // Clipper

        if (mClipperThreshold < 1.) {
          *out1 = (fmin(fabs(*out1), mClipperThreshold) / sqrt(mClipperThreshold)) * (fabs(*out1) / *out1);
          *out2 = (fmin(fabs(*out2), mClipperThreshold) / sqrt(mClipperThreshold)) * (fabs(*out2) / *out2);
        }


        // Limiter

        if (mLimiterThresh != 10.) {
          fLimiter.Process(*out1, *out2);
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



      // Output Gain
      fOutputGain.Process(*out1, *out2);
      if (mAgc)
        fAutoGain.Process(*out1, *out2);

      *out1 = fFilters[EFilters::kDC].Process(*out1, 0);
      *out2 = fFilters[EFilters::kDC].Process(*out2, 1);
      *out1 -= dcoff;
      *out2 -= dcoff;

      // ----------------
      // End Post Section


    }
    // -------------------------
    // End of global bypass test

#if USEBUFFER == 1
    mOutMeterValues[0][s] = *out1;
    mOutMeterValues[1][s] = *out2;
    mGrMeterValues[0][s] = fCompressorPeak.getGrLin();
    mGrMeterValues[1][s] = fCompressorRms.getGrLin();
    mGrMeterValues[2][s] = fDeesser.getGrLin();
#elif USEBUFFER == 2
    bOutputMeter.ProcessBuffer(*out1, 0, s);
    bOutputMeter.ProcessBuffer(*out2, 1, s);
    bGrMeter.ProcessBuffer(fCompressorPeak.GetGrLin(), 0, s);
    bGrMeter.ProcessBuffer(fCompressorRms.GetGrLin(), 1, s);
    bGrMeter.ProcessBuffer(fDeesser.GetGrLin(), 2, s);
#endif

  }
  // ----------------------------------------------------------------
  // End processing per frame and again some processing per Framesize


#ifdef USEAGC
  if (mAgc && mAgcTrigger) {
#if USEBUFFER == 1
    double sumIn = 0.; double sumOut = 0.;
    for (int s = 0; s < nFrames; s++) {
      sumIn += std::fabs(mInMeterValues[0][s]) + std::fabs(mInMeterValues[1][s]);
      sumOut += std::fabs(mOutMeterValues[0][s]) + std::fabs(mOutMeterValues[1][s]);
    }
    const double diff = sumIn / sumOut;
#elif USEBUFFER == 2
    const double diff = bInputMeter.AverageBuffer() / bOutputMeter.AverageBuffer();
#endif
    if (mAgc && diff < 8) fAutoGain.SetGain(diff);
    mAgcTrigger = false;
  }
#endif

#if USEBUFFER == 1
  mInputMeterBallistics.ProcessBlock(mInMeterValues, nFrames);
  mGrMeterBallistics.ProcessBlock(mGrMeterValues, nFrames);
  mOutputMeterBallistics.ProcessBlock(mOutMeterValues, nFrames);
  mScopeBallistics.ProcessBlock(mOutMeterValues, nFrames);
  for (int ch = 0; ch < 2; ch++) {
    delete[] mInMeterValues[ch];
  }
  delete[] mInMeterValues;

  for (int ch = 0; ch < 2; ch++) {
    delete[] mOutMeterValues[ch];
  }
  delete[] mOutMeterValues;

  for (int ch = 0; ch < 3; ch++) {
    delete[] mGrMeterValues[ch];
  }
  delete[] mGrMeterValues;
#elif USEBUFFER == 2
  mInputMeterBallistics.ProcessBlock(bInputMeter.GetBuffer(), nFrames);
  mOutputMeterBallistics.ProcessBlock(bOutputMeter.GetBuffer(), nFrames);
  mGrMeterBallistics.ProcessBlock(bGrMeter.GetBuffer(), nFrames);
  mScopeBallistics.ProcessBlock(bOutputMeter.GetBuffer(), nFrames);
#endif // USEBUFFER




}

#pragma mark - IPlug overrides

void SRChannel::OnIdle() {
  mInputMeterBallistics.TransmitData(*this);
  mGrMeterBallistics.TransmitData(*this);
  mOutputMeterBallistics.TransmitData(*this);
  mScopeBallistics.TransmitData(*this);
  SetFreqMeterValues();
}

void SRChannel::OnReset() {
  mSampleRate = GetSampleRate();
  mNumInChannels = NChannelsConnected(kInput);
  mNumOutChannels = NChannelsConnected(kOutput);
  InitEffects();
}

void SRChannel::OnParamChange(int paramIdx) {

  // If AGC enabled let the gain compensation be started on any parameter changes
#ifdef USEAGC
  if (mAgc)
    mAgcTrigger = true;
#endif // USEAGC

  //switch (GetParam(paramIdx)->Type())
  //{
  //case IParam::EParamType::kTypeDouble:
  //  break;
  //}

  switch (paramIdx)
  {

    // INPUT AND OUTPUT STAGE
    // ----------------------

  case kInputGain:
    mInputGain = DBToAmp(GetParam(paramIdx)->Value());
    fInputGain.SetGain(mInputGain);
    break;
  case kOutputGain:
    mOutputGain = DBToAmp(GetParam(paramIdx)->Value());
    fOutputGain.SetGain(mOutputGain);
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
    fLimiter.SetThresh(mLimiterThresh);
    break;
  case kPan: mPan = (GetParam(paramIdx)->Value() + 100) / 200; fPan.SetPanPosition(mPan); break;
  case kPanFreq:
    mSafePanFreq = GetParam(paramIdx)->Value();
    fFilters[EFilters::kPanHp].SetFreq(mSafePanFreq / mSampleRate);
    fFilters[EFilters::kPanHp].SetFreq(mSafePanFreq / mSampleRate);
    fFilters[EFilters::kPanLp].SetFreq(mSafePanFreq / mSampleRate);
    fFilters[EFilters::kPanLp].SetFreq(mSafePanFreq / mSampleRate);
    break;
  case kIsPanMonoLow: mIsPanMonoLow = GetParam(paramIdx)->Bool(); break;
  case kWidth:
    mWidth = GetParam(paramIdx)->Value() * 0.01;
    fPan.SetWidth(mWidth);
    break;



    // FILTER
    // ------

    // High Pass
  case kEqHpFreq:
    mEqHpFreq = GetParam(paramIdx)->Value();
    for (int f = EFilters::kOpHp; f <= EFilters::kHp10; f++) {
      fFilters[f].SetFreq(mEqHpFreq / mSampleRate);
    }
    break;

  case kEqHpOrder:
    mEqHpOrder = GetParam(paramIdx)->Int();
    switch (mEqHpOrder) {
    case EFilterSlope::dbo6:									// 1st order, 6 dB/Oct
      break;
    case EFilterSlope::dbo12:
      fFilters[EFilters::kHp1].SetQ(cascadedFilterQs[1][0]);	// 2nd order, 12 dB/Oct
      break;
    case EFilterSlope::dbo18:									// 3rd order, 18 dB/Oct
      fFilters[EFilters::kHp1].SetQ(cascadedFilterQs[2][0]);
      break;
    case EFilterSlope::dbo24:									// 4th order, 24 dB/Oct
      fFilters[EFilters::kHp1].SetQ(cascadedFilterQs[3][0]);
      fFilters[EFilters::kHp2].SetQ(cascadedFilterQs[3][1]);
      break;
    case EFilterSlope::dbo36:									// 6th order, 36 dB/Oct
      fFilters[EFilters::kHp1].SetQ(cascadedFilterQs[4][0]);
      fFilters[EFilters::kHp2].SetQ(cascadedFilterQs[4][1]);
      fFilters[EFilters::kHp3].SetQ(cascadedFilterQs[4][2]);
      break;
    case EFilterSlope::dbo48:									// 8th order, 48 dB/Oct
      fFilters[EFilters::kHp1].SetQ(cascadedFilterQs[5][0]);
      fFilters[EFilters::kHp2].SetQ(cascadedFilterQs[5][1]);
      fFilters[EFilters::kHp3].SetQ(cascadedFilterQs[5][2]);
      fFilters[EFilters::kHp4].SetQ(cascadedFilterQs[5][3]);
      break;
    case EFilterSlope::dbo60:									// 10th order, 48 dB/Oct
      fFilters[EFilters::kHp1].SetQ(cascadedFilterQs[6][0]);
      fFilters[EFilters::kHp2].SetQ(cascadedFilterQs[6][1]);
      fFilters[EFilters::kHp3].SetQ(cascadedFilterQs[6][2]);
      fFilters[EFilters::kHp4].SetQ(cascadedFilterQs[6][3]);
      fFilters[EFilters::kHp5].SetQ(cascadedFilterQs[6][4]);
      break;
    case EFilterSlope::dbo72:									// 12th order, 72 db/Oct
      fFilters[EFilters::kHp1].SetQ(cascadedFilterQs[7][0]);
      fFilters[EFilters::kHp2].SetQ(cascadedFilterQs[7][1]);
      fFilters[EFilters::kHp3].SetQ(cascadedFilterQs[7][2]);
      fFilters[EFilters::kHp4].SetQ(cascadedFilterQs[7][3]);
      fFilters[EFilters::kHp5].SetQ(cascadedFilterQs[7][4]);
      fFilters[EFilters::kHp6].SetQ(cascadedFilterQs[7][5]);
      break;
    case EFilterSlope::dbo120:									// 20th order, 120 dB/Oct
      fFilters[EFilters::kHp1].SetQ(cascadedFilterQs[8][0]);
      fFilters[EFilters::kHp2].SetQ(cascadedFilterQs[8][1]);
      fFilters[EFilters::kHp3].SetQ(cascadedFilterQs[8][2]);
      fFilters[EFilters::kHp4].SetQ(cascadedFilterQs[8][3]);
      fFilters[EFilters::kHp5].SetQ(cascadedFilterQs[8][4]);
      fFilters[EFilters::kHp6].SetQ(cascadedFilterQs[8][5]);
      fFilters[EFilters::kHp7].SetQ(cascadedFilterQs[8][6]);
      fFilters[EFilters::kHp8].SetQ(cascadedFilterQs[8][7]);
      fFilters[EFilters::kHp9].SetQ(cascadedFilterQs[8][8]);
      fFilters[EFilters::kHp10].SetQ(cascadedFilterQs[8][9]);
      break;
    default:
      break;
    }

    OnParamChange(kEqHpFreq);
    break;

    // Low Pass
  case kEqLpFreq:
    mEqLpFreq = GetParam(paramIdx)->Value();
    for (int c = 0; c < mNumOutChannels; c++) {
      fFilters[EFilters::kLp].SetFreq(mEqLpFreq / mSampleRate);
    }
    break;
    //case kEqLpOrder:
//  break;

    // EQUALIZER
    // ---------

  case kEqHfBell:
    mEqHfIsBell = GetParam(paramIdx)->Bool();
    if (mEqHfIsBell == 1) {
      fFilters[EFilters::kHf].SetType(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadPeak);
    }
    else {
      fFilters[EFilters::kHf].SetType(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighshelf);
    }
    break;

  case kEqLfBell:
    mEqLfIsBell = GetParam(paramIdx)->Bool();
    if (mEqLfIsBell == 1) {
      fFilters[EFilters::kLf].SetType(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadPeak);
    }
    else {
      fFilters[EFilters::kLf].SetType(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadLowshelf);
    }
    break;

  case kEqLfGain:
    mEqLfGain = GetParam(paramIdx)->Value() * mEqAmount;
    fFilters[EFilters::kLf].SetPeakGain(mEqLfGain);
    break;

  case kEqLfFreq:
    mEqLfFreq = GetParam(paramIdx)->Value();
    fFilters[EFilters::kLf].SetFreq(mEqLfFreq / mSampleRate);
    break;
    //case kEqLfQ: mEqLfQ = GetParam(paramIdx)->Value(); fEqLfFilter[0].setQ(mEqLfQ); fEqLfFilter[1].setQ(mEqLfQ); break;

  case kEqLmfGain:
    mEqLmfGain = GetParam(paramIdx)->Value() * mEqAmount;
    fFilters[EFilters::kLmf].SetPeakGain(mEqLmfGain);
    break;

  case kEqLmfFreq:
    mEqLmfFreq = GetParam(paramIdx)->Value();
    fFilters[EFilters::kLmf].SetFreq(mEqLmfFreq / mSampleRate);
    break;

  case kEqLmfQ:
    mEqLmfQ = GetParam(paramIdx)->Value();
    fFilters[EFilters::kLmf].SetQ(mEqLmfQ);
    break;

  case kEqHmfGain:
    mEqHmfGain = GetParam(paramIdx)->Value() * mEqAmount;
    fFilters[EFilters::kHmf].SetPeakGain(mEqHmfGain);
    break;

  case kEqHmfFreq:
    mEqHmfFreq = GetParam(paramIdx)->Value();
    fFilters[EFilters::kHmf].SetFreq(mEqHmfFreq / mSampleRate);
    break;

  case kEqHmfQ:
    mEqHmfQ = GetParam(paramIdx)->Value();
    fFilters[EFilters::kHmf].SetQ(mEqHmfQ);
    break;

  case kEqHfGain:
    mEqHfGain = GetParam(paramIdx)->Value() * mEqAmount;
    fFilters[EFilters::kHf].SetPeakGain(mEqHfGain);
    break;

  case kEqHfFreq:
    mEqHfFreq = GetParam(paramIdx)->Value();
    fFilters[EFilters::kHf].SetFreq(mEqHfFreq / mSampleRate);
    break;

  case kEqAmount:
    mEqAmount = GetParam(paramIdx)->Value() / 100.;
    OnParamChange(kEqHfGain); OnParamChange(kEqHmfGain);
    OnParamChange(kEqLmfGain); OnParamChange(kEqLfGain);
    break;
    //case kEqHfQ: mEqHfQ = GetParam(paramIdx)->Value(); fEqHfFilter[0].setQ(mEqHfQ); fEqHfFilter[1].setQ(mEqHfQ); break;


      // COMPRESSOR
      // ----------

      // Peak Compressor
  case kCompPeakRatio:
    mCompPeakRatio = (1. / GetParam(paramIdx)->Value());
    // For ratio 1:20 set infinite compression (limiting)
    if (mCompPeakRatio <= 1. / 20.) {
      mCompPeakRatio = 0.;
    }
    fCompressorPeak.SetRatio(mCompPeakRatio);
    //fCompressorPeak.SetMaxGrDb(73.4979484210802 - 88.939188010773 * (1 - exp(-1.75091102973106 * (1. / mCompPeakRatio))));
    //fCompressorPeak.SetMaxGrDb(0.);
    //mCompPeakAutoMakeup = SR::Utils::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
    break;

  case kCompPeakThresh:
    mCompPeakThresh = GetParam(paramIdx)->Value();
    fCompressorPeak.SetThresh(mCompPeakThresh);
    //mCompPeakAutoMakeup = SR::Utils::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
    break;

  case kCompPeakAttack:
    mCompPeakAttack = GetParam(paramIdx)->Value();
    fCompressorPeak.SetAttack(mCompPeakAttack);
    //mCompPeakAutoMakeup = SR::Utils::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
    break;

  case kCompPeakRelease:
    mCompPeakRelease = GetParam(paramIdx)->Value();
    fCompressorPeak.SetRelease(mCompPeakRelease);
    //mCompPeakAutoMakeup = SR::Utils::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
    break;

  case kCompPeakKneeWidthDb:
    mCompPeakKneeWidthDb = GetParam(paramIdx)->Value();
    fCompressorPeak.SetKnee(mCompPeakKneeWidthDb);
    break;

  case kCompPeakSidechainFilterFreq:
    mCompPeakSidechainFilterFreq = GetParam(paramIdx)->Value();
    fCompressorPeak.SetSidechainFilterFreq(mCompPeakSidechainFilterFreq / mSampleRate);
    fCompressorRms.SetSidechainFilterFreq(mCompPeakSidechainFilterFreq / mSampleRate); // !!! This moves to own switch when RMS sidechain filter is implemented
    break;

  case kCompPeakMakeup:
    mCompPeakMakeup = GetParam(paramIdx)->Value();
    fCompressorPeak.SetMakeup(mCompPeakMakeup);
    break;


    // RMS Compressor

  case kCompRmsRatio:
    mCompRmsRatio = (1 / GetParam(paramIdx)->Value());
    fCompressorRms.SetRatio(mCompRmsRatio);
    fCompressorRms.SetMaxGrDb(73.4979484210802 - 88.939188010773 * (1 - exp(-1.75091102973106 * (1. / mCompRmsRatio))));
    //mCompRmsAutoMakeup = SR::Utils::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsThresh:
    mCompRmsThresh = GetParam(paramIdx)->Value();
    fCompressorRms.SetThresh(mCompRmsThresh);
    //mCompRmsAutoMakeup = SR::Utils::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsAttack:
    mCompRmsAttack = GetParam(paramIdx)->Value();
    fCompressorRms.SetAttack(mCompRmsAttack);
    //mCompRmsAutoMakeup = SR::Utils::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsRelease:
    mCompRmsRelease = GetParam(paramIdx)->Value();
    fCompressorRms.SetRelease(mCompRmsRelease);
    //mCompRmsAutoMakeup = SR::Utils::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsKneeWidthDb:
    mCompRmsKneeWidthDb = GetParam(paramIdx)->Value();
    fCompressorRms.SetKnee(mCompRmsKneeWidthDb);
    break;

  case kCompRmsMakeup:
    mCompRmsMakeup = GetParam(paramIdx)->Value();
    fCompressorRms.SetMakeup(mCompRmsMakeup);
    break;

    // Both Compressors

  case kCompPeakRmsRatio:
    mCompPeakRmsRatio = GetParam(paramIdx)->Value() / 100;
    break;

  case kCompDryWet:
    mCompDryWet = GetParam(paramIdx)->Value() / 100;
    break;


    // Bool Switches

  case kCompIsParallel:
    mCompIsParallel = GetParam(paramIdx)->Bool();
    break;

  case kCompPeakIsExtSc:
    mCompPeakIsExtSc = GetParam(paramIdx)->Bool();
    break;

  case kCompRmsIsExrSc:
    mCompRmsIsExtSc = GetParam(paramIdx)->Bool();
    break;

  case kCompPeakIsFeedback:
    mCompPeakIsFeedback = GetParam(paramIdx)->Bool();
    fCompressorPeak.SetTopologyFeedback(mCompPeakIsFeedback);
    break;

  case kCompRmsIsFeedback:
    mCompRmsIsFeedback = GetParam(paramIdx)->Bool();
    fCompressorRms.SetTopologyFeedback(mCompRmsIsFeedback);
    break;


    // GLOBAL BYPASS
    // -------------

  case kEqBypass: mEqBypass = GetParam(paramIdx)->Bool(); break;
  case kCompBypass: mCompBypass = GetParam(paramIdx)->Bool(); break;
  case kInputBypass: mInputBypass = GetParam(paramIdx)->Bool(); break;
  case kOutputBypass: mOutputBypass = GetParam(paramIdx)->Bool(); break;
  case kBypass: mBypass = GetParam(paramIdx)->Bool(); break;


    // AUTOMATIC GAIN CONTROL
    // ----------------------

#ifdef USEAGC
  case kAgc:
    mAgc = GetParam(paramIdx)->Bool();
    break;
#endif // USEAGC

    // DEESSER
    // -------

  case kDeesserFreq: mDeesserFreq = GetParam(paramIdx)->Value(); fDeesser.SetFrequency(mDeesserFreq / mSampleRate); break;
  case kDeesserQ: mDeesserQ = GetParam(paramIdx)->Value(); fDeesser.SetQ(mDeesserQ); break;
  case kDeesserThresh: mDeesserThresh = GetParam(paramIdx)->Value(); fDeesser.SetThresh(mDeesserThresh); break;
  case kDeesserRatio: mDeesserRatio = (1. / GetParam(paramIdx)->Value()); fDeesser.SetRatio(mDeesserRatio); break;
  case kDeesserAttack: mDeesserAttack = GetParam(paramIdx)->Value(); fDeesser.SetAttack(mDeesserAttack); break;
  case kDeesserRelease: mDeesserRelease = GetParam(paramIdx)->Value(); fDeesser.SetRelease(mDeesserRelease); break;
  case kDeesserMakeup: mDeesserMakeup = DBToAmp(GetParam(paramIdx)->Value()); break;

  default: break;
  }
}

#pragma mark - Presets

void SRChannel::MakePresets() {
  // Make Presets
  MakeDefaultPreset("Default", 1);
  MakePresetFromNamedParams("Kick", 3,
    kEqLfFreq, 60.,
    kEqLfBell, true,
    kEqLfGain, 5.0);
  MakePresetFromNamedParams("Vocals Female", 63,
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
    kWidth, 100.0,
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
