#include "SRChannel.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#define _USE_MATH_DEFINES
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>


SRChannel::SRChannel(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo) {
  // Initialize Parameters
  for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {
    IParam* param = GetParam(paramIdx);												// ... for which we temporally create a pointer "param"
    const structParameterProperties &properties = parameterProperties[paramIdx];		// ... and a variable "properties" pointing at the current parameters properties
    GetParam(paramIdx)->InitDouble(properties.name, properties.defaultVal, properties.minVal, properties.maxVal, properties.stepValue, properties.label, 0, properties.group, new IParam::ShapePowCurve(SRPlugins::SRHelpers::SetShapeCentered(properties.minVal, properties.maxVal, properties.centerVal, properties.centerPoint)), IParam::kUnitCustom);

    // Setup display texts
    switch (paramIdx)
    {
    case kEqHpOrder:
    //case kEqLpOrder:
      GetParam(paramIdx)->SetDisplayText(EFilterSlope::dbo6, "6 dB/oct");
      GetParam(paramIdx)->SetDisplayText(EFilterSlope::dbo12, "12 dB/oct");
      GetParam(paramIdx)->SetDisplayText(EFilterSlope::dbo18, "18 dB/oct");
      GetParam(paramIdx)->SetDisplayText(EFilterSlope::dbo24, "24 dB/oct");
      GetParam(paramIdx)->SetDisplayText(EFilterSlope::dbo36, "36 dB/oct");
      GetParam(paramIdx)->SetDisplayText(EFilterSlope::dbo48, "48 dB/oct");
      GetParam(paramIdx)->SetDisplayText(EFilterSlope::dbo60, "60 dB/oct");
      GetParam(paramIdx)->SetDisplayText(EFilterSlope::dbo72, "72 dB/oct");
      GetParam(paramIdx)->SetDisplayText(EFilterSlope::dbo120, "120 dB/oct");
      break;
    case kSaturationType:
      GetParam(paramIdx)->SetDisplayText(SRPlugins::SRSaturation::SaturationTypes::typeMusicDSP, "MusicDSP");
      GetParam(paramIdx)->SetDisplayText(SRPlugins::SRSaturation::SaturationTypes::typeZoelzer, "Zoelzer");
      GetParam(paramIdx)->SetDisplayText(SRPlugins::SRSaturation::SaturationTypes::typePirkle, "Pirkle");
      GetParam(paramIdx)->SetDisplayText(SRPlugins::SRSaturation::SaturationTypes::typePirkleModified, "Pirkle Mod");
      GetParam(paramIdx)->SetDisplayText(SRPlugins::SRSaturation::SaturationTypes::typeRectHalf, "Half Rect");
      GetParam(paramIdx)->SetDisplayText(SRPlugins::SRSaturation::SaturationTypes::typeRectFull, "Full Rect");
      break;
    case kLimiterThresh: GetParam(paramIdx)->SetDisplayText(10, "Off"); break;
    case kEqHpFreq: case kCompPeakSidechainFilterFreq: GetParam(paramIdx)->SetDisplayText(16, "Off"); break;
    case kEqLpFreq: GetParam(paramIdx)->SetDisplayText(22000, "Off"); break;
    case kCompPeakRatio: GetParam(paramIdx)->SetDisplayText(20, "inf"); break;
    default: break;
    }
    OnParamChange(paramIdx);
  }
  OnReset();

  // Make Presets
  //MakeDefaultPreset("Default", 1);


 // GRAPHICS func
#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    // LOAD
    pGraphics->LoadFont(ROBOTTO_FN);                                        // Load std font
    IBitmap bmpLogo = pGraphics->LoadBitmap(LOGO_FN);                       // Load logo bitmap

     // SETUP
    pGraphics->HandleMouseOver(true);                                       // Enable Mouseovers
    pGraphics->EnableTooltips(true);                                        // Enable Tooltips

    // CANVAS'
    const IRECT rectPlugin = pGraphics->GetBounds();      // Plug canvas
    const IRECT rectHeader = IRECT(0, 0, PLUG_WIDTH, 70);
    const IRECT rectControls = IRECT(0, 70, PLUG_WIDTH, PLUG_HEIGHT - 30); // Control canvas
    const IRECT rectFooter = IRECT(0, PLUG_HEIGHT - 30, PLUG_WIDTH, PLUG_HEIGHT);
    // Section rects
    const IRECT rectInput = rectControls.SubRectHorizontal(6, 0).GetPadded(-5.f);
    const IRECT rectEq = rectControls.SubRectHorizontal(6, 1).GetPadded(-5.f);
    const IRECT rectComp = rectControls.SubRectHorizontal(6, 2).GetPadded(-5.f);
    const IRECT rectPost = rectControls.SubRectHorizontal(6, 3).GetPadded(-5.f);
    const IRECT rectOutput = rectControls.SubRectHorizontal(6, 4).GetPadded(-5.f);
    const IRECT rectMeter = rectControls.SubRectHorizontal(6, 5).GetPadded(-5.f);

   // ATTACH
    pGraphics->AttachPanelBackground(pluginLayout.colorBackground2);        // Attach Background
    pGraphics->AttachCornerResizer(kUIResizerScale, false);                 // Attach Resizer

    // Attach logo
    pGraphics->AttachControl(new IBitmapControl(*this, IRECT(PLUG_WIDTH - 300, 0, PLUG_WIDTH, 70), bmpLogo, -1, kBlendNone), cBitmapLogo);
    // Attach section rects
    pGraphics->AttachControl(new IPanelControl(*this, rectInput, pluginLayout.colorBackground, true), cPanelInput, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectEq, pluginLayout.colorBackground, true), cPanelEq, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectComp, pluginLayout.colorBackground, true), cPanelComp, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectPost, pluginLayout.colorBackground, true), cPanelPost, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectOutput, pluginLayout.colorBackground, true), cPanelOutput, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectMeter, COLOR_TRANSPARENT, true), cPanelMeter, "UI");
    pGraphics->AttachControl(new IVMeterControl<2>(*this, rectMeter.SubRectHorizontal(3,0), "In Left", "In Right"), cInputMeter, "Meter");
    pGraphics->AttachControl(new IVMeterControl<3>(*this, rectMeter.SubRectHorizontal(3,1), "GR RMS", "GR Peak", "GR Deesser"), cGrMeter, "Meter");
    pGraphics->AttachControl(new IVMeterControl<2>(*this, rectMeter.SubRectHorizontal(3,2), "Out Left", "Out Right"), cOutputMeter, "Meter");
    pGraphics->AttachControl(new IVScopeControl<2>(*this, rectHeader, "Left", "Right"), cScope, "Meter");


    for (int paramIdx = 0, ctrlIdx = 0; paramIdx < kNumParams; paramIdx++, ctrlIdx++) {
      IParam *param = GetParam(paramIdx);												// ... for which we temporally create a pointer "param"
      const IRECT *rect;
      const structParameterProperties &properties = parameterProperties[paramIdx];		// ... and a variable "properties" pointing at the current parameters properties
      int panelGridRowsAndCols[2] = { 20, 4 };

      switch (properties.AttachToControlPanel) {
      case RectHeader: rect = &rectHeader; break;
      case RectInput: rect = &rectInput; break;
      case RectEq: rect = &rectEq; break;
      case RectComp: rect = &rectComp; break;
      case RectPost: rect = &rectPost; break;
      case RectOutput: rect = &rectOutput; break;
      case RectFooter: rect = &rectFooter; break;
      default: break;
      }
      if (properties.AttachToControlPanel >= 6) {
        panelGridRowsAndCols[0] = 1;
        panelGridRowsAndCols[1] = 24;
      }

      switch (properties.Type)
      {
      case typeInt: // No int ctrl...
      case typeDouble:
        switch (paramIdx)
        {
        case kInputGain:
        case kOutputGain:
          // Attach faders
          pGraphics->AttachControl(new IVSliderControl(*this, rect->GetGridCell(properties.y, properties.x, panelGridRowsAndCols[0], panelGridRowsAndCols[1]).FracRectVertical(18.f, true).FracRectHorizontal(2.f, false), paramIdx, DEFAULT_SPEC, kVertical, true, 32.f, 2.f), ctrlIdx);
          break;
        default:
          // Attach knobs
          pGraphics->AttachControl(new SRPlugins::SRControls::SRVectorKnobText(
            *this,
            rect->GetGridCell(properties.y, properties.x, panelGridRowsAndCols[0], panelGridRowsAndCols[1]).FracRectVertical(2.f, true).FracRectHorizontal(2.f, false),
            paramIdx,
            properties.name,
            true,
            DEFAULT_SPEC,
            pluginLayout.tKnobLabel,
            pluginLayout.tKnobValue
          ), ctrlIdx);
          break;
        }
        break;
      case typeEnum:
      case typeBool:
        // Attach switches
        pGraphics->AttachControl(new IVSwitchControl(*this, rect->GetGridCell(properties.y, properties.x, panelGridRowsAndCols[0], panelGridRowsAndCols[1]), paramIdx, FlashCircleClickActionFunc, properties.shortName, DEFAULT_SPEC, (int)properties.maxVal + 1), ctrlIdx);
        break;
      default:
        break;
      }
      pGraphics->GetControl(paramIdx)->SetMOWhenGrayed(true);
      pGraphics->GetControl(paramIdx)->SetTooltip(properties.tooltip);
    }

    // Additional tooltips
    pGraphics->GetControlWithTag(cInputMeter)->SetTooltip("Input peak meter for left and right channel");
    pGraphics->GetControlWithTag(cGrMeter)->SetTooltip("Gain reduction meter for RMS, peak and deessing compressors");
    pGraphics->GetControlWithTag(cOutputMeter)->SetTooltip("Output peak meter for left and right channel");
    pGraphics->GetControlWithTag(cScope)->SetTooltip("Scope fpr left and right channel");

  };
#endif
}
// END GRAPHICS func

// Method to gray controls, which are currently bypassed. Thats why you have to test from top to bottom
void SRChannel::GrayOutControls()
{
  if (GetUI()) {
    for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {
      bool grayout;

      (mBypass == 1 && parameterProperties[paramIdx].group != "Global")
        ? grayout = true
        : (mInputBypass == 1 && parameterProperties[paramIdx].group == "Input")
        ? grayout = true
        : (mCompBypass == 1 && (parameterProperties[paramIdx].group == "Compressor" || parameterProperties[paramIdx].group == "Deesser"))
        ? grayout = true
        : (mEqBypass == 1 && parameterProperties[paramIdx].group == "EQ")
        ? grayout = true
        : (mOutputBypass == 1 && parameterProperties[paramIdx].group == "Output")
        ? grayout = true
        : (mCompIsParallel == 0 && paramIdx == kCompPeakRmsRatio)
        ? grayout = true
        //: (paramIdx == kTestParam1
        //  || paramIdx == kTestParam2
        //  || paramIdx == kTestParam3
        //  || paramIdx == kTestParam4
        //  || paramIdx == kTestParam5
        //  || paramIdx == kEqLpOrder
        //  )
        //? grayout = true
        : grayout = false;

      GetUI()->GrayOutControl(paramIdx, grayout);
    }
  }
}

// DSP func
#if IPLUG_DSP

void SRChannel::ProcessBlock(sample** inputs, sample** outputs, int nFrames) {

  const int nChans = NOutChansConnected();
  
  //for (int s = 0; s < nFrames; s++) {
  //  for (int c = 0; c < nChans; c++) {
  //    outputs[c][s] = inputs[c][s];
  //  }
  //}

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* sc1 = inputs[2];
  double* sc2 = inputs[3];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  
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


      circularBufferInL[circularBufferPointer] = *out1; // Fill circular buffer with input gain values
      circularBufferInR[circularBufferPointer] = *out2;

      // ----------------
      // End Pre Sections



      // INPUT SECTION
      // -------------
      if (mInputBypass != 1) {

        // Upsample
        // Insert upsample method here ...
        // double y = mOverSampler.Process(x, [](double input) { return std::tanh(input); })

        // Saturation
        if (mSaturationAmount != 0.) fInputSaturation.process(*out1, *out2);

        // Downsample
        // Insert downsample method here ...

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
            *out1 = fEqHpFilterOnepoleL.process(*out1);
            *out2 = fEqHpFilterOnepoleR.process(*out2);
          }
          // two pole for 2nd order
          if (mEqHpOrder >= EFilterSlope::dbo12) {
            *out1 = fEqHpFilter1L.process(*out1);
            *out2 = fEqHpFilter1R.process(*out2);
          }
          // two pole for 4th order
          if (mEqHpOrder >= EFilterSlope::dbo24) {
            *out1 = fEqHpFilter2L.process(*out1);
            *out2 = fEqHpFilter2R.process(*out2);
          }
          // two pole for 6th order
          if (mEqHpOrder >= EFilterSlope::dbo36) {
            *out1 = fEqHpFilter3L.process(*out1);
            *out2 = fEqHpFilter3R.process(*out2);
          }
          // two pole for 8th order
          if (mEqHpOrder >= EFilterSlope::dbo48) {
            *out1 = fEqHpFilter4L.process(*out1);
            *out2 = fEqHpFilter4R.process(*out2);
          }
          // two pole for 10th order
          if (mEqHpOrder >= EFilterSlope::dbo60) {
            *out1 = fEqHpFilter5L.process(*out1);
            *out2 = fEqHpFilter5R.process(*out2);
          }
          // two pole for 12th order
          if (mEqHpOrder >= EFilterSlope::dbo72) {
            *out1 = fEqHpFilter6L.process(*out1);
            *out2 = fEqHpFilter6R.process(*out2);
          }
          // two pole for 20th order
          if (mEqHpOrder >= EFilterSlope::dbo120) {
            *out1 = fEqHpFilter7L.process(*out1);
            *out2 = fEqHpFilter7R.process(*out2);
            *out1 = fEqHpFilter8L.process(*out1);
            *out2 = fEqHpFilter8R.process(*out2);
            *out1 = fEqHpFilter9L.process(*out1);
            *out2 = fEqHpFilter9R.process(*out2);
            *out1 = fEqHpFilter10L.process(*out1);
            *out2 = fEqHpFilter10R.process(*out2);
          }
        }

        // Low Pass

        if (mEqLpFreq < 22000.0) { *out1 = fEqLpFilter1L.process(*out1); *out2 = fEqLpFilter1R.process(*out2); }

        // Parametric EQ

        if (mEqLfGain != 0.0) { *out1 = fEqLfFilterL.process(*out1); *out2 = fEqLfFilterR.process(*out2); }
        if (mEqLmfGain != 0.0) { *out1 = fEqLmfFilterL.process(*out1); *out2 = fEqLmfFilterR.process(*out2); }
        if (mEqHmfGain != 0.0) { *out1 = fEqHmfFilterL.process(*out1); *out2 = fEqHmfFilterR.process(*out2); }
        if (mEqHfGain != 0.0) { *out1 = fEqHfFilterL.process(*out1); *out2 = fEqHfFilterR.process(*out2); }
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

        double vCompDry1 = *out1;
        double vCompDry2 = *out2;

        if (mCompIsParallel == 1) {
          double vCompRmsIn1 = *out1;
          double vCompPeakIn1 = *out1;
          double vCompRmsIn2 = *out2;
          double vCompPeakIn2 = *out2;

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
          vSafePanLowSignal1 = *out1;
          vSafePanHighSignal1 = *out1;
          vSafePanLowSignal2 = *out2;
          vSafePanHighSignal2 = *out2;

          vSafePanLowSignal1 = fSafePanLpL.process(vSafePanLowSignal1);
          vSafePanLowSignal2 = fSafePanLpR.process(vSafePanLowSignal2);
          vSafePanHighSignal1 = fSafePanHpL.process(vSafePanHighSignal1);
          vSafePanHighSignal2 = fSafePanHpR.process(vSafePanHighSignal2);

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
      circularBufferOutL[circularBufferPointer] = *out1;
      circularBufferOutR[circularBufferPointer] = *out2;

      // Output Gain
      fOutputGain.process(*out1, *out2);
      //if (mOutputGain != 1.) {
      //	*out1 *= mOutputGain;
      //	*out2 *= mOutputGain;
      //}

      *out1 = fDcBlockerL.process(*out1);
      *out2 = fDcBlockerR.process(*out2);
      *out1 -= dcoff;
      *out2 -= dcoff;

      // ----------------
      // End Post Section


    }
    // -------------------------
    // End of global bypass test

    (circularBufferPointer >= circularBufferLenght - 1) ? circularBufferPointer = 0 : circularBufferPointer++;

  }
  // ----------------------------------------------------------------
  // End processing per frame and again some processing per Framesize




  mInputMeterBallistics.ProcessBlock(inputs, nFrames);
  //mGrMeterBallistics.ProcessBlock(!!!sample **GAINREDUCTION!!!, nFrames);
  mOutputMeterBallistics.ProcessBlock(outputs, nFrames);
  mScopeBallistics.ProcessBlock(outputs, nFrames);
}

void SRChannel::OnReset() {
  mSampleRate = GetSampleRate();
  InitEffects();
  circularBufferPointer = 0;
}

void SRChannel::OnParamChange(int paramIdx) {
  mSampleRate = GetSampleRate();

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
  case kInputDrive:
    mInputDrive = GetParam(paramIdx)->Value();
    fInputSaturation.setDrive(mInputDrive);
    break;
  case kSaturationAmount:
    mSaturationAmount = GetParam(paramIdx)->Value() / 100.;
    fInputSaturation.setAmount(mSaturationAmount);
    break;
  case kSaturationHarmonics:
    mSaturationHarmonics = GetParam(paramIdx)->Value() / 100.;
    fInputSaturation.setHarmonics(mSaturationHarmonics);
    break;
  case kSaturationSkew:
    mSaturationSkew = GetParam(paramIdx)->Value() * 0.05;
    fInputSaturation.setSkew(mSaturationSkew);
    break;
  case kSaturationType:
    mSaturationType = int(GetParam(paramIdx)->Value());
    fInputSaturation.setType(mSaturationType);
    break;
  case kClipperThreshold: mClipperThreshold = 1. - GetParam(paramIdx)->Value() / 100.; break;
  case kLimiterThresh:
    mLimiterThresh = GetParam(paramIdx)->Value();
    fLimiter.setThresh(mLimiterThresh);
    break;
  case kPan: mPan = (GetParam(paramIdx)->Value() + 100) / 200; fPan.setPanPosition(mPan); break;
  case kPanFreq:
    mSafePanFreq = GetParam(paramIdx)->Value();
    fSafePanHpL.setFc(mSafePanFreq / mSampleRate);
    fSafePanHpR.setFc(mSafePanFreq / mSampleRate);
    fSafePanLpL.setFc(mSafePanFreq / mSampleRate);
    fSafePanLpR.setFc(mSafePanFreq / mSampleRate);
    break;
  case kIsPanMonoLow: mIsPanMonoLow = GetParam(paramIdx)->Value(); break;


    // FILTER
    // ------

    // High Pass
  case kEqHpFreq:
    mEqHpFreq = GetParam(paramIdx)->Value();
    if (mEqHpOrder == EFilterSlope::dbo6 || mEqHpOrder == EFilterSlope::dbo18) {
      fEqHpFilterOnepoleL.setFc(mEqHpFreq / mSampleRate); fEqHpFilterOnepoleR.setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo12) {
      fEqHpFilter1L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter1R.setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo24) {
      fEqHpFilter2L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter2R.setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo36) {
      fEqHpFilter3L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter3R.setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo48) {
      fEqHpFilter4L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter4R.setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo60) {
      fEqHpFilter5L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter5R.setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo72) {
      fEqHpFilter6L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter6R.setFc(mEqHpFreq / mSampleRate);
    }
    if (mEqHpOrder >= EFilterSlope::dbo120) {
      fEqHpFilter7L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter7R.setFc(mEqHpFreq / mSampleRate);
      fEqHpFilter8L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter8R.setFc(mEqHpFreq / mSampleRate);
      fEqHpFilter9L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter9R.setFc(mEqHpFreq / mSampleRate);
      fEqHpFilter10L.setFc(mEqHpFreq / mSampleRate); fEqHpFilter10R.setFc(mEqHpFreq / mSampleRate);
    }
    break;

  case kEqHpOrder:
    mEqHpOrder = int(GetParam(paramIdx)->Value());
    switch (mEqHpOrder) {
    case EFilterSlope::dbo6:									// 1st order, 6 dB/Oct
      break;
    case EFilterSlope::dbo12:
      fEqHpFilter1L.setQ(mEqPassQ_O2_F1);	// 2nd order, 12 dB/Oct
      fEqHpFilter1R.setQ(mEqPassQ_O2_F1);
      break;
    case EFilterSlope::dbo18:									// 3rd order, 18 dB/Oct
      fEqHpFilter1L.setQ(mEqPassQ_O3_F1);
      fEqHpFilter1R.setQ(mEqPassQ_O3_F1);
      break;
    case EFilterSlope::dbo24:									// 4th order, 24 dB/Oct
      fEqHpFilter1L.setQ(mEqPassQ_O4_F1);
      fEqHpFilter1R.setQ(mEqPassQ_O4_F1);
      fEqHpFilter2L.setQ(mEqPassQ_O4_F2);
      fEqHpFilter2R.setQ(mEqPassQ_O4_F2);
      break;
    case EFilterSlope::dbo36:									// 6th order, 36 dB/Oct
      fEqHpFilter1L.setQ(mEqPassQ_O6_F1);
      fEqHpFilter1R.setQ(mEqPassQ_O6_F1);
      fEqHpFilter2L.setQ(mEqPassQ_O6_F2);
      fEqHpFilter2R.setQ(mEqPassQ_O6_F2);
      fEqHpFilter3L.setQ(mEqPassQ_O6_F3);
      fEqHpFilter3R.setQ(mEqPassQ_O6_F3);
      break;
    case EFilterSlope::dbo48:									// 8th order, 48 dB/Oct
      fEqHpFilter1L.setQ(mEqPassQ_O8_F1);
      fEqHpFilter1R.setQ(mEqPassQ_O8_F1);
      fEqHpFilter2L.setQ(mEqPassQ_O8_F2);
      fEqHpFilter2R.setQ(mEqPassQ_O8_F2);
      fEqHpFilter3L.setQ(mEqPassQ_O8_F3);
      fEqHpFilter3R.setQ(mEqPassQ_O8_F3);
      fEqHpFilter4L.setQ(mEqPassQ_O8_F4);
      fEqHpFilter4R.setQ(mEqPassQ_O8_F4);
      break;
    case EFilterSlope::dbo60:									// 10th order, 48 dB/Oct
      fEqHpFilter1L.setQ(mEqPassQ_O10_F1);
      fEqHpFilter1R.setQ(mEqPassQ_O10_F1);
      fEqHpFilter2L.setQ(mEqPassQ_O10_F2);
      fEqHpFilter2R.setQ(mEqPassQ_O10_F2);
      fEqHpFilter3L.setQ(mEqPassQ_O10_F3);
      fEqHpFilter3R.setQ(mEqPassQ_O10_F3);
      fEqHpFilter4L.setQ(mEqPassQ_O10_F4);
      fEqHpFilter4R.setQ(mEqPassQ_O10_F4);
      fEqHpFilter5L.setQ(mEqPassQ_O10_F5);
      fEqHpFilter5R.setQ(mEqPassQ_O10_F5);
      break;
    case EFilterSlope::dbo72:									// 12th order, 72 db/Oct
      fEqHpFilter1L.setQ(mEqPassQ_O12_F1);
      fEqHpFilter1R.setQ(mEqPassQ_O12_F1);
      fEqHpFilter2L.setQ(mEqPassQ_O12_F2);
      fEqHpFilter2R.setQ(mEqPassQ_O12_F2);
      fEqHpFilter3L.setQ(mEqPassQ_O12_F3);
      fEqHpFilter3R.setQ(mEqPassQ_O12_F3);
      fEqHpFilter4L.setQ(mEqPassQ_O12_F4);
      fEqHpFilter4R.setQ(mEqPassQ_O12_F4);
      fEqHpFilter5L.setQ(mEqPassQ_O12_F5);
      fEqHpFilter5R.setQ(mEqPassQ_O12_F5);
      fEqHpFilter6L.setQ(mEqPassQ_O12_F6);
      fEqHpFilter6R.setQ(mEqPassQ_O12_F6);
      break;
    case EFilterSlope::dbo120:									// 20th order, 120 dB/Oct
      fEqHpFilter1L.setQ(mEqPassQ_O20_F1);
      fEqHpFilter1R.setQ(mEqPassQ_O20_F1);
      fEqHpFilter2L.setQ(mEqPassQ_O20_F2);
      fEqHpFilter2R.setQ(mEqPassQ_O20_F2);
      fEqHpFilter3L.setQ(mEqPassQ_O20_F3);
      fEqHpFilter3R.setQ(mEqPassQ_O20_F3);
      fEqHpFilter4L.setQ(mEqPassQ_O20_F4);
      fEqHpFilter4R.setQ(mEqPassQ_O20_F4);
      fEqHpFilter5L.setQ(mEqPassQ_O20_F5);
      fEqHpFilter5R.setQ(mEqPassQ_O20_F5);
      fEqHpFilter6L.setQ(mEqPassQ_O20_F6);
      fEqHpFilter6R.setQ(mEqPassQ_O20_F6);
      fEqHpFilter7L.setQ(mEqPassQ_O20_F7);
      fEqHpFilter7R.setQ(mEqPassQ_O20_F7);
      fEqHpFilter8L.setQ(mEqPassQ_O20_F8);
      fEqHpFilter8R.setQ(mEqPassQ_O20_F8);
      fEqHpFilter9L.setQ(mEqPassQ_O20_F9);
      fEqHpFilter9R.setQ(mEqPassQ_O20_F9);
      fEqHpFilter10L.setQ(mEqPassQ_O20_F10);
      fEqHpFilter10R.setQ(mEqPassQ_O20_F10);
      break;
    default:
      break;
    }
    OnParamChange(kEqHpFreq);
    break;

    // Low Pass
  //case kEqLpOrder:
  //  break;

  case kEqLpFreq:
    mEqLpFreq = GetParam(paramIdx)->Value();
    fEqLpFilter1L.setFc(mEqLpFreq / mSampleRate);
    fEqLpFilter1R.setFc(mEqLpFreq / mSampleRate);
    break;


    // EQUALIZER
    // ---------

  case kEqHfBell:
    mEqHfIsBell = GetParam(paramIdx)->Value();
    if (mEqHfIsBell == 1) { fEqHfFilterL.setType(SRPlugins::SRFilters::biquad_peak); fEqHfFilterR.setType(SRPlugins::SRFilters::biquad_peak); }
    else { fEqHfFilterL.setType(SRPlugins::SRFilters::biquad_highshelf); fEqHfFilterR.setType(SRPlugins::SRFilters::biquad_highshelf); }
    break;

  case kEqLfBell:
    mEqLfIsBell = GetParam(paramIdx)->Value();
    if (mEqLfIsBell == 1) { fEqLfFilterL.setType(SRPlugins::SRFilters::biquad_peak); fEqLfFilterR.setType(SRPlugins::SRFilters::biquad_peak); }
    else { fEqLfFilterL.setType(SRPlugins::SRFilters::biquad_lowshelf); fEqLfFilterR.setType(SRPlugins::SRFilters::biquad_lowshelf); }
    break;

  case kEqLfGain: mEqLfGain = GetParam(paramIdx)->Value(); fEqLfFilterL.setPeakGain(mEqLfGain); fEqLfFilterR.setPeakGain(mEqLfGain); break;
  case kEqLfFreq: mEqLfFreq = GetParam(paramIdx)->Value(); fEqLfFilterL.setFc(mEqLfFreq / mSampleRate); fEqLfFilterR.setFc(mEqLfFreq / mSampleRate); break;
  //case kEqLfQ: mEqLfQ = GetParam(paramIdx)->Value(); fEqLfFilterL.setQ(mEqLfQ); fEqLfFilterR.setQ(mEqLfQ); break;

  case kEqLmfGain: mEqLmfGain = GetParam(paramIdx)->Value(); fEqLmfFilterL.setPeakGain(mEqLmfGain); fEqLmfFilterR.setPeakGain(mEqLmfGain); break;
  case kEqLmfFreq: mEqLmfFreq = GetParam(paramIdx)->Value(); fEqLmfFilterL.setFc(mEqLmfFreq / mSampleRate); fEqLmfFilterR.setFc(mEqLmfFreq / mSampleRate); break;
  case kEqLmfQ: mEqLmfQ = GetParam(paramIdx)->Value(); fEqLmfFilterL.setQ(mEqLmfQ); fEqLmfFilterR.setQ(mEqLmfQ); break;

  case kEqHmfGain: mEqHmfGain = GetParam(paramIdx)->Value(); fEqHmfFilterL.setPeakGain(mEqHmfGain); fEqHmfFilterR.setPeakGain(mEqHmfGain); break;
  case kEqHmfFreq: mEqHmfFreq = GetParam(paramIdx)->Value(); fEqHmfFilterL.setFc(mEqHmfFreq / mSampleRate); fEqHmfFilterR.setFc(mEqHmfFreq / mSampleRate); break;
  case kEqHmfQ: mEqHmfQ = GetParam(paramIdx)->Value(); fEqHmfFilterL.setQ(mEqHmfQ); fEqHmfFilterR.setQ(mEqHmfQ); break;

  case kEqHfGain: mEqHfGain = GetParam(paramIdx)->Value(); fEqHfFilterL.setPeakGain(mEqHfGain); fEqHfFilterR.setPeakGain(mEqHfGain); break;
  case kEqHfFreq: mEqHfFreq = GetParam(paramIdx)->Value(); fEqHfFilterL.setFc(mEqHfFreq / mSampleRate); fEqHfFilterR.setFc(mEqHfFreq / mSampleRate); break;
  //case kEqHfQ: mEqHfQ = GetParam(paramIdx)->Value(); fEqHfFilterL.setQ(mEqHfQ); fEqHfFilterR.setQ(mEqHfQ); break;


    // COMPRESSOR
    // ----------

    // Peak Compressor
  case kCompPeakRatio:
    mCompPeakRatio = (1. / GetParam(paramIdx)->Value());
    if (mCompPeakRatio <= 1. / 20.) {
      mCompPeakRatio = 0.;
    }
    fCompressorPeak.setRatio(mCompPeakRatio);
    mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
    break;

  case kCompPeakThresh:
    mCompPeakThresh = GetParam(paramIdx)->Value();
    fCompressorPeak.setThresh(mCompPeakThresh);
    mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
    break;

  case kCompPeakAttack:
    mCompPeakAttack = GetParam(paramIdx)->Value();
    fCompressorPeak.setAttack(mCompPeakAttack);
    mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
    break;

  case kCompPeakRelease:
    mCompPeakRelease = GetParam(paramIdx)->Value();
    fCompressorPeak.setRelease(mCompPeakRelease);
    mCompPeakAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompPeakThresh, mCompPeakRatio, -18., mCompPeakAttack, mCompPeakRelease); // Auto Makeup
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
    mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsThresh:
    mCompRmsThresh = GetParam(paramIdx)->Value();
    fCompressorRms.setThresh(mCompRmsThresh);
    mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsAttack:
    mCompRmsAttack = GetParam(paramIdx)->Value();
    fCompressorRms.setAttack(mCompRmsAttack);
    mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsRelease:
    mCompRmsRelease = GetParam(paramIdx)->Value();
    fCompressorRms.setRelease(mCompRmsRelease);
    mCompRmsAutoMakeup = SRPlugins::SRHelpers::calcAutoMakeup(mCompRmsThresh, mCompRmsRatio, -18., mCompRmsAttack, mCompRmsRelease); // Auto Makeup
    break;

  case kCompRmsKneeWidthDb: mCompRmsKneeWidthDb = GetParam(paramIdx)->Value(); fCompressorRms.setKnee(mCompRmsKneeWidthDb); break;
  case kCompRmsMakeup: mCompRmsMakeup = DBToAmp(GetParam(paramIdx)->Value()); break;

    // Both Compressors
  case kCompPeakRmsRatio: mCompPeakRmsRatio = GetParam(paramIdx)->Value() / 100; break;
  case kCompDryWet: mCompDryWet = GetParam(paramIdx)->Value() / 100; break;

    // Bool Switches
  case kCompIsParallel: mCompIsParallel = GetParam(paramIdx)->Value(); if (GetUI()) GrayOutControls(); break;
  case kCompPeakIsExtSc: mCompPeakIsExtSc = GetParam(paramIdx)->Value(); break;
  case kCompRmsIsExrSc: mCompRmsIsExtSc = GetParam(paramIdx)->Value(); break;
  case kCompPeakIsFeedback: mCompPeakIsFeedback = GetParam(paramIdx)->Value(); fCompressorPeak.setTopologyFeedback(mCompPeakIsFeedback); break;
  case kCompRmsIsFeedback: mCompRmsIsFeedback = GetParam(paramIdx)->Value(); fCompressorRms.setTopologyFeedback(mCompRmsIsFeedback); break;


    // GLOBAL BYPASS
    // -------------

  case kEqBypass: mEqBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
  case kCompBypass: mCompBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
  case kInputBypass: mInputBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
  case kOutputBypass: mOutputBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;
  case kBypass: mBypass = GetParam(paramIdx)->Value(); GrayOutControls(); break;


    // AUTOMATIC GAIN CONTROL
    // ----------------------

  case kAgc:
    //if (circularBufferInL[circularBufferLenght - 1]) {
    //  sumIn = 0.;
    //  sumOut = 0.;
    //  aveIn = 0.;
    //  aveOut = 0.;
    //  diffInOut = 0.;
    //  for (int i = 0; i <= circularBufferLenght - 1; i++) {
    //    sumIn += fabs(circularBufferInL[i]) + fabs(circularBufferInR[i]);
    //    sumOut += fabs(circularBufferOutL[i]) + fabs(circularBufferOutR[i]);
    //  }
    //  aveIn = sumIn / (2. * (double(circularBufferLenght)));
    //  aveOut = sumOut / (2. * (double(circularBufferLenght)));
    //  diffInOut = sumIn / sumOut;
    //  if (GetUI()) {
    //    if (diffInOut > 4.) {
    //      GetUI()->SetParameterFromGUI(kOutputGain, ToNormalizedParam(AmpToDB(4.), GetParam(kOutputGain)->GetMin(), GetParam(kOutputGain)->GetMax(), GetParam(kOutputGain)->GetShape()));
    //    }
    //    else if (diffInOut < .125) {
    //      GetUI()->SetParameterFromGUI(kOutputGain, ToNormalizedParam(AmpToDB(.125), GetParam(kOutputGain)->GetMin(), GetParam(kOutputGain)->GetMax(), GetParam(kOutputGain)->GetShape()));
    //    }
    //    else {
    //      GetUI()->SetParameterFromGUI(kOutputGain, ToNormalizedParam(AmpToDB(diffInOut), GetParam(kOutputGain)->GetMin(), GetParam(kOutputGain)->GetMax(), GetParam(kOutputGain)->GetShape()));
    //    }
    //  }
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

    // TEST PARAMS
    // -----------

  //case kTestParam1: mTestParam1 = GetParam(paramIdx)->Value(); break;
  //case kTestParam2: mTestParam2 = GetParam(paramIdx)->Value(); break;
  //case kTestParam3: mTestParam3 = GetParam(paramIdx)->Value(); break;
  //case kTestParam4: mTestParam4 = GetParam(paramIdx)->Value(); break;
  //case kTestParam5: mTestParam5 = GetParam(paramIdx)->Value(); break;

  default: break;
  }

}

void SRChannel::OnIdle() {
  mInputMeterBallistics.TransmitData(*this);
  //mGrMeterBallistics.TransmitData(*this);
  mOutputMeterBallistics.TransmitData(*this);
  mScopeBallistics.TransmitData(*this);
}


void SRChannel::InitEffects() {
  mSampleRate = GetSampleRate();
  fInputGain.initGain(mInputGain, mInputGain, double(mSampleRate) / 10., false);
  fOutputGain.initGain(mOutputGain, mOutputGain, double(mSampleRate) / 10., false);
  fPan.initPan(SRPlugins::SRGain::typeSinusodial, mPan, true);

  fEqHpFilterOnepoleL.setFc(mEqHpFreq);
  fEqHpFilter1L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter2L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter3L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter4L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter5L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter6L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter7L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter8L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter9L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter10L.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqLpFilter1L.setFilter(SRPlugins::SRFilters::biquad_lowpass, mEqLpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHfFilterL.setFilter(SRPlugins::SRFilters::biquad_highshelf, mEqHfFreq / mSampleRate, mEqHfQ, mEqHfGain, mSampleRate);
  fEqHmfFilterL.setFilter(SRPlugins::SRFilters::biquad_peak, mEqHmfFreq / mSampleRate, mEqHmfQ, mEqHmfGain, mSampleRate);
  fEqLmfFilterL.setFilter(SRPlugins::SRFilters::biquad_peak, mEqLmfFreq / mSampleRate, mEqLmfQ, mEqLmfGain, mSampleRate);
  fEqLfFilterL.setFilter(SRPlugins::SRFilters::biquad_lowshelf, mEqLfFreq / mSampleRate, mEqLfQ, mEqLfGain, mSampleRate);
  fDcBlockerL.setFc(10. / mSampleRate);


  fEqHpFilterOnepoleR.setFc(mEqHpFreq);
  fEqHpFilter1R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter2R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter3R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter4R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter5R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter6R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter7R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter8R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter9R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHpFilter10R.setFilter(SRPlugins::SRFilters::biquad_highpass, mEqHpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqLpFilter1R.setFilter(SRPlugins::SRFilters::biquad_lowpass, mEqLpFreq / mSampleRate, stQ, 0., mSampleRate);
  fEqHfFilterR.setFilter(SRPlugins::SRFilters::biquad_highshelf, mEqHfFreq / mSampleRate, mEqHfQ, mEqHfGain, mSampleRate);
  fEqHmfFilterR.setFilter(SRPlugins::SRFilters::biquad_peak, mEqHmfFreq / mSampleRate, mEqHmfQ, mEqHmfGain, mSampleRate);
  fEqLmfFilterR.setFilter(SRPlugins::SRFilters::biquad_peak, mEqLmfFreq / mSampleRate, mEqLmfQ, mEqLmfGain, mSampleRate);
  fEqLfFilterR.setFilter(SRPlugins::SRFilters::biquad_lowshelf, mEqLfFreq / mSampleRate, mEqLfQ, mEqLfGain, mSampleRate);
  fDcBlockerR.setFc(10. / mSampleRate);

  fCompressorPeak.initCompressor(mCompPeakThresh, mCompPeakRatio, mCompPeakAttack, mCompPeakRelease, mCompPeakSidechainFilterFreq, mCompPeakKneeWidthDb, mCompPeakIsFeedback, mSampleRate);
  fCompressorPeak.initRuntime();

  // For sidechain filter frequency it requires an own knob later
  fCompressorRms.initCompressor(mCompRmsThresh, mCompRmsRatio, mCompRmsAttack, mCompRmsRelease, mCompPeakSidechainFilterFreq, mCompRmsKneeWidthDb, 300., mCompRmsIsFeedback, mSampleRate);
  fCompressorRms.initRuntime();

  fLimiter.setSampleRate(mSampleRate);
  fLimiter.setAttack(1.);
  fLimiter.setRelease(100.);
  fLimiter.setThresh(mLimiterThresh);
  fLimiter.initRuntime();

  fSafePanHpL.setFilter(SRPlugins::SRFilters::iir_linkwitz_highpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);
  fSafePanLpL.setFilter(SRPlugins::SRFilters::iir_linkwitz_lowpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);
  fSafePanHpR.setFilter(SRPlugins::SRFilters::iir_linkwitz_highpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);
  fSafePanLpR.setFilter(SRPlugins::SRFilters::iir_linkwitz_lowpass, mSafePanFreq / mSampleRate, 0., 0., mSampleRate);

  fDeesser.setDeesser(mDeesserThresh, mDeesserRatio, mDeesserAttack, mDeesserRelease, mDeesserFreq / mSampleRate, mDeesserQ, 10., mSampleRate);
  fDeesser.initRuntime();

  fInputSaturation.setSaturation(SRPlugins::SRSaturation::SaturationTypes::typeMusicDSP, mInputDrive, mSaturationAmount, mSaturationHarmonics, false, mSaturationSkew, 1.);

  //if (GetAPI() == kAPIVST2) // for VST2 we name individual outputs
  //{
    SetChannelLabel(ERoute::kInput, 0, "In Left", true);
    SetChannelLabel(ERoute::kInput, 1, "In Right", true);
    SetChannelLabel(ERoute::kInput, 2, "ExtSC Left", true);
    SetChannelLabel(ERoute::kInput, 3, "ExtSC Right", true);
    SetChannelLabel(ERoute::kOutput, 0, "Out Left", true);
    SetChannelLabel(ERoute::kOutput, 1, "Out Right", true);
  //}
  //else // for AU and VST3 we name buses
  //{
  //  SetChannel
  //  SetInputBusLabel(0, "Input");
  //  SetInputBusLabel(1, "Ext SC");
  //  SetOutputBusLabel(0, "Output");
  //}
}

#endif
