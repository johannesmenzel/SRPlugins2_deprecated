#include "SRChannel.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "../SRClasses/SRControls.h"
#include "../SRClasses/SRHelpers.h"

SRChannel::SRChannel(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo) {
  // Initialize Parameters
  for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {
    IParam* param = GetParam(paramIdx);												// ... for which we temporally create a pointer "param"
    const structParameterProperties &properties = parameterProperties[paramIdx];		// ... and a variable "properties" pointing at the current parameters properties
    GetParam(paramIdx)->InitDouble(properties.name, properties.defaultVal, properties.minVal, properties.maxVal, properties.stepValue, properties.label, 0, properties.group, new IParam::ShapePowCurve(SRPlugins::SRHelpers::SetShapeCentered(properties.minVal, properties.maxVal, properties.centerVal, properties.centerPoint)), IParam::kUnitCustom);
  }
  //GetParam(kInputGain)->InitDouble("Input Gain", 0., -60., 12., 0.001, "dB", 0, "Input", new IParam::ShapePowCurve(SRPlugins::SRHelpers::SetShapeCentered(-60., 12., 0., 0.5)), IParam::kUnitDB);



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

    //pGraphics->AttachControl(new IVKnobControl(*this, rectPluginWindow.GetCentredInside(100).GetVShifted(-100), kGain));
    //pGraphics->AttachControl(new SRPlugins::SRControls::SRVectorKnobText(*this, rectInputStage.GetGridCell(0, 0, 10, 2), kInputGain, GetParam(kInputGain)->GetNameForHost(), true, DEFAULT_SPEC, pluginLayout.tKnobLabel, pluginLayout.tKnobValue), cInputGain, "Input");

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
        pGraphics->AttachControl(new IVSwitchControl(*this, rect->GetGridCell(properties.y, properties.x, panelGridRowsAndCols[0], panelGridRowsAndCols[1]), paramIdx, FlashCircleClickActionFunc, properties.shortName, DEFAULT_SPEC, (int)properties.maxVal + 1), ctrlIdx);
        break;
      default:
        break;
      }
      pGraphics->GetControl(paramIdx)->SetMOWhenGrayed(true);
      pGraphics->GetControl(paramIdx)->SetTooltip(properties.tooltip);
    }

  };
#endif
}
// END GRAPHICS func



// DSP func
#if IPLUG_DSP

void SRChannel::ProcessBlock(sample** inputs, sample** outputs, int nFrames) {

  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s];
    }
  }

  mInputMeterBallistics.ProcessBlock(inputs, nFrames);
  //mGrMeterBallistics.ProcessBlock(!!!sample **GAINREDUCTION!!!, nFrames);
  mOutputMeterBallistics.ProcessBlock(outputs, nFrames);
  mScopeBallistics.ProcessBlock(outputs, nFrames);
}

void SRChannel::OnReset() {

}

void SRChannel::OnParamChange(int paramIdx) {

}

void SRChannel::OnIdle() {
  mInputMeterBallistics.TransmitData(*this);
  //mGrMeterBallistics.TransmitData(*this);
  mOutputMeterBallistics.TransmitData(*this);
  mScopeBallistics.TransmitData(*this);
}

#endif
