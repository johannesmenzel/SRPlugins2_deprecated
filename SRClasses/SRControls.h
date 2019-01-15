#pragma once
#include "IPlug_include_in_plug_hdr.h"
#include "IControl.h"
//#include <string>
 
namespace SRPlugins {
 
	namespace SRControls {
 

/** A vector knob control drawn using graphics primitves */
class SRVectorKnobText : public IKnobControlBase
                    , public IVectorBase
{
public:
  SRVectorKnobText(IGEditorDelegate& dlg, IRECT bounds, int paramIdx,
                const char* label = "", bool displayParamValue = false,
                const IVColorSpec& colorSpec = DEFAULT_SPEC, const IColor& color = DEFAULT_FGCOLOR, const IText& labelText = IText(DEFAULT_TEXT_SIZE + 5, IText::kVAlignTop), const IText& valueText = IText(DEFAULT_TEXT_SIZE, IText::kVAlignBottom),
                float aMin = -135.f, float aMax = 135.f, float knobFrac = 0.50f,
                EDirection direction = kVertical, double gearing = DEFAULT_GEARING);

  SRVectorKnobText(IGEditorDelegate& dlg, IRECT bounds, IActionFunction actionFunction,
                const char* label = "", bool displayParamValue = false,
                const IVColorSpec& colorSpec = DEFAULT_SPEC, const IColor& color = DEFAULT_FGCOLOR, const IText& labelText = IText(DEFAULT_TEXT_SIZE + 5, IText::kVAlignTop), const IText& valueText = IText(DEFAULT_TEXT_SIZE, IText::kVAlignBottom),
                float aMin = -135.f, float aMax = 135.f, float knobFrac = 0.50f,
                EDirection direction = kVertical, double gearing = DEFAULT_GEARING);

  virtual ~SRVectorKnobText() {}

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
  IText mLabelShadowText;
  IText& mValueText = mText;
  IColor mColor;
};


/** A vector switch control. Click to cycle through states. */
class SRVectorSwitch : public ISwitchControlBase
  , public IVectorBase
{
public:
  SRVectorSwitch(IGEditorDelegate& dlg, IRECT bounds, int paramIdx = kNoParameter, IActionFunction actionFunc = FlashCircleClickActionFunc,
    const char* label = "", const IVColorSpec& colorSpec = DEFAULT_SPEC, int numStates = 2);

  void Draw(IGraphics& g) override;

  void SetDirty(bool push) override;

protected:
  WDL_String mStr;
};


/** A basic control to display some text */
class SRLogo : public IControl
{
public:
  SRLogo(IGEditorDelegate& dlg, IRECT bounds, const char* str = "", const IText& text = DEFAULT_TEXT, const IColor& BGColor = DEFAULT_BGCOLOR)
    : IControl(dlg, bounds)
    , mStr(str)
    , mBGColor(BGColor)
  {
    mIgnoreMouse = true;
    IControl::mText = text;
  }

  void Draw(IGraphics& g) override;

  virtual void SetStr(const char* str);
  virtual void SetStrFmt(int maxlen, const char* fmt, ...);
  virtual void ClearStr() { SetStr(""); }

protected:
  WDL_String mStr;
  IColor mBGColor;
};


/** Control to test drawing gradients with path based drawing backends
 *   @ingroup TestControls */
class TestGradientControl : public IKnobControlBase
{
public:
  TestGradientControl(IGEditorDelegate& dlg, IRECT rect, int paramIdx = kNoParameter)
    : IKnobControlBase(dlg, rect, paramIdx)
  {
    SetTooltip("TestGradientControl");
    RandomiseGradient();
  }

  void Draw(IGraphics& g) override
  {
    if (g.HasPathSupport())
    {
      double cr = mValue * (mRECT.H() / 2.0);
      g.PathRoundRect(mRECT.GetPadded(-2), cr);
      IFillOptions fillOptions;
      IStrokeOptions strokeOptions;
      fillOptions.mPreserve = true;
      g.PathFill(mPattern, fillOptions);
      g.PathStroke(IColor(255, 0, 0, 0), 3, strokeOptions);
    }
    else
      g.DrawText(mText, "UNSUPPORTED", mRECT);
  }

  void OnMouseDown(float x, float y, const IMouseMod& mod) override
  {
    RandomiseGradient();
    SetDirty(false);
  }

  void RandomiseGradient()
  {
    //IPattern tmp(kLinearPattern);
    //tmp.SetTransform(1.0/mRECT.W(), 0, 0, 1.0/mRECT.W(), 1.0/mRECT.W()*-mRECT.L, 1.0/mRECT.W()*-mRECT.T);
    IPattern tmp(kSolidPattern);

    if (std::rand() & 0x100)
      tmp = IPattern::CreateRadialGradient(mRECT.MW(), mRECT.MH(), mRECT.MH());
    else
      tmp = IPattern::CreateLinearGradient(mRECT.L, mRECT.MH(), mRECT.L + mRECT.W() * 0.5, mRECT.MH());

    tmp.mExtend = (std::rand() & 0x10) ? ((std::rand() & 0x1000) ? kExtendNone : kExtendPad) : ((std::rand() & 0x1000) ? kExtendRepeat : kExtendReflect);

    tmp.AddStop(IColor::GetRandomColor(), 0.0);
    tmp.AddStop(IColor::GetRandomColor(), 0.1);
    tmp.AddStop(IColor::GetRandomColor(), 0.4);
    tmp.AddStop(IColor::GetRandomColor(), 0.6);
    tmp.AddStop(IColor::GetRandomColor(), 1.0);

    mPattern = tmp;
  }

private:
  IPattern mPattern = IPattern(kLinearPattern);
};

	} // End namespace SRControls
} // End namespace SRPlugins