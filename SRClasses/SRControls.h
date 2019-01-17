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
      SRVectorKnobText(IRECT bounds, int paramIdx,
        const char* label = "", const char* minLabel = "", const char* maxLabel = "", const char* ctrLabel = "", bool drawCircleLabels = false, bool displayParamValue = false,
        const IVColorSpec& colorSpec = DEFAULT_SPEC, const IColor& color = DEFAULT_FGCOLOR,
        const IText& labelText = IText(DEFAULT_TEXT_SIZE + 5, IText::kVAlignTop), const IText& valueText = IText(DEFAULT_TEXT_SIZE, IText::kVAlignBottom),
        float aMin = -135.f, float aMax = 135.f, float aDef = 0.f, float knobFrac = 0.50f,
        EDirection direction = kVertical, double gearing = DEFAULT_GEARING);

      SRVectorKnobText(IRECT bounds, IActionFunction actionFunction,
        const char* label = "", const char* minLabel = "", const char* maxLabel = "", const char* ctrLabel = "", bool drawCircleLabels = false, bool displayParamValue = false,
        const IVColorSpec& colorSpec = DEFAULT_SPEC, const IColor& color = DEFAULT_FGCOLOR,
        const IText& labelText = IText(DEFAULT_TEXT_SIZE + 5, IText::kVAlignTop), const IText& valueText = IText(DEFAULT_TEXT_SIZE, IText::kVAlignBottom),
        float aMin = -135.f, float aMax = 135.f, float aDef = 0.f, float knobFrac = 0.50f,
        EDirection direction = kVertical, double gearing = DEFAULT_GEARING);

      virtual ~SRVectorKnobText() {}

      void Draw(IGraphics& g) override;
      void OnMouseDown(float x, float y, const IMouseMod& mod) override;
      void OnInit() override;
      //  void OnMouseDblClick(float x, float y, const IMouseMod& mod) override {  OnMouseDown(x, y, mod); }
      void OnResize() override;



    protected:
      bool mDisplayParamValue;
      bool mShowParamLabel = true;
      IRECT mHandleBounds;
      IRECT mLabelBounds;
      IRECT mValueBounds;
      IRECT mMinLabelBounds;
      IRECT mMaxLabelBounds;
      IRECT mCtrLabelBounds;
      float mAngleMin, mAngleMax, mAngleDefault;
      float mKnobFrac;
      float mRadius, mAngleValue, mCenterX, mCenterY, mRelThickness, mRelShadowOffset;
      WDL_String mLabel;
      WDL_String mLabelMin;
      WDL_String mLabelMax;
      WDL_String mLabelCtr;
      bool mDrawCircleLabels;
      IText mLabelText;
      IText& mValueText = mText;
      IText mTextCircleLabelMin = IText(10, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignFar);
      IText mTextCircleLabelMax = IText(10, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignNear);
      IText mTextCircleLabelCtr = IText(10, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignCenter);
      IColor mColor;
      struct KnobScaleVals
      {
        const float relRadius = 1.f;
        const float relThickness = 2.f;
        const float relInnerRadius = 0.f;
      };
      struct KnobScales
      {
        const KnobScaleVals valArc = { 1.f, 4.f };
        const KnobScaleVals dots = { 1.f, 2.f };
        const KnobScaleVals rim = { 0.825f, -1.f };
        const KnobScaleVals outerRim = { 0.9f, 3.f };
        const KnobScaleVals head = { 0.8f, -1.f };
        const KnobScaleVals headLights = { 0.775f, 0.05f };
        const KnobScaleVals outerArrow = { 0.9f, 3.f, 0.7f };
        const KnobScaleVals arrow = { 0.8f, 3.f, 0.f };

      } knobScales;
    };


    /** A vector switch control. Click to cycle through states. */
    class SRVectorSwitch : public ISwitchControlBase
      , public IVectorBase
    {
    public:
      SRVectorSwitch(IRECT bounds, int paramIdx = kNoParameter, IActionFunction actionFunc = FlashCircleClickActionFunc,
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
      SRLogo(IRECT bounds, const char* str = "", const IText& text = DEFAULT_TEXT, const IColor& BGColor = DEFAULT_BGCOLOR)
        : IControl(bounds)
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
      TestGradientControl(IRECT rect, int paramIdx = kNoParameter)
        : IKnobControlBase(rect, paramIdx)
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