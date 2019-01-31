#include "SRControls.h"
//#include "IPlug_include_in_plug_src.h"
//#include "IControls.h"

namespace SR {
  namespace Graphics {



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
      , mPatternShadow(IPattern(EPatternType::kSolidPattern))
      , mPatternHead(IPattern(EPatternType::kRadialPattern))
      , mPatternHeadLights(IPattern(EPatternType::kRadialPattern))
      , mPatternRim(IPattern(EPatternType::kLinearPattern))
      , mPatternEdge(IPattern(EPatternType::kLinearPattern))
      , mShadowFrame(IShadow(mPatternShadow, 1.f, mShadowOffset, mShadowOffset, 0.5f, true))
      , mShadowHead(IShadow(mPatternShadow, 1.f, mShadowOffset, mShadowOffset, 0.5f, true))
      , mShadowArrow(IShadow(mPatternShadow, 1.f, mShadowOffset, mShadowOffset, 0.5f, true))

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
      , mTextCircleLabelMin(IText(10, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignFar))
      , mTextCircleLabelMax(IText(10, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignNear))
      , mTextCircleLabelCtr(IText(10, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignCenter))
      , mPatternShadow(IPattern(EPatternType::kLinearPattern))
      , mPatternHead(IPattern(EPatternType::kRadialPattern))
      , mPatternHeadLights(IPattern(EPatternType::kRadialPattern))
      , mPatternRim(IPattern(EPatternType::kLinearPattern))
      , mPatternEdge(IPattern(EPatternType::kLinearPattern))
      , mShadowFrame(IShadow(mPatternShadow, 1.f, mShadowOffset, mShadowOffset, 0.5f, true))
      , mShadowHead(IShadow(mPatternShadow, 1.f, mShadowOffset, mShadowOffset, 0.5f, true))
      , mShadowArrow(IShadow(mPatternShadow, 1.f, mShadowOffset, mShadowOffset, 0.5f, true))
    {
      if (mDisplayParamValue) DisablePrompt(false);
      mValueText = valueText;
      SetActionFunction(actionFunction);
      AttachIControl(this);
    }

    //void SRVectorKnobText::OnInit()
    //{

    //}

    void SRVectorKnobText::Draw(IGraphics& g) {
      const float mAngleValue = mAngleMin + ((float)mValue * (mAngleMax - mAngleMin));
      const float colorIntensity = fabsf((float)mValue - (float)mDefaultValue) / fmaxf((float)mDefaultValue, (1.f - (float)mDefaultValue));
      IColor arcColor;
      IColor::LinearInterpolateBetween(GetColor(kBG), mColor, arcColor, 0.5f + 0.5f * colorIntensity);
      IStrokeOptions strokeOptions;
      strokeOptions.mPreserve = true;
      IFillOptions fillOptions;
      fillOptions.mPreserve = true;

      // Background
      //g.FillRect(GetColor(kBG), mRECT);

      // Value Arc
      if (mAngleValue <= mAngleDefault) g.DrawArc(arcColor, mCenterX, mCenterY, knobScales.valArc.relRadius * mRadius, mAngleValue, mAngleDefault, 0, knobScales.valArc.relThickness * mRelThickness);
      else g.DrawArc(arcColor, mCenterX, mCenterY, mRadius, mAngleDefault, mAngleValue, 0, 4.f * mRelThickness);

      // Dots
      for (int i = 0; i <= 10; i++) {
        g.DrawRadialLine(COLOR_MID_GRAY, mCenterX, mCenterY, mAngleMin + (float(i) * 0.1f) * (mAngleMax - mAngleMin), mRadius, mRadius + knobScales.dots.relThickness * mRelThickness, 0, knobScales.dots.relThickness * mRelThickness);
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

      // Text Label
      if (mLabelBounds.H()) {
        if (!mMouseIsOver && !mGrayed)
          g.DrawText(mLabelText, mLabel.Get(), mLabelBounds);
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

      g.StartLayer(mRECT);
      // Draw Frame
      //g.DrawCircle(GetColor(kFR), mCenterX, mCenterY, mRadius * 0.9f, 0, mRelThickness);

      // Draw Rim
      g.PathClear();
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 7.f, mAngleValue + 53.f);
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.rim.relRadius, mAngleValue + 55.f, mAngleValue + 65.f);
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 67.f, mAngleValue + 113.f);
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.rim.relRadius, mAngleValue + 115.f, mAngleValue + 125.f);
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 127.f, mAngleValue + 173.f);
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.rim.relRadius, mAngleValue + 175.f, mAngleValue + 185.f);
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 187.f, mAngleValue + 233.f);
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.rim.relRadius, mAngleValue + 235.f, mAngleValue + 245.f);
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 247.f, mAngleValue + 293.f);
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.rim.relRadius, mAngleValue + 295.f, mAngleValue + 305.f);
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.outerRim.relRadius, mAngleValue + 307.f, mAngleValue + 353.f);
      g.PathArc(mCenterX, mCenterY, mRadius * knobScales.rim.relRadius, mAngleValue + 355.f, mAngleValue + 5.f);
      g.PathClose();
      //g.PathStroke(mPatternEdge, knobScales.outerRim.relThickness * mRadius, strokeOptions);
      g.PathFill(mPatternRim);
      mLayer = g.EndLayer();
      if (!mEmboss && !mGrayed) g.ApplyLayerDropShadow(mLayer, mShadowFrame);
      g.DrawLayer(mLayer);

      // Draw Head
      g.StartLayer(mRECT);
      g.PathClear();
      g.PathCircle(mCenterX, mCenterY, mRadius * knobScales.head.relRadius);
      if (!mGrayed) g.PathFill(mPatternHead, fillOptions);
      else g.PathFill(COLOR_GRAY, fillOptions);
      mLayer = g.EndLayer();
      if (!mEmboss && !mGrayed) g.ApplyLayerDropShadow(mLayer, mShadowHead);
      g.DrawLayer(mLayer);

      if (!mEmboss && !mGrayed) {
        g.PathClear();
        g.PathCircle(mCenterX, mCenterY, mRadius * knobScales.head.relRadius);
        g.PathFill(mPatternHeadLights, fillOptions);
      }


      // Head Lights
      if (!mEmboss && !mGrayed) {
        g.PathClear();
        g.PathCircle(mCenterX, mCenterY, mRadius * knobScales.head.relRadius - 0.5f * knobScales.head.relThickness * mRadius);
        g.PathStroke(mPatternEdge, mRadius * knobScales.head.relThickness, strokeOptions);
      }

      // Outer Arrow
      g.DrawRadialLine(GetColor(kFR), mCenterX, mCenterY, mAngleValue, knobScales.outerArrow.relInnerRadius * mRadius, mRadius * knobScales.outerArrow.relRadius, 0, knobScales.outerArrow.relThickness * mRelThickness);

      // Arrow
      g.StartLayer(mRECT);
      if (mColor.B + mColor.R + mColor.G > 600) {
        g.DrawRadialLine(GetColor(kFR), mCenterX, mCenterY, mAngleValue, knobScales.arrow.relInnerRadius, knobScales.arrow.relRadius * mRadius, 0, knobScales.arrow.relThickness * mRelThickness);
        g.FillCircle(GetColor(kFR), mCenterX, mCenterY, knobScales.arrow.relThickness * mRelThickness * 0.5f, 0);
      }
      else {
        g.DrawRadialLine(GetColor(kON), mCenterX, mCenterY, mAngleValue, knobScales.arrow.relInnerRadius, knobScales.arrow.relRadius * mRadius, 0, knobScales.arrow.relThickness * mRelThickness);
        g.FillCircle(GetColor(kON), mCenterX, mCenterY, knobScales.arrow.relThickness * mRelThickness * 0.5f, 0);
      }
      mLayer = g.EndLayer();
      if (!mEmboss && !mGrayed) g.ApplyLayerDropShadow(mLayer, mShadowArrow);
      g.DrawLayer(mLayer);


      // Mouseover
      if (mMouseIsOver && !mGrayed) g.FillCircle(GetColor(kHL), mCenterX, mCenterY, mRadius * knobScales.head.relRadius);

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

      mTargetRECT = mHandleBounds = mRECT;
      mTextCircleLabelMin.mSize = mTextCircleLabelMax.mSize = mTextCircleLabelCtr.mSize = int(mRECT.H() * 0.18f);
      mLabelText.mSize = mValueText.mSize = int(mRECT.H() * 0.22f);


      // LABEL
      if (mLabel.GetLength())
      {
        IRECT textRect;
        GetUI()->MeasureText(mLabelText, mLabel.Get(), textRect);
        const float labelDisplayWidth = mTargetRECT.W() * mKnobFrac * 0.5f;
        switch (mLabelText.mVAlign)
        {
        case IText::kVAlignTop:
          mLabelBounds = mTargetRECT.GetFromTop(textRect.H()).GetMidHPadded(labelDisplayWidth);
          mHandleBounds = mHandleBounds.GetReducedFromTop(textRect.H());
          break;
        case IText::kVAlignMiddle:
          mLabelBounds = mTargetRECT.GetMidVPadded(textRect.H() / 2.f).GetMidHPadded(labelDisplayWidth);
          break;
        case IText::kVAlignBottom:
          mLabelBounds = mTargetRECT.GetFromBottom(textRect.H()).GetMidHPadded(labelDisplayWidth);
          mHandleBounds = mHandleBounds.GetReducedFromBottom(textRect.H());
          break;
        default:
          break;
        }
        if (mLabelBounds.W() < textRect.W()) mLabelBounds = mLabelBounds.GetMidHPadded(mTargetRECT.W() / 2.f);
      }
      else {
        mLabelBounds = IRECT();
      }

      // DISPLAY VALUE
      if (mDisplayParamValue)
      {
        IRECT textRect;
        WDL_String str;
        GetParam()->GetDisplayForHost(str);

        GetUI()->MeasureText(mValueText, str.Get(), textRect);

        const float valueDisplayWidth = mTargetRECT.W() * mKnobFrac * 0.5f;

        switch (mValueText.mVAlign) {
        case IText::kVAlignTop:
          mValueBounds = mTargetRECT.GetFromTop(textRect.H()).GetMidHPadded(valueDisplayWidth);
          if (mLabelText.mVAlign != IText::kVAlignTop)
            mHandleBounds = mHandleBounds.GetReducedFromTop(textRect.H());
          break;
        case IText::kVAlignMiddle:
          mValueBounds = mTargetRECT.GetMidVPadded(textRect.H() / 2.f).GetMidHPadded(valueDisplayWidth);
          break;
        case IText::kVAlignBottom:
          mValueBounds = mTargetRECT.GetFromBottom(textRect.H()).GetMidHPadded(valueDisplayWidth);
          if (mLabelText.mVAlign != IText::kVAlignBottom)
            mHandleBounds = mHandleBounds.GetReducedFromBottom(textRect.H());
          break;
        default:
          break;
        }

        if (mValueBounds.W() < textRect.W()) mValueBounds = mValueBounds.GetMidHPadded(mTargetRECT.W() / 2.f);
      }
      else {
        mValueBounds = IRECT();
      }

      // CIRCLE LABELS
      if (mDrawCircleLabels) {
        mHandleBounds = mHandleBounds.GetReducedFromTop((float)mTextCircleLabelCtr.mSize);
      }

      mHandleBounds = mHandleBounds.GetMidHPadded(mHandleBounds.H() * 0.5f);
      mTargetRECT = mTargetRECT.GetMidHPadded(mHandleBounds.W() * 0.5f);

      if (mDrawCircleLabels) {
        if (mLabelMin.GetLength()) {
          IRECT textRect;
          GetUI()->MeasureText(mTextCircleLabelMin, mLabelMin.Get(), textRect);
          mMinLabelBounds = IRECT(mRECT.L, mHandleBounds.B - textRect.H(), mHandleBounds.L, mHandleBounds.B);
        }
        if (mLabelMax.GetLength()) {
          IRECT textRect;
          GetUI()->MeasureText(mTextCircleLabelMax, mLabelMax.Get(), textRect);
          mMaxLabelBounds = IRECT(mHandleBounds.R, mHandleBounds.B - textRect.H(), mRECT.R, mHandleBounds.B);
        }
        // Center Label need rework
        if (mLabelCtr.GetLength()) {
          IRECT textRect;
          GetUI()->MeasureText(mTextCircleLabelCtr, mLabelCtr.Get(), textRect);
          mCtrLabelBounds = mRECT.GetFromTop(textRect.H()).GetMidHPadded(textRect.W() * 0.5f);
        }

      }

      //mHandleBounds = GetAdjustedHandleBounds(mTargetRECT).GetReducedFromTop(mCtrLabelBounds.H()).GetReducedFromBottom(;


      // ----------------------------------------------------------------------------










      mRadius = (mHandleBounds.W() * 0.5f);
      mCenterX = mHandleBounds.MW();
      mCenterY = mHandleBounds.MH();
      mRelThickness = mRadius * mFrameThickness * 0.05f / 2.f;
      mRelShadowOffset = mRadius * mShadowOffset * 0.2f / 3.f;

      // PATTERN TEST
      // Patterns
      mPatternShadow = IPattern(COLOR_BLACK);
      mPatternHead = IPattern::CreateRadialGradient(
        mCenterX,
        mCenterY,
        mRadius,
        {
          IColorStop(mColor, 0.9f),
          //IColorStop(mColor.GetContrasted(1.0f), 0.9f),
          //IColorStop(mColor, 0.95f),
          IColorStop(ColorGetAltered(mColor, 0.5f), 1.f)
        }
      );
      mPatternHeadLights = IPattern::CreateRadialGradient(
        mCenterX - GetPercW() * 0.5f * mRadius,
        mCenterY - GetPercH() * 0.5f * mRadius,
        mRadius * knobScales.head.relRadius,
        {
          IColorStop(IColor(200, 255,255,255), 0.0f),
          IColorStop(COLOR_TRANSPARENT, 1.0f)
        }
      );
      mPatternRim = IPattern::CreateLinearGradient(
        mCenterX - mRadius, mCenterY - mRadius,
        mCenterX + mRadius, mCenterY + mRadius,
        {
          IColorStop(GetColor(kON), 0.5f),
          IColorStop(GetColor(kFR), 1.0f)
        }
      );
      mPatternEdge = IPattern::CreateLinearGradient(
        mCenterX - 0.5f * mRadius, mCenterY - 0.5f * mRadius,
        mCenterX + 0.5f * mRadius, mCenterY + 0.5f * mRadius,
        {
          IColorStop(IColor(130, 255, 255, 255), 0.0f),
          IColorStop(IColor(20, 130, 130, 130), 0.5f),
          IColorStop(IColor(130, 0, 0, 0), 1.0f)
        }
      );

      // Shadows
      mShadowFrame = IShadow(
        mPatternShadow,
        mRadius * 0.1f,
        0.6f * mRadius * GetPercW(),
        0.6f * mRadius * GetPercH(),
        0.5f,
        true
      );

      mShadowHead = IShadow(
        mPatternShadow,
        mRadius * 0.01f,
        0.3f * mRadius * GetPercW(),
        0.3f * mRadius * GetPercH(),
        0.5f,
        true
      );

      mShadowArrow = IShadow(
        mPatternShadow,
        mRadius * 0.01f,
        0.1f * mRadius * GetPercW(),
        0.1f * mRadius * GetPercH(),
        0.5f,
        true
      );
      // ---}

      SetDirty(false);
    }

    //void SRVectorKnobText::GrayOut(bool gray)
    //{
    //  mGrayed = gray;
    //  if (mGrayed) mColor = COLOR_GRAY;
    //  SetDirty(false);
    //}



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

    IRECT SRVectorSwitch::DrawVectorButton(IGraphics& g, const IRECT& bounds, bool pressed, bool mouseOver)
    {
      // background
      g.FillRect(GetColor(kBG), bounds);

      IRECT handleBounds = GetAdjustedHandleBounds(bounds);
      const float cornerRadius = mRoundness * (handleBounds.W() / 2.f);

      // Pressed
      if (pressed) {
        g.FillRoundRect(GetColor(kPR), handleBounds, cornerRadius);

        //inner shadow
        if (mDrawShadows && mEmboss) {
          g.PathRect(handleBounds.GetHSliced(mShadowOffset));
          g.PathRect(handleBounds.GetVSliced(mShadowOffset));
          g.PathFill(GetColor(kSH));
        }

        if (mouseOver) g.FillRoundRect(GetColor(kHL), handleBounds, cornerRadius);
        if (mControl->GetAnimationFunction()) DrawFlashCircle(g);
        if (mDrawFrame) g.DrawRoundRect(GetColor(kFR), handleBounds, cornerRadius, 0, mFrameThickness);
      }
      else {
        // Normal button state
        if (mNumStates > 2 || mValue == 0) {
          //outer shadow
          if (mDrawShadows && !mEmboss) g.FillRoundRect(GetColor(kSH), handleBounds, cornerRadius);
          g.FillRoundRect(GetColor(kFG), handleBounds, cornerRadius);
          if (mouseOver) g.FillRoundRect(GetColor(kHL), handleBounds, cornerRadius);
          if (mControl->GetAnimationFunction()) DrawFlashCircle(g);
          if (mDrawFrame) g.DrawRoundRect(GetColor(kFR), handleBounds, cornerRadius, 0, mFrameThickness);
        }

        // If button should be "inside"
        else {
          g.FillRoundRect(GetColor(kPR), handleBounds.GetTranslated(mShadowOffset, mShadowOffset), cornerRadius);
          //inner shadow
          if (mDrawShadows && mEmboss) {
            g.PathRect(handleBounds.GetHSliced(mShadowOffset));
            g.PathRect(handleBounds.GetVSliced(mShadowOffset));
            g.PathFill(GetColor(kSH));
          }
          if (mouseOver) g.FillRoundRect(GetColor(kHL), handleBounds.GetTranslated(mShadowOffset, mShadowOffset), cornerRadius);
          if (mControl->GetAnimationFunction()) DrawFlashCircle(g);
          if (mDrawFrame) g.DrawRoundRect(GetColor(kFR), handleBounds.GetTranslated(mShadowOffset, mShadowOffset), cornerRadius, 0, mFrameThickness);
        }
      }

      return handleBounds;
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





    void SRPresetMenu::Draw(IGraphics& g) {
      int pNumber = mPlug->GetCurrentPresetIdx();

      mDisp.SetFormatted(32, "%02d: %s", pNumber + 1, mPlug->GetPresetName(pNumber));

      IColor colorBg = IColor(50, 0, 0, 0);
      IColor colorFrame = IColor(50, 255, 255, 255);
      g.FillRoundRect(colorBg, mRECT, 5.f);
      g.DrawRoundRect(colorFrame, mRECT, 5.f);

      if (mDisp.Get())
      {
        g.DrawText(mText, mDisp.Get(), mRECT);
      }
    }

    void SRPresetMenu::OnMouseDown(float x, float y, const IMouseMod& mod) {
      if (mod.R) {
        const char* pname = mPlug->GetPresetName(mPlug->GetCurrentPresetIdx());
        GetUI()->CreateTextEntry(*this, mText, mRECT, pname);
      }
      else {
        doPopupMenu(*GetUI());
      }
      //Redraw(); // seems to need this
      SetDirty();
    }

    void SRPresetMenu::doPopupMenu(IGraphics& g) {
      const int numPresets = mPlug->NPresets();
      IPopupMenu menuMain;
      int currentPresetIdx = mPlug->GetCurrentPresetIdx();

      for (int i = 0; i < numPresets; i++) {
        const char* str = mPlug->GetPresetName(i);
        if (i == currentPresetIdx)
          menuMain.AddItem(str, i, IPopupMenu::Item::Flags::kChecked);
        else
          menuMain.AddItem(str, i, IPopupMenu::Item::Flags::kNoFlags);
      }
      menuMain.AddItem("Export Preset", numPresets);

      menuMain.SetPrefix(0);

      if (g.CreatePopupMenu(menuMain, mRECT)) {
        int itemChosen = menuMain.GetChosenItemIdx();

        if (itemChosen > -1 && itemChosen < numPresets) {
          mPlug->RestorePreset(itemChosen);
          mPlug->InformHostOfProgramChange();
          mPlug->DirtyParametersFromUI();
        }
        else {
          //const int numParams = mPlug->NParams();
          //const char** enumNames = new const char*[numParams];
          //for (int i = 0; i < mPlug->NParams(); i++) {
          //  enumNames[i] = mPlug->GetParamGroupName(i);
          //}
          WDL_String filename, path;
          GetUI()->PromptForFile(filename, path, EFileAction::kFileSave, "txt");
          mPlug->DumpPresetSrcCode(filename.Get(), mNamedParams);
        }
      }
    }
    void SRPresetMenu::TextFromTextEntry(const char * txt) {
      WDL_String safeName;
      safeName.Set(txt, MAX_PRESET_NAME_LEN);

      mPlug->ModifyCurrentPreset(safeName.Get());
      mPlug->InformHostOfProgramChange();
      mPlug->DirtyParametersFromUI();
      SetDirty(false);
    }


  } // End namespace SRControls
} // End namespace SR