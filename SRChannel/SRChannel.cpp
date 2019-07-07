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
  , mEnvInput(0.0)
  , mEnvPost(0.0)
  , mEnvOutput(0.0)
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
          SR::DSP::SRSaturation::ESaturationType::kNumTypes,
          p.unit,
          0,
          p.group,
          "MusicDSP",
          "Zoelzer",
          "Pirkle",
          "Pirkle Mod",
          "Soft Sat",
          "Half Rect",
          "Full Rect"
        );
        break;
      case kOversamplingRate:
        thisParameter->InitEnum(
          p.name,
          (int)p.defaultVal,
          EFactor::kNumFactors,
          p.unit,
          0,
          p.group,
          "OS OFF",
          "OS 2X",
          "OS 4X",
          "OS 8X",
          "OS 16X"
        );
        break;
      default:
        break;
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
    const IPattern patternPanel = IPattern::CreateLinearGradient(rectControls.L, rectControls.T, rectControls.R, rectControls.B, { IColorStop(SR::Graphics::SRLayout.colorPanelBG, 0.2f), IColorStop(COLOR_BLACK, 0.5f) });

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

      // Resize info boxes
      pGraphics->GetControlWithTag(cInfo)->SetTargetAndDrawRECTs(rectFooter.GetFromBottom(10.f));


      // Resize METERS:
      pGraphics->GetControlWithTag(cInputMeter)->SetTargetAndDrawRECTs(rectMeter.SubRectHorizontal(5, 1));
      pGraphics->GetControlWithTag(cInputMeterRms)->SetTargetAndDrawRECTs(rectMeter.SubRectHorizontal(5, 0));
      pGraphics->GetControlWithTag(cGrMeter)->SetTargetAndDrawRECTs(rectMeter.SubRectHorizontal(5, 2));
      pGraphics->GetControlWithTag(cOutputMeter)->SetTargetAndDrawRECTs(rectMeter.SubRectHorizontal(5, 3));
      pGraphics->GetControlWithTag(cOutputMeterRms)->SetTargetAndDrawRECTs(rectMeter.SubRectHorizontal(5, 4));
      pGraphics->GetControlWithTag(cScope)->SetTargetAndDrawRECTs(rectHeader.SubRectHorizontal(6, 0).GetVPadded(-5.f));
      pGraphics->GetControlWithTag(cFreqMeter)->SetTargetAndDrawRECTs(rectHeader.SubRectHorizontal(6, 1).GetVPadded(-5.f));

      // Resize PRESET menu:
      //pGraphics->GetControlWithTag(cPresetMenu)->SetTargetAndDrawRECTs(rectHeader.SubRectHorizontal(6, 2).SubRectVertical(4, 1).FracRectVertical(2.f, true).GetPadded(-5.f));

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
        thisControl->SetTargetAndDrawRECTs(rectCurrentControl);

      }

      // If resizing finished:
      return;

    } // End resize function



    // LOAD
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);                                        // Load std font
    pGraphics->LoadFont("Century-Gothic", CENTURY_FN);

    IBitmap bmpSRPluginsLogo = pGraphics->LoadBitmap(SRPLUGINSLOGO_FN);                       // Load logo bitmap
    IBitmap bmpSRChannelLogo = pGraphics->LoadBitmap(SRCHANNELLOGO_FN);                       // Load logo bitmap

    // SETUP
    if (!pGraphics->CanHandleMouseOver()) pGraphics->HandleMouseOver(true);                                       // Enable Mouseovers
    if (!pGraphics->TooltipsEnabled()) pGraphics->EnableTooltips(true);                                        // Enable Tooltips

    // ATTACH
    pGraphics->AttachPanelBackground(SR::Graphics::SRLayout.colorPluginBG);        // Attach Background
    pGraphics->AttachCornerResizer(EUIResizerMode::Size, true);                 // Attach Resizer

    // Attach logos and version string:
    pGraphics->AttachControl(new IBitmapControl(rectHeader.SubRectVertical(2, 0).GetFromLeft(float(bmpSRPluginsLogo.W())), bmpSRPluginsLogo, -1, EBlend::None), cSRPluginsLogo, "UI");
    pGraphics->AttachControl(new IBitmapControl(rectHeader.SubRectVertical(2, 0).GetFromRight(float(bmpSRChannelLogo.W())), bmpSRChannelLogo, -1, EBlend::None), cSRChannelLogo, "UI");
    //pGraphics->AttachControl(new ITextControl(pGraphics->GetControlWithTag(cBitmapLogo)->GetRECT().GetFromTop(SRLayout.textKnobLabel.mSize), "v" PLUG_VERSION_STR"-a", SRLayout.textVersionString), cVersion, "UI");

    // Attach section rect PANELS
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectInput, patternPanel, true), cPanelInput, "UI");
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectEq, patternPanel, true), cPanelEq, "UI");
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectComp, patternPanel, true), cPanelComp, "UI");
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectPost, patternPanel, true), cPanelPost, "UI");
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectOutput, patternPanel, true), cPanelOutput, "UI");
    pGraphics->AttachControl(new SR::Graphics::SRPanel(rectMeter, patternPanel, true), cPanelMeter, "UI");

    // Attach Info boxes
    pGraphics->AttachControl(new ITextControl(rectFooter.GetFromBottom(10.f), "TEST THINGY", SR::Graphics::SRLayout.textKnobLabel), cInfo, "UI");

    // Attach meters
    pGraphics->AttachControl(new SR::Graphics::SRMeter<2, 1024>(rectMeter.SubRectHorizontal(5, 1), false, false, -60.f, 12.f, (float)SR::Utils::SetShapeCentered(-60., 12., 0., .75), 1, 6, "In Left", "In Right"), cInputMeter, "Meter");
    pGraphics->AttachControl(new SR::Graphics::SRMeter<2, 1024>(rectMeter.SubRectHorizontal(5, 0), false, false, -60.f, 12.f, (float)SR::Utils::SetShapeCentered(-60., 12., 0., .75), 1, 6, "In Left", "In Right"), cInputMeterRms, "Meter");
    pGraphics->AttachControl(new SR::Graphics::SRMeter<3, 1024>(rectMeter.SubRectHorizontal(5, 2), true, true, -18.f, 0.f, (float)SR::Utils::SetShapeCentered(-18., 0., -9., .5), 1, 3, "GR RMS", "GR Peak", "GR Deesser"), cGrMeter, "Meter");
    pGraphics->AttachControl(new SR::Graphics::SRMeter<2, 1024>(rectMeter.SubRectHorizontal(5, 3), false, false, -60.f, 12.f, (float)SR::Utils::SetShapeCentered(-60., 12., 0., .75), 1, 6, "Out Left", "Out Right"), cOutputMeter, "Meter");
    pGraphics->AttachControl(new SR::Graphics::SRMeter<2, 1024>(rectMeter.SubRectHorizontal(5, 4), false, false, -60.f, 12.f, (float)SR::Utils::SetShapeCentered(-60., 12., 0., .75), 1, 6, "Out Left", "Out Right"), cOutputMeterRms, "Meter");
    pGraphics->AttachControl(new IVScopeControl<2>(rectHeader.SubRectHorizontal(6, 0).GetVPadded(-5.f), "Waveform", SR::Graphics::SRLayout.style, "Left"), cScope, "UI");
    //pGraphics->AttachControl(new SR::Graphics::SRFrequencyResponseMeter(rectHeader.SubRectHorizontal(6, 1).GetVPadded(-5.f), FREQUENCYRESPONSE, mFreqMeterValues, SR::Utils::SetShapeCentered(0., 22000., 1000., .5), SRLayout.colorSpec), cFreqMeter, "Meter");
    pGraphics->AttachControl(new SR::Graphics::SRGraphBase(rectHeader.SubRectHorizontal(6, 1).GetVPadded(-5.f), FREQUENCYRESPONSE, mFreqMeterValues, SR::Graphics::SRLayout.style), cFreqMeter, "Meter");

    // Attach preset menu:
    //pGraphics->AttachControl(new SR::Graphics::SRPresetMenu(this, rectHeader.SubRectHorizontal(6, 2).SubRectVertical(4, 1).FracRectVertical(2.f, true).GetPadded(-5.f), SRLayout.textPresetMenu, namedParams), cPresetMenu, "UI");

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
      case EControlImages::SslBlue: knobColor = SR::Graphics::SRLayout.colorKnobSslBlue; break;
      case EControlImages::SslGreen: knobColor = SR::Graphics::SRLayout.colorKnobSslGreen; break;
      case EControlImages::SslRed: knobColor = SR::Graphics::SRLayout.colorKnobSslRed; break;
      case EControlImages::SslOrange: knobColor = SR::Graphics::SRLayout.colorKnobSslOrange; break;
      case EControlImages::SslYellow: knobColor = SR::Graphics::SRLayout.colorKnobSslYellow; break;
      case EControlImages::SslBlack: knobColor = SR::Graphics::SRLayout.colorKnobSslBlack; break;
      case EControlImages::SslWhite: knobColor = SR::Graphics::SRLayout.colorKnobSslWhite; break;
      default: knobColor = SR::Graphics::SRLayout.style.colorSpec.GetColor(EVColor::kFG); break;
      }

      switch (p.Type)
      {
      case IParam::EParamType::kTypeInt: // No int ctrl...
      case IParam::EParamType::kTypeDouble:
        switch (paramIdx)
        {

        case kInputGain:
        case kOutputGain:
          // Attach input and output faders
          pGraphics->AttachControl(new IVSliderControl(rectCurrentControl, paramIdx, p.label, SR::Graphics::SRLayout.style, true, EDirection::Vertical, false, 8.f, 2.f), ctrlIdx);
          break;

        default:
          // Attach knobs for all other double

          // This will return when old Knob is implemented again
          /*
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
          */

          pGraphics->AttachControl(new SR::Graphics::SRVectorKnob(
            rectCurrentControl,
            paramIdx,
            p.label,
            SR::Graphics::SRLayout.style,
            true,
            false
          ), ctrlIdx);
          break;
        }
        break;

      // Attach switches for bool and enum
      case IParam::EParamType::kTypeEnum:

        pGraphics->AttachControl(new SR::Graphics::SRVectorSwitch(
          rectCurrentControl,
          paramIdx,
          p.label,
          SR::Graphics::SRLayout.style,
          false
        ), ctrlIdx);
        break;

      case IParam::EParamType::kTypeBool:

        // TODO: Commented until reimplementation
        /*
        pGraphics->AttachControl(new SR::Graphics::SRVectorSwitch(
          rectCurrentControl,
          paramIdx,
          SplashClickActionFunc,
          p.label,
          SRLayout.style,
          GetParam(paramIdx)->NDisplayTexts()
        ), ctrlIdx);
        */

        pGraphics->AttachControl(new SR::Graphics::SRVectorToggle(
          rectCurrentControl,
          paramIdx,
          p.label,
          SR::Graphics::SRLayout.style
        ), ctrlIdx);

      default:
        break;
      }

      // Other setup for EACH parameter control
      pGraphics->GetControlWithTag(ctrlIdx)->SetMOWhenGrayed(true);
      pGraphics->GetControlWithTag(ctrlIdx)->SetTooltip(p.tooltip);
    }

    // Other setup for ALL parameter controls
    //pGraphics->StyleAllVectorControls(true, true, false, 0.1f, 2.f, 3.f, SRLayout.colorSpec); //TODO: DEPRECATED
    pGraphics->StyleAllVectorControls(SR::Graphics::SRLayout.style);

  };
  // END LAYOUT function

}
// End of plugin class constructor

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

  for (int channel = 0; channel < MAXNUMOUTCHANNELS; channel++) {

    // Init envelope filter
    fEnvInput.Reset(10000., mSampleRate);
    fEnvPost.Reset(10000., mSampleRate);
    fEnvOutput.Reset(10000., mSampleRate);

    // Init saturation
    fInputSaturation[channel].SetSaturation(SR::DSP::SRSaturation::ESaturationType::kMusicDSP, mSaturationDrive, mSaturationAmount, mSaturationHarmonics, false, mSaturationSkew, 1., mSampleRate);

    // Oversampling
    mOverSampler[channel].Reset();
  }

  // Init compressor
  fCompressorPeak.InitCompressor(mCompPeakThresh, mCompPeakRatio, mCompPeakAttack, mCompPeakRelease, mCompPeakSidechainFilterFreq, mCompPeakKneeWidthDb, mCompPeakIsFeedback, false, -18., mSampleRate);
  fCompressorPeak.InitSidechainFilter(mCompPeakSidechainFilterFreq);
  fCompressorPeak.Reset();
  fCompressorPeak.SetMaxGrDb(-50., false);

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
  bInputMeter.ResetBuffer(2, GetBlockSize());
  bGrMeter.ResetBuffer(3, GetBlockSize());
  bOutputMeter.ResetBuffer(2, GetBlockSize());
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

  bInputMeter.SetNumFrames(nFrames);
  bOutputMeter.SetNumFrames(nFrames);
  bGrMeter.SetNumFrames(nFrames);



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
      bInputMeter.ProcessBuffer(*out1, 0, s);
      bInputMeter.ProcessBuffer(*out2, 1, s);

      mEnvInput = fEnvInput.Process(std::fabs((*out1 + *out2) * 0.5));


      // ----------------
      // End Pre Sections



      // INPUT SECTION
      // -------------
      if (mInputBypass != 1) {

        // Saturation
        if (mSaturationAmount != 0.) {
          if (mOversamplingRate == EFactor::kNone) {
            // Non oversampled saturation processing
            *out1 = fInputSaturation[0].Process(*out1);
            *out2 = fInputSaturation[1].Process(*out2);
          }
          else {
            // oversampled saturation processing
            *out1 = mOverSampler[0].Process(*out1, std::bind(&SR::DSP::SRSaturation::Process, fInputSaturation[0], std::placeholders::_1));
            *out2 = mOverSampler[1].Process(*out2, std::bind(&SR::DSP::SRSaturation::Process, fInputSaturation[1], std::placeholders::_1));
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
          *out1 = (1. - 4. / (*out1 + (4. - 1.))) * 4. + 1.;
        }
        else
        {
          if (*out1 < -1.) {
            *out1 = (1. + 4. / (*out1 - (4. - 1.))) * -4. - 1.;
          }
        }

        if (*out2 > 1.) {
          *out2 = (1. - 4. / (*out2 + (4. - 1.))) * 4. + 1.;
        }
        else
        {
          if (*out2 < -1.) {
            *out2 = (1. + 4. / (*out2 - (4. - 1.))) * -4. - 1.;
          }
        }

      }
      // ------------------
      // End Output Section



      // POST SECTIONS
      // -------------

      // Apply out gain
      fOutputGain.Process(*out1, *out2);


      mEnvPost = fEnvPost.Process(std::fabs((*out1 + *out2) * 0.5));


      // Apply automatic gain
      if (mAgc)
        fAutoGain.Process(*out1, *out2);

      // Apply DC Remove filter
      *out1 = fFilters[EFilters::kDC].Process(*out1, 0);
      *out2 = fFilters[EFilters::kDC].Process(*out2, 1);

      // subtract DC offset
      *out1 -= dcoff;
      *out2 -= dcoff;

      mEnvOutput = fEnvOutput.Process(std::fabs((*out1 + *out2) * 0.5));


      // ----------------
      // End Post Section


    }
    // -------------------------
    // End of global bypass test

    // fill meter value buffers w ith current sample
    bOutputMeter.ProcessBuffer(*out1, 0, s);
    bOutputMeter.ProcessBuffer(*out2, 1, s);
    bGrMeter.ProcessBuffer(fCompressorRms.GetGrLin(), 0, s);
    bGrMeter.ProcessBuffer(fCompressorPeak.GetGrLin(), 1, s);
    bGrMeter.ProcessBuffer(fDeesser.GetGrLin(), 2, s);

  }
  // ----------------------------------------------------------------
  // End processing per frame and again some processing per Framesize

#ifdef USEAGC
  // This is the automatic gain compensation. Takes place after the sample loop
  // mAgcTrigger is true after every parameter change
  if (mAgc && mAgcTrigger) {
    // Get the average of the last sample of each the input and output envelope buffer
    // calculating the difference

    if (mEnvInput > 0.003981071705534969 && mEnvPost > 0.003981071705534969) { // this is -48dB in linear
      const double diff = mEnvInput / mEnvPost;
      mAutoGain = diff;
      fAutoGain.SetGain(mAutoGain);
      const double totaldiff = mEnvInput / mEnvOutput;
      DBGMSG("%f %f", mAutoGain, totaldiff);
      if (std::fabs(1. - totaldiff) < 0.0001) {
        mAgcTrigger = false;
      }

    }
  }
#endif

  // Send meter buffer data to meter ballistics class
  mInputMeterBallistics.ProcessBlock(bInputMeter.GetBuffer(), nFrames);
  mOutputMeterBallistics.ProcessBlock(bOutputMeter.GetBuffer(), nFrames);
  mGrMeterBallistics.ProcessBlock(bGrMeter.GetBuffer(), nFrames);
  mScopeBallistics.ProcessBlock(bOutputMeter.GetBuffer(), nFrames);
}

#pragma mark - IPlug overrides

void SRChannel::OnIdle() {
  mInputMeterBallistics.TransmitData(*this);
  mGrMeterBallistics.TransmitData(*this);
  mOutputMeterBallistics.TransmitData(*this);
  mScopeBallistics.TransmitData(*this);
  SetFreqMeterValues();
  if (GetUI())
    dynamic_cast<ITextControl*>(GetUI()->GetControlWithTag(cInfo))->SetStrFmt(1024, "Samplerate: %6.0f | Framesize: %d | Channels: %d/%d | Version: %s", mSampleRate, GetBlockSize(), mNumInChannels, mNumOutChannels, PLUG_VERSION_STR);
}


void SRChannel::OnReset() {
  mSampleRate = GetSampleRate();
  mNumInChannels = NChannelsConnected(ERoute::kInput);
  mNumOutChannels = NChannelsConnected(ERoute::kOutput);
  InitEffects();
}

void SRChannel::OnParamChange(int paramIdx) {
  IParam* paramChanged = GetParam(paramIdx);

  // If AGC enabled let the gain compensation be started on any parameter changes
  // Could optionally only apply if parameter has something to do with any gain changes
#ifdef USEAGC
  if (mAgc) {
    mAgcTrigger = true;
  }
#endif // USEAGC

  switch (paramIdx)
  {

    // ---------------------------------------------------------------------------------------------------------------
    // INPUT AND OUTPUT STAGE
    // ---------------------------------------------------------------------------------------------------------------

  case kInputGain:
    mInputGain = DBToAmp(paramChanged->Value());
    fInputGain.SetGain(mInputGain);
    break;

  case kOutputGain:
    mOutputGain = DBToAmp(paramChanged->Value());
    fOutputGain.SetGain(mOutputGain);
    break;

  case kSaturationDrive:
    mSaturationDrive = paramChanged->Value();
    fInputSaturation[0].SetDrive(mSaturationDrive);
    fInputSaturation[1].SetDrive(mSaturationDrive);
    break;

  case kSaturationAmount:
    mSaturationAmount = paramChanged->Value() / 100.;
    fInputSaturation[0].SetAmount(mSaturationAmount);
    fInputSaturation[1].SetAmount(mSaturationAmount);
    break;

  case kSaturationHarmonics:
    mSaturationHarmonics = paramChanged->Value() / 100.;
    fInputSaturation[0].SetHarmonics(mSaturationHarmonics);
    fInputSaturation[1].SetHarmonics(mSaturationHarmonics);
    break;

  case kSaturationSkew:
    mSaturationSkew = paramChanged->Value() * 0.05;
    fInputSaturation[0].SetSkew(mSaturationSkew);
    fInputSaturation[1].SetSkew(mSaturationSkew);
    break;

  case kSaturationType:
    mSaturationType = paramChanged->Int();
    fInputSaturation[0].SetType(SR::DSP::SRSaturation::ESaturationType(mSaturationType));
    fInputSaturation[1].SetType(SR::DSP::SRSaturation::ESaturationType(mSaturationType));
    break;

  case kOversamplingRate:
    mOversamplingRate = paramChanged->Int();
    mOverSampler[0].SetOverSampling((EFactor)mOversamplingRate);
    mOverSampler[1].SetOverSampling((EFactor)mOversamplingRate);
    break;

  case kClipperThreshold:
    mClipperThreshold = 1. - paramChanged->Value() / 100.;
    break;

  case kLimiterThresh:
    mLimiterThresh = paramChanged->Value();
    fLimiter.SetThresh(mLimiterThresh);
    break;

  case kPan:
    mPan = (paramChanged->Value() + 100.) / 200.;
    fPan.SetPanPosition(mPan);
    break;

  case kPanFreq:
    mSafePanFreq = paramChanged->Value();
    fFilters[EFilters::kPanHp].SetFreq(mSafePanFreq / mSampleRate);
    fFilters[EFilters::kPanHp].SetFreq(mSafePanFreq / mSampleRate);
    fFilters[EFilters::kPanLp].SetFreq(mSafePanFreq / mSampleRate);
    fFilters[EFilters::kPanLp].SetFreq(mSafePanFreq / mSampleRate);
    break;

  case kIsPanMonoLow:
    mIsPanMonoLow = paramChanged->Bool();
    break;

  case kWidth:
    mWidth = paramChanged->Value() * 0.01;
    fPan.SetWidth(mWidth);
    break;


    // ---------------------------------------------------------------------------------------------------------------
    // FILTER
    // ---------------------------------------------------------------------------------------------------------------

      // High Pass
  case kEqHpFreq:
    mEqHpFreq = paramChanged->Value();
    for (int f = EFilters::kOpHp; f <= EFilters::kHp10; f++) {
      fFilters[f].SetFreq(mEqHpFreq / mSampleRate);
    }
    break;

  case kEqHpOrder:
    mEqHpOrder = paramChanged->Int();
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
    mEqLpFreq = paramChanged->Value();
    fFilters[EFilters::kLp].SetFreq(mEqLpFreq / mSampleRate);
    break;
    //case kEqLpOrder:
    //  break;


      // ---------------------------------------------------------------------------------------------------------------
      // EQUALIZER
      // ---------------------------------------------------------------------------------------------------------------

  case kEqHfBell:
    mEqHfIsBell = paramChanged->Bool();
    if (mEqHfIsBell)
      fFilters[EFilters::kHf].SetType(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadPeak);
    else
      fFilters[EFilters::kHf].SetType(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadHighshelf);
    break;

  case kEqLfBell:
    mEqLfIsBell = paramChanged->Bool();
    if (mEqLfIsBell)
      fFilters[EFilters::kLf].SetType(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadPeak);
    else
      fFilters[EFilters::kLf].SetType(SR::DSP::SRFilterIIR<sample, MAXNUMOUTCHANNELS>::EFilterType::BiquadLowshelf);
    break;

  case kEqLfGain:
    mEqLfGain = paramChanged->Value() * mEqAmount;
    fFilters[EFilters::kLf].SetPeakGain(mEqLfGain);
    break;

  case kEqLfFreq:
    mEqLfFreq = paramChanged->Value();
    fFilters[EFilters::kLf].SetFreq(mEqLfFreq / mSampleRate);
    break;

  case kEqLmfGain:
    mEqLmfGain = paramChanged->Value() * mEqAmount;
    fFilters[EFilters::kLmf].SetPeakGain(mEqLmfGain);
    break;

  case kEqLmfFreq:
    mEqLmfFreq = paramChanged->Value();
    fFilters[EFilters::kLmf].SetFreq(mEqLmfFreq / mSampleRate);
    break;

  case kEqLmfQ:
    mEqLmfQ = paramChanged->Value();
    fFilters[EFilters::kLmf].SetQ(mEqLmfQ);
    break;

  case kEqHmfGain:
    mEqHmfGain = paramChanged->Value() * mEqAmount;
    fFilters[EFilters::kHmf].SetPeakGain(mEqHmfGain);
    break;

  case kEqHmfFreq:
    mEqHmfFreq = paramChanged->Value();
    fFilters[EFilters::kHmf].SetFreq(mEqHmfFreq / mSampleRate);
    break;

  case kEqHmfQ:
    mEqHmfQ = paramChanged->Value();
    fFilters[EFilters::kHmf].SetQ(mEqHmfQ);
    break;

  case kEqHfGain:
    mEqHfGain = paramChanged->Value() * mEqAmount;
    fFilters[EFilters::kHf].SetPeakGain(mEqHfGain);
    break;

  case kEqHfFreq:
    mEqHfFreq = paramChanged->Value();
    fFilters[EFilters::kHf].SetFreq(mEqHfFreq / mSampleRate);
    break;

  case kEqAmount:
    mEqAmount = paramChanged->Value() / 100.;
    OnParamChange(kEqHfGain); OnParamChange(kEqHmfGain);
    OnParamChange(kEqLmfGain); OnParamChange(kEqLfGain);
    break;

    // ---------------------------------------------------------------------------------------------------------------
    // COMPRESSOR
    // ---------------------------------------------------------------------------------------------------------------

        // Peak Compressor
  case kCompPeakRatio:
    mCompPeakRatio = (1. / paramChanged->Value());
    // For ratio 1:20 set infinite compression (limiting)
    if (mCompPeakRatio <= 1. / 20.) {
      mCompPeakRatio = 0.;
    }
    fCompressorPeak.SetRatio(mCompPeakRatio);
    //fCompressorPeak.SetMaxGrDb(73.4979484210802 - 88.939188010773 * (1 - exp(-1.75091102973106 * (1. / mCompPeakRatio))));
    //fCompressorPeak.SetMaxGrDb(0.);
    break;

  case kCompPeakThresh:
    mCompPeakThresh = paramChanged->Value();
    fCompressorPeak.SetThresh(mCompPeakThresh);
    break;

  case kCompPeakAttack:
    mCompPeakAttack = paramChanged->Value();
    fCompressorPeak.SetAttack(mCompPeakAttack);
    break;

  case kCompPeakRelease:
    mCompPeakRelease = paramChanged->Value();
    fCompressorPeak.SetRelease(mCompPeakRelease);
    break;

  case kCompPeakKneeWidthDb:
    mCompPeakKneeWidthDb = paramChanged->Value();
    fCompressorPeak.SetKnee(mCompPeakKneeWidthDb);
    break;

  case kCompPeakSidechainFilterFreq:
    mCompPeakSidechainFilterFreq = paramChanged->Value();
    fCompressorPeak.SetSidechainFilterFreq(mCompPeakSidechainFilterFreq / mSampleRate);
    fCompressorRms.SetSidechainFilterFreq(mCompPeakSidechainFilterFreq / mSampleRate); // !!! This moves to own switch when RMS sidechain filter is implemented
    break;

  case kCompPeakMakeup:
    mCompPeakMakeup = paramChanged->Value();
    fCompressorPeak.SetMakeup(mCompPeakMakeup);
    break;


    // RMS Compressor

  case kCompRmsRatio:
    mCompRmsRatio = (1. / paramChanged->Value());
    fCompressorRms.SetRatio(mCompRmsRatio);
    fCompressorRms.SetMaxGrDb(-15.);
    break;

  case kCompRmsThresh:
    mCompRmsThresh = paramChanged->Value();
    fCompressorRms.SetThresh(mCompRmsThresh);
    break;

  case kCompRmsAttack:
    mCompRmsAttack = paramChanged->Value();
    fCompressorRms.SetAttack(mCompRmsAttack);
    break;

  case kCompRmsRelease:
    mCompRmsRelease = paramChanged->Value();
    fCompressorRms.SetRelease(mCompRmsRelease);
    break;

  case kCompRmsKneeWidthDb:
    mCompRmsKneeWidthDb = paramChanged->Value();
    fCompressorRms.SetKnee(mCompRmsKneeWidthDb);
    break;

  case kCompRmsMakeup:
    mCompRmsMakeup = paramChanged->Value();
    fCompressorRms.SetMakeup(mCompRmsMakeup);
    break;

    // Both Compressors

  case kCompPeakRmsRatio:
    mCompPeakRmsRatio = paramChanged->Value() / 100.;
    break;

  case kCompDryWet:
    mCompDryWet = paramChanged->Value() / 100.;
    break;


    // Bool Switches

  case kCompIsParallel:
    mCompIsParallel = paramChanged->Bool();
    break;

  case kCompPeakIsExtSc:
    mCompPeakIsExtSc = paramChanged->Bool();
    break;

  case kCompRmsIsExrSc:
    mCompRmsIsExtSc = paramChanged->Bool();
    break;

  case kCompPeakIsFeedback:
    mCompPeakIsFeedback = paramChanged->Bool();
    fCompressorPeak.SetTopologyFeedback(mCompPeakIsFeedback);
    break;

  case kCompRmsIsFeedback:
    mCompRmsIsFeedback = paramChanged->Bool();
    fCompressorRms.SetTopologyFeedback(mCompRmsIsFeedback);
    break;

    // ---------------------------------------------------------------------------------------------------------------
    // GLOBAL BYPASS
    // ---------------------------------------------------------------------------------------------------------------

  case kEqBypass: mEqBypass = paramChanged->Bool(); break;
  case kCompBypass: mCompBypass = paramChanged->Bool(); break;
  case kInputBypass: mInputBypass = paramChanged->Bool(); break;
  case kOutputBypass: mOutputBypass = paramChanged->Bool(); break;
  case kBypass: mBypass = paramChanged->Bool(); break;

    // ---------------------------------------------------------------------------------------------------------------
    // AUTOMATIC GAIN CONTROL
    // ---------------------------------------------------------------------------------------------------------------

#ifdef USEAGC
  case kAgc:
    mAgc = paramChanged->Bool();
    break;
#endif // USEAGC


    // ---------------------------------------------------------------------------------------------------------------
    // DEESSER
    // ---------------------------------------------------------------------------------------------------------------

  case kDeesserFreq: mDeesserFreq = paramChanged->Value(); fDeesser.SetFrequency(mDeesserFreq / mSampleRate); break;
  case kDeesserQ: mDeesserQ = paramChanged->Value(); fDeesser.SetQ(mDeesserQ); break;
  case kDeesserThresh: mDeesserThresh = paramChanged->Value(); fDeesser.SetThresh(mDeesserThresh); break;
  case kDeesserRatio: mDeesserRatio = (1. / paramChanged->Value()); fDeesser.SetRatio(mDeesserRatio); break;
  case kDeesserAttack: mDeesserAttack = paramChanged->Value(); fDeesser.SetAttack(mDeesserAttack); break;
  case kDeesserRelease: mDeesserRelease = paramChanged->Value(); fDeesser.SetRelease(mDeesserRelease); break;
  case kDeesserMakeup: mDeesserMakeup = DBToAmp(paramChanged->Value()); break;

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
