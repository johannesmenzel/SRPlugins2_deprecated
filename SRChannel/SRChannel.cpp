#include "SRChannel.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "../SRClasses/SRControls.h"
#include "../SRClasses/SRHelpers.h"

SRChannel::SRChannel(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  //GetParam(kGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
  for (int paramIdx = 0; paramIdx < kNumParams; paramIdx++) {
    IParam* param = GetParam(paramIdx);												// ... for which we temporally create a pointer "param"
    const structParameterProperties &properties = parameterProperties[paramIdx];		// ... and a variable "properties" pointing at the current parameters properties
    GetParam(paramIdx)->InitDouble(properties.name, properties.defaultVal, properties.minVal, properties.maxVal, properties.stepValue, properties.label, 0, properties.group, new IParam::ShapePowCurve(SRPlugins::SRHelpers::SetShapeCentered(properties.minVal, properties.maxVal, properties.centerVal, properties.centerPoint)), IParam::kUnitCustom);
  }
  //GetParam(kInputGain)->InitDouble("Input Gain", 0., -60., 12., 0.001, "dB", 0, "Input", new IParam::ShapePowCurve(SRPlugins::SRHelpers::SetShapeCentered(-60., 12., 0., 0.5)), IParam::kUnitDB);

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    // Plugin Colors
    const struct {
      const IColor colorBackground;
      const IColor colorBackground2;
      const IText tKnobLabel;
      const IText tKnobValue;
    } pluginLayout = {
      IColor(255, 37, 53, 69),
      IColor(255, 13, 18, 23),
      IText(DEFAULT_TEXT_SIZE + 5, COLOR_WHITE, DEFAULT_FONT, IText::kStyleBold, IText::kAlignCenter, IText::kVAlignTop, 0, IText::kQualityNonAntiAliased),
      IText(DEFAULT_TEXT_SIZE, COLOR_WHITE, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignCenter, IText::kVAlignBottom, 0, IText::kQualityNonAntiAliased)
    };

    pGraphics->AttachCornerResizer(kUIResizerScale, false);     // Resizer
    pGraphics->AttachPanelBackground(pluginLayout.colorBackground2);               // Background
    pGraphics->LoadFont(ROBOTTO_FN);                          // Font
    pGraphics->HandleMouseOver(true);
    pGraphics->EnableTooltips(true);
    IBitmap bmpLogo = pGraphics->LoadBitmap(LOGO_FN);
    const IRECT rectPlugin = pGraphics->GetBounds();      // Canvas
    // Control Canvas
    const IRECT rectControls = IRECT(0, 70, PLUG_WIDTH, PLUG_HEIGHT - 30);
    const IRECT rectInputStage = rectControls.SubRectHorizontal(6, 0).GetPadded(-5.f);
    const IRECT rectEqStage = rectControls.SubRectHorizontal(6, 1).GetPadded(-5.f);
    const IRECT rectCompStage = rectControls.SubRectHorizontal(6, 2).GetPadded(-5.f);
    const IRECT rectPostStage = rectControls.SubRectHorizontal(6, 3).GetPadded(-5.f);
    const IRECT rectOutputStage = rectControls.SubRectHorizontal(6, 4).GetPadded(-5.f);
    const IRECT rectMeterStage = rectControls.SubRectHorizontal(6, 5).GetPadded(-5.f);
    pGraphics->AttachControl(new IBitmapControl(*this, IRECT(PLUG_WIDTH - 300, 0, PLUG_WIDTH, 70), bmpLogo, -1));
    pGraphics->AttachControl(new IPanelControl(*this, rectInputStage, pluginLayout.colorBackground, true), cPanelInput, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectEqStage, pluginLayout.colorBackground, true), cPanelEq, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectCompStage, pluginLayout.colorBackground, true), cPanelComp, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectPostStage, pluginLayout.colorBackground, true), cPanelPost, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectOutputStage, pluginLayout.colorBackground, true), cPanelOutput, "UI");
    pGraphics->AttachControl(new IPanelControl(*this, rectMeterStage, COLOR_TRANSPARENT, true), cPanelMeter, "UI");
    //pGraphics->AttachControl(new IVKnobControl(*this, rectPluginWindow.GetCentredInside(100).GetVShifted(-100), kGain));
    //pGraphics->AttachControl(new IVSliderControl(*this, rectInputStage.GetGridCell(1,0,10,2).FracRectVertical(9.f, true), kInputGain, DEFAULT_SPEC, kVertical, true, 32.f, 2.f), cInputGain, "Input");
    //pGraphics->AttachControl(new SRPlugins::SRControls::SRVectorKnobText(*this, rectInputStage.GetGridCell(0, 0, 10, 2), kInputGain, GetParam(kInputGain)->GetNameForHost(), true, DEFAULT_SPEC, pluginLayout.tKnobLabel, pluginLayout.tKnobValue), cInputGain, "Input");

    for (int paramIdx = 0, ctrlIdx = 0; paramIdx < kNumParams; paramIdx++, ctrlIdx++) {
      IParam* param = GetParam(paramIdx);												// ... for which we temporally create a pointer "param"
      const structParameterProperties &properties = parameterProperties[paramIdx];		// ... and a variable "properties" pointing at the current parameters properties
      pGraphics->AttachControl(new SRPlugins::SRControls::SRVectorKnobText(*this, rectControls.GetGridCell(paramIdx, 10, 12), paramIdx, properties.name, true, DEFAULT_SPEC, pluginLayout.tKnobLabel, pluginLayout.tKnobValue), ctrlIdx);
      pGraphics->GetControl(ctrlIdx)->SetTooltip(properties.tooltip);
    }
  };
#endif
}

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
