#include "SRControls.h"
//#include "IPlug_include_in_plug_src.h"
//#include "IControls.h"
 
namespace SRPlugins {
	namespace SRControls {

SRVectorKnobText::SRVectorKnobText(IGEditorDelegate& dlg, IRECT bounds, int paramIdx,
                             const char* label, bool displayParamValue,
                             const IVColorSpec& colorSpec, const IText& labelText, const IText& valueText,
                             float aMin, float aMax, float knobFrac,
                             EDirection direction, double gearing)
: IKnobControlBase(dlg, bounds, paramIdx, direction, gearing)
, IVectorBase(colorSpec)
, mAngleMin(aMin)
, mAngleMax(aMax)
, mLabel(label)
, mDisplayParamValue(displayParamValue)
, mLabelText(labelText)
, mKnobFrac(knobFrac)
{
  if(mDisplayParamValue)
    DisablePrompt(false);
  
  mValueText = valueText;
  AttachIControl(this);
}
 
SRVectorKnobText::SRVectorKnobText(IGEditorDelegate& dlg, IRECT bounds, IActionFunction actionFunction,
                             const char* label, bool displayParamValue,
                             const IVColorSpec& colorSpec, const IText& labelText, const IText& valueText,
                             float aMin, float aMax, float knobFrac,
                             EDirection direction, double gearing)
: IKnobControlBase(dlg, bounds, kNoParameter, direction, gearing)
, IVectorBase(colorSpec)
, mAngleMin(aMin)
, mAngleMax(aMax)
, mLabel(label)
, mDisplayParamValue(displayParamValue)
, mLabelText(labelText)
, mKnobFrac(knobFrac)
{
  if(mDisplayParamValue)
    DisablePrompt(false);
  
  mValueText = valueText;
  SetActionFunction(actionFunction);
  AttachIControl(this);
}
 
void SRVectorKnobText::Draw(IGraphics& g)
{
  g.FillRect(GetColor(kBG), mRECT);
 
  const float v = mAngleMin + ((float)mValue * (mAngleMax - mAngleMin));
  const float cx = mHandleBounds.MW(), cy = mHandleBounds.MH();
  const float radius = (mHandleBounds.W()/2.f);
  // Value Arc 
  g.DrawArc(GetColor(kFR), cx, cy, radius + 5.f, mAngleMin, v, 0, 1.f);

  // Shadow
  if(mDrawShadows && !mEmboss) g.FillCircle(GetColor(kSH), cx + mShadowOffset, cy + mShadowOffset, radius);

  // Knob
  g.FillCircle(GetColor(kFG), cx, cy, radius);
 
  // Rim
  g.DrawCircle(GetColor(kON), cx, cy, radius * 0.9f, 0, mFrameThickness);

  
  if(mMouseIsOver)
    g.FillCircle(GetColor(kHL), cx, cy, radius * 0.8f);
  
  g.DrawCircle(GetColor(kFR), cx, cy, radius, 0, mFrameThickness);
  g.DrawRadialLine(GetColor(kFR), cx, cy, v, 0.7f * radius, 0.9f * radius, 0, mFrameThickness);
  
  if(mLabelBounds.H())
    g.DrawText(mLabelText, mLabel.Get(), mLabelBounds);
  
  if(mDisplayParamValue)
  {
    WDL_String str;
    GetParam()->GetDisplayForHost(str);
    
    if (mShowParamLabel)
    {
      str.Append(" ");
      str.Append(GetParam()->GetLabelForHost());
    }
    
    g.DrawText(mValueText, str.Get(), mValueBounds);
  }
  
}
 
void SRVectorKnobText::OnMouseDown(float x, float y, const IMouseMod& mod)
{
  if(mDisplayParamValue && mValueBounds.Contains(x, y))
  {
    PromptUserInput(mValueBounds);
  }
  else
    IKnobControlBase::OnMouseDown(x, y, mod);
}
 
void SRVectorKnobText::OnResize()
{
  IRECT clickableArea;
  
  if(mLabel.GetLength())
  {
    IRECT textRect;
    GetUI()->MeasureText(mLabelText, mLabel.Get(), textRect);
    
    mLabelBounds = mRECT.GetFromTop(textRect.H());
  }
  else
    mLabelBounds = IRECT();
  
  if(mLabelBounds.H())
    clickableArea = mRECT.GetReducedFromTop(mLabelBounds.H());
  else
    clickableArea = mRECT;
 
  if (mDisplayParamValue)
  {
    IRECT textRect;
    WDL_String str;
    GetParam()->GetDisplayForHost(str);
    
    GetUI()->MeasureText(mValueText, str.Get(), textRect);
    
    const float valueDisplayWidth = clickableArea.W() * mKnobFrac * 0.5f;
    switch (mValueText.mVAlign)
    {
      case IText::kVAlignMiddle:
        mHandleBounds = clickableArea;
        mValueBounds = clickableArea.GetMidVPadded(textRect.H()/2.f).GetMidHPadded(valueDisplayWidth);
        break;
      case IText::kVAlignBottom:
      {
        mValueBounds = clickableArea.GetFromBottom(textRect.H()).GetMidHPadded(valueDisplayWidth);
        mHandleBounds = clickableArea.GetReducedFromBottom(textRect.H());
        break;
      }
      case IText::kVAlignTop:
        mValueBounds = clickableArea.GetFromTop(textRect.H()).GetMidHPadded(valueDisplayWidth);
        mHandleBounds = clickableArea.GetReducedFromTop(textRect.H());
        break;
      default:
        break;
    }
    
    if(mValueBounds.W() < textRect.W())
      mValueBounds = mValueBounds.GetMidHPadded(mTargetRECT.W()/2.f);
  }
  
  mHandleBounds = GetAdjustedHandleBounds(mTargetRECT).GetScaledAboutCentre(mKnobFrac);
  
  SetDirty(false);
}

} // End namespace SRControls
} // End namespace SRPlugins