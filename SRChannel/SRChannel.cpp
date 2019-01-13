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
    pGraphics->AttachControl(new IBitmapControl(*this, IRECT(PLUG_WIDTH - 300, 0, PLUG_WIDTH, 70), bmpLogo, -1));
    // Attach section rects
    pGraphics->AttachControl(new IPanelControl(*this, rectInput, pluginLayout.colorBackground, true), cPanelInput, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectEq, pluginLayout.colorBackground, true), cPanelEq, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectComp, pluginLayout.colorBackground, true), cPanelComp, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectPost, pluginLayout.colorBackground, true), cPanelPost, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectOutput, pluginLayout.colorBackground, true), cPanelOutput, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectMeter, COLOR_TRANSPARENT, true), cPanelMeter, "UI");
    //pGraphics->AttachControl(new IVKnobControl(*this, rectPluginWindow.GetCentredInside(100).GetVShifted(-100), kGain));
    //pGraphics->AttachControl(new IVSliderControl(*this, rectInputStage.GetGridCell(1,0,10,2).FracRectVertical(9.f, true), kInputGain, DEFAULT_SPEC, kVertical, true, 32.f, 2.f), cInputGain, "Input");
    //pGraphics->AttachControl(new SRPlugins::SRControls::SRVectorKnobText(*this, rectInputStage.GetGridCell(0, 0, 10, 2), kInputGain, GetParam(kInputGain)->GetNameForHost(), true, DEFAULT_SPEC, pluginLayout.tKnobLabel, pluginLayout.tKnobValue), cInputGain, "Input");

    for (int paramIdx = 0, ctrlIdx = 0; paramIdx < kNumParams; paramIdx++, ctrlIdx++) {
      IParam *param = GetParam(paramIdx);												// ... for which we temporally create a pointer "param"
      IRECT *rect;
      const structParameterProperties &properties = parameterProperties[paramIdx];		// ... and a variable "properties" pointing at the current parameters properties
      switch (properties.AttachToControlPanel) {
      case cPanelHeader: rect = &rectHeader; break;
      case cPanelInput: rect = &rectInput; break;
      case cPanelEq: rect = &rectEq; break;
      case cPanelComp: rect = &rectComp; break;
      case cPanelPost: rect = &rectPost; break;
      case cPanelOutput: rect = &rectOutput; break;
      case cPanelFooter: rect = &rectFooter; break;


      default:
        break;
      }
      pGraphics->AttachControl(new SRPlugins::SRControls::SRVectorKnobText(*this, rectControls.GetGridCell(paramIdx, 10, 12), paramIdx, properties.shortName, true, DEFAULT_SPEC, pluginLayout.tKnobLabel, pluginLayout.tKnobValue), ctrlIdx);
      pGraphics->GetControl(ctrlIdx)->SetTooltip(properties.tooltip);
    }

  };
#endif
}
// END GRAPHICS func



// DSP func
#if IPLUG_DSP
void SRChannel::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s];
    }
  }
}
void SRChannel::OnReset()
{
}
void SRChannel::OnParamChange(int paramIdx)
{
}
void SRChannel::OnIdle()
{
}
#endif
