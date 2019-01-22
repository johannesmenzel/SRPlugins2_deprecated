#pragma once
#include "IPlug_include_in_plug_hdr.h"
#include "IControl.h"
#include "SRHelpers.h"
// For meter, see IVMeterControl.h
#include "IPlugQueue.h"
#include "IPlugStructs.h"
#include "IVMeterControl.h"
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
      void OnResize() override;
      float GetPercW() { return mCenterX / GetDelegate()->GetEditorWidth(); }
      float GetPercH() { return mCenterY / GetDelegate()->GetEditorHeight(); }
      IColor ColorGetAltered(IColor pColor, float change) {
        IColor color = pColor;
        color.R = int(color.R * change); color.R = (color.R > 255) ? 255 : color.R;
        color.B = int(color.B * change); color.B = (color.B > 255) ? 255 : color.B;
        color.G = int(color.G * change); color.G = (color.G > 255) ? 255 : color.G;
        return color;
      }



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
      IText mTextCircleLabelMin;
      IText mTextCircleLabelMax;
      IText mTextCircleLabelCtr;
      IColor mColor;
      IPattern mPatternShadow;
      IPattern mPatternHead;
      IPattern mPatternHeadLights;
      IPattern mPatternRim;
      IPattern mPatternEdge;
      ILayerPtr mLayer;
      IShadow mShadowFrame;
      IShadow mShadowHead;
      IShadow mShadowArrow;
      const struct KnobScaleVals
      {
        const float relRadius = 1.f;
        const float relThickness = 2.f;
        const float relInnerRadius = 0.f;
      };
      const struct KnobScales
      {
        const KnobScaleVals valArc = { 1.f, 4.f };
        const KnobScaleVals dots = { 1.f, 2.f };
        const KnobScaleVals rim = { 0.825f, -1.f };
        const KnobScaleVals outerRim = { 0.9f, 0.06f };
        const KnobScaleVals head = { 0.75f, 0.06f };
        const KnobScaleVals outerArrow = { 0.825f, 3.f, 0.7f };
        const KnobScaleVals arrow = { 0.75f, 3.f, 0.f };

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


    ///** Control to test drawing gradients with path based drawing backends
    // *   @ingroup TestControls */
    //class TestGradientControl : public IKnobControlBase
    //{
    //public:
    //  TestGradientControl(IRECT rect, int paramIdx = kNoParameter)
    //    : IKnobControlBase(rect, paramIdx)
    //  {
    //    SetTooltip("TestGradientControl");
    //    RandomiseGradient();
    //  }

    //  void Draw(IGraphics& g) override
    //  {
    //    if (g.HasPathSupport())
    //    {
    //      double cr = mValue * (mRECT.H() / 2.0);
    //      g.PathRoundRect(mRECT.GetPadded(-2), cr);
    //      IFillOptions fillOptions;
    //      IStrokeOptions strokeOptions;
    //      fillOptions.mPreserve = true;
    //      g.PathFill(mPattern, fillOptions);
    //      g.PathStroke(IColor(255, 0, 0, 0), 3, strokeOptions);
    //    }
    //    else
    //      g.DrawText(mText, "UNSUPPORTED", mRECT);
    //  }

    //  void OnMouseDown(float x, float y, const IMouseMod& mod) override
    //  {
    //    RandomiseGradient();
    //    SetDirty(false);
    //  }

    //  void RandomiseGradient()
    //  {
    //    //IPattern tmp(kLinearPattern);
    //    //tmp.SetTransform(1.0/mRECT.W(), 0, 0, 1.0/mRECT.W(), 1.0/mRECT.W()*-mRECT.L, 1.0/mRECT.W()*-mRECT.T);
    //    IPattern tmp(kSolidPattern);

    //    if (std::rand() & 0x100)
    //      tmp = IPattern::CreateRadialGradient(mRECT.MW(), mRECT.MH(), mRECT.MH());
    //    else
    //      tmp = IPattern::CreateLinearGradient(mRECT.L, mRECT.MH(), mRECT.L + mRECT.W() * 0.5, mRECT.MH());

    //    tmp.mExtend = (std::rand() & 0x10) ? ((std::rand() & 0x1000) ? kExtendNone : kExtendPad) : ((std::rand() & 0x1000) ? kExtendRepeat : kExtendReflect);

    //    tmp.AddStop(IColor::GetRandomColor(), 0.0);
    //    tmp.AddStop(IColor::GetRandomColor(), 0.1);
    //    tmp.AddStop(IColor::GetRandomColor(), 0.4);
    //    tmp.AddStop(IColor::GetRandomColor(), 0.6);
    //    tmp.AddStop(IColor::GetRandomColor(), 1.0);

    //    mPattern = tmp;
    //  }

    //private:
    //  IPattern mPattern = IPattern(kLinearPattern);
    //};



    ///** A base class for mult-strip/track controls, such as multi-sliders, meters */
    //class SRTrackControl : public IControl
    //  , public IVectorBase
    //{
    //public:
    //  SRTrackControl(IRECT bounds, int maxNTracks = 1, float minTrackValue = 0.f, float maxTrackValue = 1.f, const char* trackNames = 0, ...)
    //    : IControl(bounds)
    //    , mMaxNTracks(maxNTracks)
    //    , mMinTrackValue(minTrackValue)
    //    , mMaxTrackValue(maxTrackValue)
    //  {
    //    for (int i = 0; i < maxNTracks; i++)
    //    {
    //      mTrackData.Add(0.f);
    //      mTrackBounds.Add(IRECT());
    //    }

    //    AttachIControl(this);
    //  }

    //  void MakeRects()
    //  {
    //    for (int ch = 0; ch < MaxNTracks(); ch++)
    //    {
    //      mTrackBounds.Get()[ch] = mRECT.GetPadded(-mOuterPadding).
    //        SubRect(EDirection(!mDirection), MaxNTracks(), ch).
    //        GetPadded(0, -mTrackPadding * (float)mDirection, -mTrackPadding * (float)!mDirection, -mTrackPadding);
    //    }
    //  }

    //  void Draw(IGraphics& g) override
    //  {
    //    g.FillRect(GetColor(kBG), mRECT);

    //    for (int ch = 0; ch < MaxNTracks(); ch++)
    //    {
    //      DrawTrack(g, mTrackBounds.Get()[ch], ch);
    //    }

    //    if (mDrawFrame)
    //      DrawFrame(g);
    //  }

    //  int NTracks() { return mNTracks; }
    //  int MaxNTracks() { return mMaxNTracks; }
    //  void SetTrackData(int trackIdx, float val) { mTrackData.Get()[trackIdx] = Clip(val, mMinTrackValue, mMaxTrackValue); }
    //  float* GetTrackData(int trackIdx) { return &mTrackData.Get()[trackIdx]; }
    //  void SetAllTrackData(float val) { memset(mTrackData.Get(), (int)Clip(val, mMinTrackValue, mMaxTrackValue), mTrackData.GetSize() * sizeof(float)); }
    //private:
    //  virtual void DrawFrame(IGraphics& g)
    //  {
    //    g.DrawRect(GetColor(kFR), mRECT, nullptr, mFrameThickness);
    //  }

    //  virtual void DrawTrack(IGraphics& g, IRECT& r, int chIdx)
    //  {
    //    DrawTrackBG(g, r, chIdx);
    //    DrawTrackHandle(g, r, chIdx);

    //    if (mDrawTrackFrame)
    //      g.DrawRect(GetColor(kFR), r, nullptr, mFrameThickness);
    //  }

    //  virtual void DrawTrackBG(IGraphics& g, IRECT& r, int chIdx)
    //  {
    //    g.FillRect(GetColor(kSH), r);
    //  }

    //  virtual void DrawTrackHandle(IGraphics& g, IRECT& r, int chIdx)
    //  {
    //    IRECT fillRect = r.FracRect(mDirection, *GetTrackData(chIdx));

    //    g.FillRect(GetColor(kFG), fillRect); // TODO: shadows!

    //    IRECT peakRect;

    //    if (mDirection == kVertical)
    //      peakRect = IRECT(fillRect.L, fillRect.T, fillRect.R, fillRect.T + mPeakSize);
    //    else
    //      peakRect = IRECT(fillRect.R - mPeakSize, fillRect.T, fillRect.R, fillRect.B);

    //    DrawPeak(g, peakRect, chIdx);
    //  }

    //  virtual void DrawPeak(IGraphics& g, IRECT& r, int chIdx)
    //  {
    //    g.FillRect(GetColor(kHL), r);
    //  }

    //  void OnResize() override
    //  {
    //    MakeRects();
    //  }

    //protected:

    //  EDirection mDirection = EDirection::kVertical;
    //  int mMaxNTracks;
    //  WDL_TypedBuf<float> mTrackData; // real values of sliders/meters
    //  WDL_TypedBuf<IRECT> mTrackBounds;

    //  int mNTracks = 1;

    //  float mMinTrackValue;
    //  float mMaxTrackValue;
    //  float mOuterPadding = 10.;
    //  float mTrackPadding = 2;
    //  float mPeakSize = 5.;
    //  bool mDrawTrackFrame = true;
    //};

    template <int MAXNC = 1, int QUEUE_SIZE = 1024>
    class SRMeter : public IVTrackControlBase
    {
    public:
      static constexpr int kUpdateMessage = 0;

      /** Data packet */
      struct Data
      {
        int nchans = MAXNC;
        float vals[MAXNC] = {};

        bool AboveThreshold()
        {
          static const float threshold = (float)DBToAmp(-90.);

          float sum = 0.f;

          for (int i = 0; i < MAXNC; i++)
          {
            sum += vals[i];
          }

          return std::abs(sum) > threshold;
        }
      };

      /** Used on the DSP side in order to queue sample values and transfer data to low priority thread. */
      class SRMeterBallistics
      {
      public:
        SRMeterBallistics(int controlTag)
          : mControlTag(controlTag)
        {
        }

        void ProcessBlock(sample** inputs, int nFrames)
        {
          Data d;

          for (auto s = 0; s < nFrames; s++)
          {
            for (auto c = 0; c < MAXNC; c++)
            {
              d.vals[c] += std::fabs((float)inputs[c][s]);
            }
          }

          for (auto c = 0; c < MAXNC; c++)
          {
            d.vals[c] /= (float)nFrames;
          }

          if (mPrevAboveThreshold)
            mQueue.Push(d); // TODO: expensive?

          mPrevAboveThreshold = d.AboveThreshold();
        }

        // this must be called on the main thread - typically in MyPlugin::OnIdle()
        void TransmitData(IEditorDelegate& dlg)
        {
          while (mQueue.ElementsAvailable())
          {
            Data d;
            mQueue.Pop(d);
            dlg.SendControlMsgFromDelegate(mControlTag, kUpdateMessage, sizeof(Data), (void*)&d);
          }
        }

      private:
        int mControlTag;
        bool mPrevAboveThreshold = true;
        IPlugQueue<Data> mQueue{ QUEUE_SIZE };
      };

      SRMeter(IRECT bounds, bool drawFromTop = false, bool drawInverted = false, bool drawDb = true, double minDb = 90., double maxDb = 0., double shape = 1.0, int numLabelSteps = 9, const char* trackNames = 0, ...)
        : IVTrackControlBase(bounds, MAXNC, 0, 1., trackNames)
        , mDrawFromTop(drawFromTop)
        , mDrawInverted(drawInverted)
        , mDrawDb(drawDb)
        , mMinDb(minDb)
        , mMaxDb(maxDb)
        , mShape(shape)
        , mNumLabelSteps(numLabelSteps)
        , mText(IText(10, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignCenter, IText::kVAlignMiddle, 0, IText::kQualityClearType))

      {
      }

      //  void OnResize() override;
      //  void OnMouseDblClick(float x, float y, const IMouseMod& mod) override;
      //  void OnMouseDown(float x, float y, const IMouseMod& mod) override;

      void Draw(IGraphics& g) override
      {
        g.FillRect(GetColor(kBG), mRECT);

        for (int ch = 0; ch < MaxNTracks(); ch++)
        {
          DrawTrack(g, mTrackBounds.Get()[ch], ch);
        }

        if (mDrawFrame)
          DrawFrame(g);

        const IRECT rectLabelFrame = mRECT.GetPadded(-mOuterPadding);
        for (int i = 0; i <= mNumLabelSteps; i++) {
          const float val = mMinDb + ((float)i / (float)mNumLabelSteps) * (mMaxDb - mMinDb);
          const float vPosition = std::pow((val - mMinDb) / (mMaxDb - mMinDb), 1.0 / mShape);
          const IRECT labelRECT = IRECT(rectLabelFrame.L, -mText.mSize + rectLabelFrame.B - vPosition * rectLabelFrame.H(), rectLabelFrame.R, mText.mSize + rectLabelFrame.B - vPosition * rectLabelFrame.H());
          const int valInt = int(std::round(val));
          std::string measureStr = std::to_string(valInt);
          const char *measureChar = measureStr.c_str();
          WDL_String str;
          str.Set(measureChar);
          g.DrawText(mText, str.Get(), labelRECT);
        }
      }

      void MakeRects()
      {
        for (int ch = 0; ch < MaxNTracks(); ch++)
        {
          mTrackBounds.Get()[ch] = mRECT.GetPadded(-mOuterPadding).
            SubRect(EDirection(!mDirection), MaxNTracks(), ch).
            GetPadded(0, -mTrackPadding, -mTrackPadding * (float)!mDirection, -mTrackPadding * (float)mDirection);
        }
      }

      virtual void DrawFrame(IGraphics& g)
      {
        g.DrawRect(GetColor(kFR), mRECT, nullptr, mFrameThickness);
      }

      virtual void DrawTrack(IGraphics& g, IRECT& r, int chIdx)
      {
        DrawTrackBG(g, r, chIdx);
        DrawTrackHandle(g, r, chIdx);

        if (mDrawTrackFrame)
          g.DrawRect(GetColor(kFR), r, nullptr, mFrameThickness);
      }

      virtual void DrawTrackBG(IGraphics& g, IRECT& r, int chIdx)
      {
        g.FillRect(GetColor(kSH), r);
      }

      virtual void DrawTrackHandle(IGraphics& g, IRECT& r, int chIdx)
      {
        IRECT fillRect;
        double value = *GetTrackData(chIdx);
        if (mDrawDb) value = std::pow((AmpToDB(value) - mMinDb) / (mMaxDb - mMinDb), 1.0 / mShape);
        if (!mDrawInverted) {
          fillRect = r.FracRect(mDirection, value, mDrawFromTop); // HERE the value rect is drawn!
        }
        else {
          fillRect = r.FracRect(mDirection, (1. - value), mDrawFromTop); // HERE the value rect is drawn!
        }

        g.FillRect(GetColor(kFG), fillRect); // TODO: shadows!

        IRECT peakRect;

        if (mDirection == kVertical)
          peakRect = IRECT(fillRect.L, fillRect.T, fillRect.R, fillRect.T + mPeakSize);
        else
          peakRect = IRECT(fillRect.R - mPeakSize, fillRect.T, fillRect.R, fillRect.B);

        DrawPeak(g, peakRect, chIdx);


      }

      virtual void DrawPeak(IGraphics& g, IRECT& r, int chIdx)
      {
        g.FillRect(GetColor(kHL), r);
      }

      void OnMsgFromDelegate(int messageTag, int dataSize, const void* pData) override
      {
        IByteStream stream(pData, dataSize);

        int pos = 0;
        Data data;
        pos = stream.Get(&data.nchans, pos);

        while (pos < stream.Size())
        {
          for (auto i = 0; i < data.nchans; i++) {
            pos = stream.Get(&data.vals[i], pos);
            float* pVal = GetTrackData(i);
            *pVal = Clip(data.vals[i], 0.f, 1.f);
          }
        }

        SetDirty(false);
      }
    protected:
      bool mDrawFromTop;
      bool mDrawInverted;
      bool mDrawDb;
      double mMinDb;
      double mMaxDb;
      int mNumLabelSteps;
      double mShape;
      IText mText;
    };



  }
} // End namespaces