#pragma once
#include "IPlug_include_in_plug_hdr.h"
#include "IControl.h"
//#include "IPlugPluginBase.h"
#include "../Utils/SRHelpers.h"
// For meter, see IVMeterControl.h
#include "IPlugQueue.h"
#include "IPlugStructs.h"
#include "IVMeterControl.h"
//#include <string>

namespace SR {
  namespace Graphics {


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
      //void OnInit() override;
      void OnResize() override;
      //void GrayOut(bool gray) override;
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
      bool mDrawCircleLabels;
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

      IRECT DrawVectorButton(IGraphics&g, const IRECT& bounds, bool pressed, bool mouseOver);


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



    /** A base class for mult-strip/track controls, such as multi-sliders, meters */
    class SRTrackControlBase : public IControl
      , public IVectorBase
    {
    public:
      SRTrackControlBase(IRECT bounds, int maxNTracks = 1, float minTrackValue = 0.f, float maxTrackValue = 4.f, const char* trackNames = 0, ...)
        : IControl(bounds)
        , mMaxNTracks(maxNTracks)
        , mMinTrackValue(minTrackValue)
        , mMaxTrackValue(maxTrackValue)
      {
        for (int i = 0; i < maxNTracks; i++)
        {
          mTrackData.Add(0.f);
          mTrackBounds.Add(IRECT());
        }

        AttachIControl(this);
      }

      void MakeRects()
      {
        for (int ch = 0; ch < MaxNTracks(); ch++)
        {
          mTrackBounds.Get()[ch] = mRECT.GetPadded(-mOuterPadding).
            SubRect(EDirection(!mDirection), MaxNTracks(), ch).
            GetPadded(0, -mTrackPadding * (float)mDirection, -mTrackPadding * (float)!mDirection, -mTrackPadding);
        }
      }

      void Draw(IGraphics& g) override
      {
        g.FillRect(GetColor(kBG), mRECT);

        for (int ch = 0; ch < MaxNTracks(); ch++)
        {
          DrawTrack(g, mTrackBounds.Get()[ch], ch);
        }

        if (mDrawFrame)
          DrawFrame(g);
      }

      int NTracks() { return mNTracks; }
      int MaxNTracks() { return mMaxNTracks; }
      void SetTrackData(int trackIdx, float val) { mTrackData.Get()[trackIdx] = Clip(val, mMinTrackValue, mMaxTrackValue); }
      float* GetTrackData(int trackIdx) { return &mTrackData.Get()[trackIdx]; }
      void SetAllTrackData(float val) { memset(mTrackData.Get(), (int)Clip(val, mMinTrackValue, mMaxTrackValue), mTrackData.GetSize() * sizeof(float)); }
    private:
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
        IRECT fillRect = r.FracRect(mDirection, *GetTrackData(chIdx));

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

      void OnResize() override
      {
        MakeRects();
      }

    protected:

      EDirection mDirection = EDirection::kVertical;
      int mMaxNTracks;
      WDL_TypedBuf<float> mTrackData; // real values of sliders/meters
      WDL_TypedBuf<IRECT> mTrackBounds;

      int mNTracks = 1;

      float mMinTrackValue;
      float mMaxTrackValue;
      float mOuterPadding = 10.;
      float mTrackPadding = 2;
      float mPeakSize = 5.;
      bool mDrawTrackFrame = true;
    };

    // End TrackControl class


    // METER CLASS
    // ------------

    template <int MAXNC = 1, int QUEUE_SIZE = 1024>
    class SRMeter : public SRTrackControlBase {
    public:

      static constexpr int kUpdateMessage = 0;

      /** Data packet */
      struct Data
      {
        int nchans = MAXNC;
        float vals[MAXNC] = {};
        bool AboveThreshold() {
          static const float threshold = (float)DBToAmp(-90.);
          float sum = 0.f;
          for (int i = 0; i < MAXNC; i++) {
            sum += vals[i];
          }
          return std::abs(sum) > threshold;
        }
      };

      /** Used on the DSP side in order to queue sample values and transfer data to low priority thread. */
      class SRMeterBallistics {

      public:
        SRMeterBallistics(int controlTag)
          : mControlTag(controlTag)
        {
        }

        void ProcessBlock(sample** inputs, int nFrames)
        {
          Data d;

          for (auto s = 0; s < nFrames; s++) {
            for (auto c = 0; c < MAXNC; c++) {
              d.vals[c] += std::fabs((float)inputs[c][s]);
            }
          }

          for (auto c = 0; c < MAXNC; c++) {
            d.vals[c] /= (float)nFrames;
          }

          if (mPrevAboveThreshold)
            mQueue.Push(d); // TODO: expensive?

          mPrevAboveThreshold = d.AboveThreshold();
        }

        // this must be called on the main thread - typically in MyPlugin::OnIdle()
        void TransmitData(IEditorDelegate& dlg) {
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

      struct LabelFrames {
        IRECT rectLabel = IRECT();
        WDL_String str = WDL_String();
        void SetLabelFrame(IRECT r, float val) {
          str.SetFormatted(MAX_PARAM_DISPLAY_LEN, "%d", static_cast<int>(roundf(val)));
          rectLabel = r;
        }
      };

      struct LinePos {
        float position = 0.0f;
        float thickness = 1.0f;
        void SetLine(float pos, float thick) {
          position = pos;
          thickness = thick;
        }
      };

      // If you want to create a meter with decibel values
      SRMeter(IRECT bounds, bool drawFromTop = false, bool drawInverted = false, float minDb = 90., float maxDb = 0., float shape = 1.0, int markStep = 1, int labelStep = 6, const char* trackNames = 0, ...)
        : SRTrackControlBase(bounds, MAXNC, 0, 1., trackNames)
        , mDrawFromTop(drawFromTop)
        , mDrawInverted(drawInverted)
        , mMinDb(minDb)
        , mMaxDb(maxDb)
        , mShape(shape)
        , mMaxTrackValue(4.f)
        , mMarkStep(markStep)
        , mLabelStep(labelStep)
        , mText(IText(14, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignCenter, IText::kVAlignMiddle, 0, IText::kQualityClearType))
        , mNumLines(int(maxDb - minDb))
        , mNumLabels(int((maxDb - minDb) / labelStep))
        , rectLabelFrame(IRECT())
        //, mPattern(kLinearPattern)
        , mDrawDb(true)
      {
        mLabelFrames = new LabelFrames[mNumLabels + 1];
        mLineCoord = new LinePos[mNumLines + 1];
      }

      // If you want to create a meter with normalized values 0..1
      SRMeter(IRECT bounds, bool drawFromTop = false, bool drawInverted = false, const char* trackNames = 0, ...)
        : IVTrackControlBase(bounds, MAXNC, 0, 1., trackNames)
        , mDrawFromTop(drawFromTop)
        , mDrawInverted(drawInverted)
        , mText(IText(14, COLOR_LIGHT_GRAY, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignCenter, IText::kVAlignMiddle, 0, IText::kQualityClearType))
        //, mPattern(kLinearPattern)
        , mDrawDb(false)
      {
      }

      void OnResize() override {
        MakeRects();
        mText.mSize = int(mRECT.W() * 0.25f);
      };

      //  void OnMouseDblClick(float x, float y, const IMouseMod& mod) override;
      //  void OnMouseDown(float x, float y, const IMouseMod& mod) override;

      void MakeRects() {
        for (int ch = 0; ch < MaxNTracks(); ch++) {
          mTrackBounds.Get()[ch] = mRECT.GetPadded(-mOuterPadding).
            SubRect(EDirection(!mDirection), MaxNTracks(), ch).
            GetPadded(0, -mTrackPadding, -mTrackPadding * (float)!mDirection, -mTrackPadding * (float)mDirection);
        }

        if (mDrawDb) {

          rectLabelFrame = mRECT.GetPadded(-mOuterPadding);

          for (int i = 0, j = 0; i <= mNumLines; i++) {
            const float val = std::roundf(mMinDb + ((float)i / (float)mNumLines) * (mMaxDb - mMinDb));
            const float vPosition = std::pow<float>((val - mMinDb) / (mMaxDb - mMinDb), 1.0f / mShape);
            mLineCoord[i].SetLine(rectLabelFrame.B - rectLabelFrame.H() * vPosition, 1.f);
            if (i % mLabelStep == 0) {
              mLineCoord[i].thickness = mFrameThickness;
              mLabelFrames[j].SetLabelFrame(
                IRECT(rectLabelFrame.L, -mText.mSize + rectLabelFrame.B - vPosition * rectLabelFrame.H(), rectLabelFrame.R, mText.mSize + rectLabelFrame.B - vPosition * rectLabelFrame.H()),
                val
              );
              j++;
            }
          }
        }

      }

      void Draw(IGraphics& g) override {
        // Don't want to fill with transparent IColor
        // g.FillRect(GetColor(kBG), mRECT);

        for (int ch = 0; ch < MaxNTracks(); ch++) {
          DrawTrack(g, mTrackBounds.Get()[ch], ch);
        }

        // Don't want to draw a Frame
        //if (mDrawFrame)
        //  DrawFrame(g);

        if (mDrawDb) {
          for (int i = 0; i <= mNumLabels; i++) {
            g.DrawText(mText, mLabelFrames[i].str.Get(), mLabelFrames[i].rectLabel);
          }
          for (int i = 0; i <= mNumLines; i++) {
            g.DrawLine(GetColor(kHL), rectLabelFrame.L, mLineCoord[i].position, rectLabelFrame.R, mLineCoord[i].position, 0, mLineCoord[i].thickness);
          }
        }

      }

      //virtual void DrawFrame(IGraphics& g)
      //{
      //  //g.DrawRect(GetColor(kFR), mRECT, nullptr, mFrameThickness);
      //}

      virtual void DrawTrack(IGraphics& g, IRECT& r, int chIdx) {
        DrawTrackBG(g, r, chIdx);
        DrawTrackHandle(g, r, chIdx);
        //if (mDrawTrackFrame)
        //  g.DrawRect(GetColor(kBG), r, nullptr, mFrameThickness);
      }

      virtual void DrawTrackBG(IGraphics& g, IRECT& r, int chIdx) {
        g.FillRect(GetColor(kHL), r);
      }

      virtual void DrawTrackHandle(IGraphics& g, IRECT& r, int chIdx) {
        IRECT fillRect;
        float value = *GetTrackData(chIdx);
        if (mDrawDb) value = std::pow<float>(((float)AmpToDB(value) - mMinDb) / (mMaxDb - mMinDb), 1.0f / mShape);
        fillRect = r.FracRect(mDirection, (!mDrawInverted) ? value : 1.f - value, mDrawFromTop); // HERE the value rect is drawn!
        g.FillRect(GetColor(kFG), fillRect); // TODO: shadows!

        IRECT peakRect;
        if (mDirection == kVertical)
          peakRect = (!mDrawFromTop) ? fillRect.GetFromTop(mPeakSize) : fillRect.GetFromBottom(mPeakSize);
        else
          peakRect = (!mDrawFromTop) ? fillRect.GetFromRight(mPeakSize) : fillRect.GetFromLeft(mPeakSize);
        DrawPeak(g, peakRect, chIdx);
      }

      virtual void DrawPeak(IGraphics& g, IRECT& r, int chIdx) {
        g.FillRect(GetColor(kX1), r);
      }

      void OnMsgFromDelegate(int messageTag, int dataSize, const void* pData) override {
        IByteStream stream(pData, dataSize);

        int pos = 0;
        Data data;
        pos = stream.Get(&data.nchans, pos);

        while (pos < stream.Size()) {
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
      float mMinDb;
      float mMaxDb;
      float mMaxTrackValue;
      int mLabelStep;
      int mMarkStep;
      int mNumLabels;
      int mNumLines;
      float mShape;
      IText mText;
      LabelFrames* mLabelFrames;
      LinePos* mLineCoord;
      IRECT rectLabelFrame;
      //IPattern mPattern;
    };

    /** A basic control to fill a rectangle with a color or gradient */
    class SRPanel : public IControl {
    public:
      SRPanel(IRECT bounds, const IColor& color, bool drawFrame = false)
        : IControl(bounds, kNoParameter)
        , mPattern(color)
        , mDrawFrame(drawFrame)
      {
        mIgnoreMouse = true;
      }

      SRPanel(IRECT bounds, const IPattern& pattern, bool drawFrame = false)
        : IControl(bounds, kNoParameter)
        , mPattern(pattern)
        , mDrawFrame(drawFrame)
      {
        mIgnoreMouse = true;
      }

      void Draw(IGraphics& g) override {
        if (g.HasPathSupport()) {
          g.PathRect(mRECT);
          g.PathFill(mPattern);
        }
        else {
          g.FillRect(mPattern.GetStop(0).mColor, mRECT);
        }

        if (mDrawFrame) {
          g.DrawRect(COLOR_LIGHT_GRAY, mRECT);
        }
      }

      void SetPattern(const IPattern& pattern) {
        mPattern = pattern;
        SetDirty(false);
      }

    private:
      IPattern mPattern;
      bool mDrawFrame;
    };



    // Preset Menu
// -----------------------------------

    class SRPresetMenu : public IControl {
    public:
      SRPresetMenu(IPlug *pPlug, IRECT bounds, IText pText, const char** pNamedParams)
        : IControl(bounds, -1) {
        mTextEntryLength = MAX_PRESET_NAME_LEN - 3;
        mText = pText;
        mPlug = pPlug;
        mNamedParams = pNamedParams;
      }
      void Draw(IGraphics& g) override;
      void OnMouseDown(float x, float y, const IMouseMod& mod) override;
      void doPopupMenu(IGraphics& g);
      void TextFromTextEntry(const char* txt);
    private:
      WDL_String mDisp;
      IPlug* mPlug;
      const char** mNamedParams;
    };


    // TODO: Draw with PathConvexShape from ptr to member array updated from Updatefunction
    class SRFrequencyResponseMeter : public IControl
      , public IVectorBase
    {
    public:
      SRFrequencyResponseMeter(IRECT bounds, int numValues, double* values, double shape = 1.0, const IVColorSpec& colorSpec = DEFAULT_SPEC)
        : IControl(bounds, -1)
        , IVectorBase(colorSpec)
        , mValues(values)
        , mNumValues(numValues)
        , mShape(shape)
        , mPatternFill(IPattern(EPatternType::kSolidPattern))
        , mPatternStroke(IPattern(EPatternType::kSolidPattern))
      {
        mStrokeOptions.mPreserve = true;
        mFillOptions.mPreserve = true;
        mPatternStroke = IPattern(GetColor(kFG));
        mPatternFill = IPattern(GetColor(kHL));
        AttachIControl(this);
      }

      ~SRFrequencyResponseMeter() {}

      void Draw(IGraphics& g) override {
        g.PathClear();
        g.PathMoveTo(mRECT.L, mRECT.MH());

        for (int i = 0; i < mNumValues; i++) {
          const float y = mRECT.MH() - (mValues[i] * 0.5 * mRECT.H());
          const float x = mRECT.L + ((float)i / ((float)mNumValues - 1.f)) * mRECT.W();
          g.PathLineTo(x, y);
        }

        g.PathLineTo(mRECT.R, mRECT.MH());
        //g.PathClose();
        g.PathFill(mPatternFill, mFillOptions, 0);
        g.PathStroke(mPatternStroke, 1.f, mStrokeOptions, 0);
      };

      void UpdateValues(double* values) {
        mValues = values;
        for (int i = 0; i < mNumValues; i++) {
          mValues[i] = Clip<double>(mValues[i], -1, 1);
        }
        SetDirty(false);
      };
      //void OnMouseDown(float x, float y, const IMouseMod& mod) override;
    private:
      //WDL_String mDisp;
      double* mValues;
      int mNumValues;
      double mShape;
      IStrokeOptions mStrokeOptions;
      IFillOptions mFillOptions;
      IPattern mPatternFill;
      IPattern mPatternStroke;
    };

  }
} // End namespaces