#include "SRControls.h"
//#include "IPlug_include_in_plug_src.h"
//#include "IControls.h"

namespace SRPlugins {
  namespace SRControls {


    // KNOB
    // ----


    SRVectorKnobText::SRVectorKnobText(IRECT bounds, int paramIdx,
      const char* label, const char* minLabel, const char* maxLabel, const char* ctrLabel, bool drawCircleLabels, bool displayParamValue,
      const IVColorSpec& colorSpec, const IColor& color, const IText& labelText, const IText& valueText,
      float aMin, float aMax, float aDef, float knobFrac,
      EDirection direction, double gearing)
      : IKnobControlBase(bounds, paramIdx, direction, gearing)
      , IVectorBase(colorSpec)
      , mAngleMin(aMin)
      , mAngleMax(aMax)
      , mAngleDefault(aMin + aDef * (aMax - aMin))
      , mLabel(label)
      , mLabelMin(minLabel)
      , mLabelMax(maxLabel)
      , mLabelCtr(ctrLabel)
      , mDrawCircleLabels(drawCircleLabels)
      , mDisplayParamValue(displayParamValue)
      , mLabelText(labelText)
      , mKnobFrac(knobFrac)
      , mColor(color)
      , mTextCircleLabelMin(IText(10, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignFar))
      , mTextCircleLabelMax(IText(10, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignNear))
      , mTextCircleLabelCtr(IText(10, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignCenter))

    {
      if (mDisplayParamValue) DisablePrompt(false);
      mValueText = valueText;
      AttachIControl(this);
    }

    SRVectorKnobText::SRVectorKnobText(IRECT bounds, IActionFunction actionFunction,
      const char* label, const char* minLabel, const char* maxLabel, const char* ctrLabel, bool drawCircleLabels, bool displayParamValue,
      const IVColorSpec& colorSpec, const IColor& color, const IText& labelText, const IText& valueText,
      float aMin, float aMax, float aDef, float knobFrac,
      EDirection direction, double gearing)
      : IKnobControlBase(bounds, kNoParameter, direction, gearing)
      , IVectorBase(colorSpec)
      , mAngleMin(aMin)
      , mAngleMax(aMax)
      , mAngleDefault(aMin + aDef * (aMax - aMin))
      , mLabel(label)
      , mLabelMin(minLabel)
      , mLabelMax(maxLabel)
      , mLabelCtr(ctrLabel)
      , mDrawCircleLabels(drawCircleLabels)
      , mDisplayParamValue(displayParamValue)
      , mLabelText(labelText)
      , mKnobFrac(knobFrac)
      , mColor(color)
    {
      if (mDisplayParamValue) DisablePrompt(false);
      mValueText = valueText;
      SetActionFunction(actionFunction);
      AttachIControl(this);
    }

    void SRVectorKnobText::OnInit()
    {

    }

    void SRVectorKnobText::Draw(IGraphics& g) {
      const float mAngleValue = mAngleMin + ((float)mValue * (mAngleMax - mAngleMin));
      const float colorIntensity = fabs(mValue - mDefaultValue) / fmaxf(mDefaultValue, (1.f - mDefaultValue));
      IColor arcColor;
      IColor::LinearInterpolateBetween(GetColor(kBG), mColor, arcColor, 0.5f + 0.5f * colorIntensity);
      const IText circleLabelMinText = IText(mText.mSize, IText::kAlignFar);
      const IText circleLabelMaxText = IText(mText.mSize, IText::kAlignFar);
      const IText circleLabelCtrText = IText(mText.mSize, IText::kAlignFar);

      // Background
      //g.FillRect(GetColor(kBG), mRECT);

      // Text Label
      if (mLabelBounds.H()) {
        if (!mMouseIsOver)
          g.DrawText(mLabelText, mLabel.Get(), mValueBounds);
        else {
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

            //g.FillRoundRect(GetColor(kFR), mValueBounds, 2.f, 0);
            g.DrawText(mValueText, str.Get(), mValueBounds);
          }
        }
      }

      // Draw Circle Labels
      if (mDrawCircleLabels) {
        if (mMinLabelBounds.H()) {
          g.DrawText(mTextCircleLabelMin, mLabelMin.Get(), mMinLabelBounds, 0);
        }
        if (mMaxLabelBounds.H()) {
          g.DrawText(mTextCircleLabelMax, mLabelMax.Get(), mMaxLabelBounds, 0);
        }
        if (mCtrLabelBounds.H()) {
          g.DrawText(mTextCircleLabelCtr, mLabelCtr.Get(), mCtrLabelBounds, 0);
        }
      }


      // Value Arc
      if (mAngleValue <= mAngleDefault) g.DrawArc(arcColor, mCenterX, mCenterY, knobScales.valArc.relRadius * mRadius, mAngleValue, mAngleDefault, 0, knobScales.valArc.relThickness * mRelThickness);
      else g.DrawArc(arcColor, mCenterX, mCenterY, mRadius, mAngleDefault, mAngleValue, 0, 4.f * mRelThickness);

      // Dots
      for (int i = 0; i <= 10; i++) {
        g.DrawRadialLine(COLOR_MID_GRAY, mCenterX, mCenterY, mAngleMin + (float(i) * 0.1f) * (mAngleMax - mAngleMin), mRadius, mRadius + knobScales.dots.relThickness * mRelThickness, 0, knobScales.dots.relThickness * mRelThickness);
      }



      // Frame
      //g.DrawCircle(GetColor(kFR), mCenterX, mCenterY, mRadius * 0.9f, 0, mRelThickness);

      // Rim Shadows
      if (mDrawShadows && !mEmboss) {
        // Rim Shadow
        g.FillCircle(GetColor(kSH), mCenterX + mRelShadowOffset, mCenterY + mRelShadowOffset, mRadius * knobScales.rim.relRadius);
        // Outer Rim Shadow
        g.DrawArc(GetColor(kSH), mCenterX + mRelShadowOffset, mCenterY + mRelShadowOffset, mRadius * knobScales.outerRim.relRadius, mAngleValue + 5.f, mAngleValue + 55.f, 0, knobScales.outerRim.relThickness * mRelThickness);
        g.DrawArc(GetColor(kSH), mCenterX + mRelShadowOffset, mCenterY + mRelShadowOffset, mRadius * knobScales.outerRim.relRadius, mAngleValue + 65.f, mAngleValue + 115.f, 0, knobScales.outerRim.relThickness * mRelThickness);
        g.DrawArc(GetColor(kSH), mCenterX + mRelShadowOffset, mCenterY + mRelShadowOffset, mRadius * knobScales.outerRim.relRadius, mAngleValue + 125.f, mAngleValue + 175.f, 0, knobScales.outerRim.relThickness * mRelThickness);
        g.DrawArc(GetColor(kSH), mCenterX + mRelShadowOffset, mCenterY + mRelShadowOffset, mRadius * knobScales.outerRim.relRadius, mAngleValue + 185.f, mAngleValue + 235.f, 0, knobScales.outerRim.relThickness * mRelThickness);
        g.DrawArc(GetColor(kSH), mCenterX + mRelShadowOffset, mCenterY + mRelShadowOffset, mRadius * knobScales.outerRim.relRadius, mAngleValue + 245.f, mAngleValue + 295.f, 0, knobScales.outerRim.relThickness * mRelThickness);
        g.DrawArc(GetColor(kSH), mCenterX + mRelShadowOffset, mCenterY + mRelShadowOffset, mRadius * knobScales.outerRim.relRadius, mAngleValue + 305.f, mAngleValue + 355.f, 0, knobScales.outerRim.relThickness * mRelThickness);
      }

      // Rim
      g.FillCircle(GetColor(kON), mCenterX, mCenterY, mRadius * knobScales.rim.relRadius, 0);
      // Outer Rim
      g.FillArc(GetColor(kON), mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 5.f, mAngleValue + 55.f, 0);
      g.FillArc(GetColor(kON), mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 65.f, mAngleValue + 115.f, 0);
      g.FillArc(GetColor(kON), mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 125.f, mAngleValue + 175.f, 0);
      g.FillArc(GetColor(kON), mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 185.f, mAngleValue + 235.f, 0);
      g.FillArc(GetColor(kON), mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 245.f, mAngleValue + 295.f, 0);
      g.FillArc(GetColor(kON), mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 305.f, mAngleValue + 355.f, 0);


      // Head shadow and head
      if (mDrawShadows && !mEmboss) g.FillCircle(GetColor(kSH), mCenterX + mRelThickness, mCenterY + mRelThickness, mRadius * knobScales.head.relRadius);
      g.FillCircle(mColor, mCenterX, mCenterY, (mRadius * knobScales.head.relRadius), 0);

      // Knob Lights
      if (!mEmboss) {
        g.DrawArc(IColor(70, 0, 0, 0), mCenterX, mCenterY, (mRadius * knobScales.headLights.relRadius), 45.f, -135.f, 0, mRadius * knobScales.headLights.relThickness);
        g.DrawArc(IColor(100, 255, 255, 255), mCenterX, mCenterY, (mRadius * knobScales.headLights.relRadius), -180.f, 90.f, 0, mRadius * knobScales.headLights.relThickness);
      }

      // Outer Arrow
      g.DrawRadialLine(GetColor(kFR), mCenterX, mCenterY, mAngleValue, knobScales.outerArrow.relInnerRadius * mRadius, mRadius * knobScales.outerArrow.relRadius, 0, knobScales.outerArrow.relThickness * mRelThickness);

      // Arrow shadow
      g.DrawRadialLine(GetColor(kSH), mCenterX + mRelThickness, mCenterY + mRelThickness, mAngleValue, knobScales.arrow.relInnerRadius, knobScales.arrow.relRadius * mRadius, 0, knobScales.arrow.relThickness * mRelThickness);
      g.FillCircle(GetColor(kSH), mCenterX + mRelThickness, mCenterY + mRelThickness, knobScales.arrow.relThickness * mRelThickness * 0.5f, 0);

      // Arrow
      if (mColor.B + mColor.R + mColor.G > 600) {
        g.DrawRadialLine(GetColor(kFR), mCenterX, mCenterY, mAngleValue, knobScales.arrow.relInnerRadius, knobScales.arrow.relRadius * mRadius, 0, knobScales.arrow.relThickness * mRelThickness);
        g.FillCircle(GetColor(kFR), mCenterX, mCenterY, knobScales.arrow.relThickness * mRelThickness * 0.5f, 0);
      }
      else {
        g.DrawRadialLine(GetColor(kON), mCenterX, mCenterY, mAngleValue, knobScales.arrow.relInnerRadius, knobScales.arrow.relRadius * mRadius, 0, knobScales.arrow.relThickness * mRelThickness);
        g.FillCircle(GetColor(kON), mCenterX, mCenterY, knobScales.arrow.relThickness * mRelThickness * 0.5f, 0);
      }

      // Mouseover
      if (mMouseIsOver) g.FillCircle(GetColor(kHL), mCenterX, mCenterY, mRadius * 0.8f);

    }

    void SRVectorKnobText::OnMouseDown(float x, float y, const IMouseMod& mod) {
      if (mDisplayParamValue && mValueBounds.Contains(x, y))
      {
        PromptUserInput(mValueBounds);
      }
      else {
        IKnobControlBase::OnMouseDown(x, y, mod);
      }
    }





    void SRVectorKnobText::OnResize() {
      IRECT clickableArea;

      if (mLabel.GetLength())
      {
        IRECT textRect;
        GetUI()->MeasureText(mLabelText, mLabel.Get(), textRect);

        mLabelBounds = mRECT.GetFromTop(textRect.H());
      }
      else
        mLabelBounds = IRECT();

      if (mLabelBounds.H())
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
          mValueBounds = clickableArea.GetMidVPadded(textRect.H() / 2.f).GetMidHPadded(valueDisplayWidth);
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

        if (mValueBounds.W() < textRect.W())
          mValueBounds = mValueBounds.GetMidHPadded(mTargetRECT.W() / 2.f);
      }

      mHandleBounds = GetAdjustedHandleBounds(mTargetRECT).GetScaledAboutCentre(mKnobFrac);

      mRadius = (mHandleBounds.W() / 2.f);
      mCenterX = mHandleBounds.MW();
      mCenterY = mHandleBounds.MH();
      mRelThickness = mRadius * mFrameThickness * 0.05f / 2.f;
      mRelShadowOffset = mRadius * mShadowOffset * 0.2f / 3.f;

      if (mDrawCircleLabels) {
        if (mLabelMin.GetLength()) {
          mTextCircleLabelMin.mSize = mRadius * 0.5f;
          IRECT textRect;
          GetUI()->MeasureText(mTextCircleLabelMin, mLabelMin.Get(), textRect);
          mMinLabelBounds = IRECT(mHandleBounds.L, float(mValueBounds.T - textRect.H()), mValueBounds.L, mValueBounds.T);
        }
        if (mLabelMax.GetLength()) {
          mTextCircleLabelMax.mSize = mRadius * 0.5f;
          IRECT textRect;
          GetUI()->MeasureText(mTextCircleLabelMax, mLabelMax.Get(), textRect);
          mMaxLabelBounds = IRECT(mValueBounds.R, float(mValueBounds.T - textRect.H()), mHandleBounds.R, mValueBounds.T);
        }
        // Center Label need rework
        if (mLabelCtr.GetLength()) {
          mTextCircleLabelCtr.mSize = mRadius * 0.5f;
          IRECT textRect;
          GetUI()->MeasureText(mTextCircleLabelCtr, mLabelCtr.Get(), textRect);
          mCtrLabelBounds = IRECT(mRECT.L, mRECT.T, mRECT.R, (mRECT.T + textRect.H()));
        }
      }


      SetDirty(false);
    }



    // SWITCH
    // ------

    SRVectorSwitch::SRVectorSwitch(IRECT bounds, int paramIdx, IActionFunction actionFunc
      , const char* str, const IVColorSpec& colorSpec, int numStates)
      : ISwitchControlBase(bounds, paramIdx, actionFunc, numStates)
      , IVectorBase(colorSpec)
    {
      AttachIControl(this);
      mDblAsSingleClick = true;
      mStr.Set(str);
    }

    void SRVectorSwitch::SetDirty(bool push)
    {
      IControl::SetDirty(push);

      const IParam* pParam = GetParam();

      if (pParam)
        pParam->GetDisplayForHost(mStr);
    }

    void SRVectorSwitch::Draw(IGraphics& g)
    {
      IRECT handleBounds = DrawVectorButton(g, mRECT, mMouseDown, mMouseIsOver);

      if (CStringHasContents(mStr.Get()))
        g.DrawText(mText, mStr.Get(), handleBounds);
    }



    void SRLogo::SetStr(const char* str)
    {
      if (strcmp(mStr.Get(), str))
      {
        mStr.Set(str);
        SetDirty(false);
      }
    }

    void SRLogo::SetStrFmt(int maxlen, const char* fmt, ...)
    {
      va_list arglist;
      va_start(arglist, fmt);
      mStr.SetAppendFormattedArgs(false, maxlen, fmt, arglist);
      va_end(arglist);
      SetDirty(false);
    }

    void SRLogo::Draw(IGraphics& g)
    {
      g.FillRect(mBGColor, mRECT);

      if (mStr.GetLength())
        g.DrawText(mText, mStr.Get(), mRECT);
    }

  } // End namespace SRControls
} // End namespace SRPlugins