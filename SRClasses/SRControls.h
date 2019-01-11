#pragma once
#include "IPlug_include_in_plug_hdr.h"
//#include <string>
 
namespace SRPlugins {
 
	namespace SRControls {
 

/** A vector knob control drawn using graphics primitves */
class IVKnobControl : public IKnobControlBase
                    , public IVectorBase
{
public:
  IVKnobControl(IGEditorDelegate& dlg, IRECT bounds, int paramIdx,
                const char* label = "", bool displayParamValue = false,
                const IVColorSpec& colorSpec = DEFAULT_SPEC, const IText& labelText = IText(DEFAULT_TEXT_SIZE + 5, IText::kVAlignTop), const IText& valueText = IText(DEFAULT_TEXT_SIZE, IText::kVAlignBottom),
                float aMin = -135.f, float aMax = 135.f, float knobFrac = 0.50f,
                EDirection direction = kVertical, double gearing = DEFAULT_GEARING);

  IVKnobControl(IGEditorDelegate& dlg, IRECT bounds, IActionFunction actionFunction,
                const char* label = "", bool displayParamValue = false,
                const IVColorSpec& colorSpec = DEFAULT_SPEC, const IText& labelText = IText(DEFAULT_TEXT_SIZE + 5, IText::kVAlignTop), const IText& valueText = IText(DEFAULT_TEXT_SIZE, IText::kVAlignBottom),
                float aMin = -135.f, float aMax = 135.f, float knobFrac = 0.50f,
                EDirection direction = kVertical, double gearing = DEFAULT_GEARING);

  virtual ~IVKnobControl() {}

  void Draw(IGraphics& g) override;
  void OnMouseDown(float x, float y, const IMouseMod& mod) override;
//  void OnMouseDblClick(float x, float y, const IMouseMod& mod) override {  OnMouseDown(x, y, mod); }
  void OnResize() override;
protected:
  bool mDisplayParamValue;
  bool mShowParamLabel = true;
  IRECT mHandleBounds;
  IRECT mLabelBounds;
  IRECT mValueBounds;
  float mAngleMin, mAngleMax;
  float mKnobFrac;
  WDL_String mLabel;
  IText mLabelText;
  IText& mValueText = mText;
};

	} // End namespace SRControls
} // End namespace SRPlugins