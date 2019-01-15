#include "SRControls.h"
//#include "IPlug_include_in_plug_src.h"
//#include "IControls.h"
 
namespace SRPlugins {
	namespace SRControls {

SRVectorKnobText::SRVectorKnobText(IGEditorDelegate& dlg, IRECT bounds, int paramIdx,
                             const char* label, bool displayParamValue,
                             const IVColorSpec& colorSpec, const IColor& color, const IText& labelText, const IText& valueText,
                             float aMin, float aMax, float knobFrac,
                             EDirection direction, double gearing)
: IKnobControlBase(dlg, bounds, paramIdx, direction, gearing)
, IVectorBase(colorSpec)
, mAngleMin(aMin)
, mAngleMax(aMax)
, mLabel(label)
, mDisplayParamValue(displayParamValue)
, mLabelText(labelText)
, mLabelShadowText(labelText)
, mKnobFrac(knobFrac)
, mColor(color)
{
  if(mDisplayParamValue)
    DisablePrompt(false);
  
  mValueText = valueText;
  AttachIControl(this);
}
 
SRVectorKnobText::SRVectorKnobText(IGEditorDelegate& dlg, IRECT bounds, IActionFunction actionFunction,
                             const char* label, bool displayParamValue,
                             const IVColorSpec& colorSpec, const IColor& color, const IText& labelText, const IText& valueText,
                             float aMin, float aMax, float knobFrac,
                             EDirection direction, double gearing)
: IKnobControlBase(dlg, bounds, kNoParameter, direction, gearing)
, IVectorBase(colorSpec)
, mAngleMin(aMin)
, mAngleMax(aMax)
, mLabel(label)
, mDisplayParamValue(displayParamValue)
, mLabelText(labelText)
, mLabelShadowText(labelText)
, mKnobFrac(knobFrac)
, mColor(color)
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
  const float mAngleDefault = mAngleMin + mDefaultValue * (mAngleMax - mAngleMin);
  const float colorIntensity = fabs(mValue - mDefaultValue) / fmaxf(mDefaultValue, (1.f - mDefaultValue));
  IColor arcColor;
  IColor::LinearInterpolateBetween(GetColor(kBG), mColor, arcColor, 0.5f + 0.5f * colorIntensity);

  SetFrameThickness(radius * 0.05f);
  SetShadowOffset(radius * 0.2f);

  // Text Label
  if (mLabelBounds.H())
    mLabelShadowText.mFGColor = IColor(150, 0, 0, 0);
  g.DrawText(mLabelShadowText, mLabel.Get(), mLabelBounds.GetHShifted(-1.f).GetVShifted(-1.f));
  g.DrawText(mLabelText, mLabel.Get(), mLabelBounds);

  // Text Value
  if (mDisplayParamValue)
  {
    WDL_String str;
    GetParam()->GetDisplayForHost(str);

    if (mShowParamLabel)
    {
      str.Append(" ");
      str.Append(GetParam()->GetLabelForHost());
    }

    g.FillRoundRect(GetColor(kFR), mValueBounds, 2.f, 0);
    g.DrawText(mValueText, str.Get(), mValueBounds);
  }


  // Value Arc
  if (v <= mAngleDefault) {
    g.DrawArc(arcColor, cx, cy, radius, v, mAngleDefault, 0, 4.f * mFrameThickness);
  }
  else {
    g.DrawArc(arcColor, cx, cy, radius, mAngleDefault, v, 0, 4.f * mFrameThickness);
  }

  // Frame
  //g.DrawCircle(GetColor(kFR), cx, cy, radius * 0.9f, 0, mFrameThickness);

  if (mDrawShadows && !mEmboss) {
  // Shadow
  g.FillCircle(GetColor(kSH), cx + mShadowOffset, cy + mShadowOffset, radius * 0.875f);
  // Outer Rim
  g.DrawArc(GetColor(kSH), cx + mShadowOffset, cy + mShadowOffset, radius * 0.9f, v + 5.f, v + 55.f, 0, mFrameThickness);
  g.DrawArc(GetColor(kSH), cx + mShadowOffset, cy + mShadowOffset, radius * 0.9f, v + 65.f, v + 115.f, 0, mFrameThickness);
  g.DrawArc(GetColor(kSH), cx + mShadowOffset, cy + mShadowOffset, radius * 0.9f, v + 125.f, v + 175.f, 0, mFrameThickness);
  g.DrawArc(GetColor(kSH), cx + mShadowOffset, cy + mShadowOffset, radius * 0.9f, v + 185.f, v + 235.f, 0, mFrameThickness);
  g.DrawArc(GetColor(kSH), cx + mShadowOffset, cy + mShadowOffset, radius * 0.9f, v + 245.f, v + 295.f, 0, mFrameThickness);
  g.DrawArc(GetColor(kSH), cx + mShadowOffset, cy + mShadowOffset, radius * 0.9f, v + 305.f, v + 355.f, 0, mFrameThickness);
  }

  // Rim
  g.DrawCircle(GetColor(kON), cx, cy, radius * 0.825f, 0, 2.f * mFrameThickness);
  // Outer Rim
  g.DrawArc(GetColor(kON), cx, cy, radius * 0.9f, v + 5.f, v + 55.f, 0, mFrameThickness);
  g.DrawArc(GetColor(kON), cx, cy, radius * 0.9f, v + 65.f, v + 115.f, 0, mFrameThickness);
  g.DrawArc(GetColor(kON), cx, cy, radius * 0.9f, v + 125.f, v + 175.f, 0, mFrameThickness);
  g.DrawArc(GetColor(kON), cx, cy, radius * 0.9f, v + 185.f, v + 235.f, 0, mFrameThickness);
  g.DrawArc(GetColor(kON), cx, cy, radius * 0.9f, v + 245.f, v + 295.f, 0, mFrameThickness);
  g.DrawArc(GetColor(kON), cx, cy, radius * 0.9f, v + 305.f, v + 355.f, 0, mFrameThickness);


  // Head shadow and head
  if (mDrawShadows && !mEmboss) g.FillCircle(GetColor(kSH), cx + mFrameThickness, cy + mFrameThickness, radius * 0.8f);
  g.FillCircle(mColor, cx, cy, (radius * 0.8f), 0);

  // Knob Lights
  if (!mEmboss) {
  g.DrawArc(IColor(70, 0, 0, 0), cx, cy, (radius * 0.775f), 45.f, -135.f, 0, radius * 0.05f);
  g.DrawArc(IColor(100, 255, 255, 255), cx, cy, (radius * 0.775f), -180.f, 90.f, 0, radius * 0.05f);
  }

  // Outer Arrow
  g.DrawRadialLine(GetColor(kFR), cx, cy, v, 0.7f * radius, radius * 0.9f, 0, 3.f * mFrameThickness);

  // Arrow and it's shadow
  g.DrawRadialLine(GetColor(kSH), cx + mFrameThickness, cy + mFrameThickness, v, 0.f, 0.8f * radius, 0, 3.f * mFrameThickness);
  g.FillCircle(GetColor(kSH), cx + mFrameThickness, cy + mFrameThickness, 1.5f * mFrameThickness, 0);

  if (mColor.B + mColor.R + mColor.G > 600) {
    g.DrawRadialLine(GetColor(kFR), cx, cy, v, 0.f, 0.8f * radius, 0, 3.f * mFrameThickness);
    g.FillCircle(GetColor(kFR), cx, cy, 1.5f * mFrameThickness, 0);
  }
  else {
    g.DrawRadialLine(GetColor(kON), cx, cy, v, 0.f, 0.8f * radius, 0, 3.f * mFrameThickness);
    g.FillCircle(GetColor(kON), cx, cy, 1.5f * mFrameThickness, 0);
  }



  // Test POLYGON
  //float polygon[2][2] = { {cx - radius, cx}, {cy - radius, cy} };
  //float* pointx = &polygon[0][0];
  //float* pointy = &polygon[1][0];
  //g.DrawConvexPolygon(GetColor(kON), pointx, pointy, 2, 0, 3.f);
 
  
  if(mMouseIsOver)
    g.FillCircle(GetColor(kHL), cx, cy, radius * 0.8f);


  
  
  
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