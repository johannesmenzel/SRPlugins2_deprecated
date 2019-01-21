#pragma once
#include "../SRClasses/SRConstants.h"
#include <stddef.h>

enum EPrograms {
  defaultProgram = 0,
  kick,
  kNumPrograms
};

//const int kNumPrograms = 1;
const double METER_ATTACK = .6, METER_DECAY = .05; // This is the global attack and release constants for meters
const int circularBufferLenght = 65536; // should be replaced with a queue somewhen

// Plugin's parameters
enum EParams
{
  // Input Stage
  kInputGain = 0,
  kSaturationType,
  kSaturationDrive,
  kSaturationAmount,
  kSaturationHarmonics,
  kSaturationSkew,
  kOversamplingRate,
  // EQ Stage
  // -- Filter
  kEqHpFreq,
  kEqLpFreq,
  kEqHpOrder,
  //kEqLpOrder,
  kEqHfGain,
  // -- EQ
  kEqHfFreq,
  //kEqHfQ,
  kEqHfBell,
  kEqHmfGain,
  kEqHmfFreq,
  kEqHmfQ,
  kEqLmfGain,
  kEqLmfFreq,
  kEqLmfQ,
  kEqLfGain,
  kEqLfFreq,
  //kEqLfQ,
  kEqLfBell,
  kEqAmount,
  // RMS Compressor
  kCompRmsThresh,
  kCompRmsRatio,
  kCompRmsAttack,
  kCompRmsRelease,
  kCompRmsKneeWidthDb,
  kCompRmsMakeup,
  kCompRmsIsFeedback,
  kCompRmsIsExrSc,
  // Peak Compressor
  kCompPeakThresh,
  kCompPeakRatio,
  kCompPeakAttack,
  kCompPeakRelease,
  kCompPeakKneeWidthDb,
  kCompPeakMakeup,
  kCompPeakSidechainFilterFreq,
  kCompPeakIsFeedback,
  kCompPeakIsExtSc,
  // Compressor
  kCompIsParallel,
  kCompPeakRmsRatio,
  kCompDryWet,
  // Deesser
  kDeesserFreq,
  kDeesserQ,
  kDeesserThresh,
  kDeesserRatio,
  kDeesserAttack,
  kDeesserRelease,
  kDeesserMakeup,
  // Output Stage
  kPan,
  kPanFreq,
  kIsPanMonoLow,
  kLimiterThresh,
  kClipperThreshold,
  kAgc,
  kOutputGain,
  // Bypasses
  kInputBypass,
  kEqBypass,
  kCompBypass,
  kOutputBypass,
  kBypass,
  // Test Parameters
  //kTestParam1,
  //kTestParam2,
  //kTestParam3,
  //kTestParam4,
  //kTestParam5,
  kNumParams
};

// Plugins control tags
enum ECtrlTags {

  // Input Stage
  cInputGain = 0,
  cSaturationType,
  cSaturationDrive,
  cSaturationAmount,
  cSaturationHarmonics,
  cSaturationSkew,
  cOversamplingRate,

  // EQ Stage

  // -- Filter
  cEqHpFreq,
  cEqLpFreq,
  cEqHpOrder,
  //cEqLpOrder,

  // -- EQ
  cEqHfGain,
  cEqHfFreq,
  //cEqHfQ,
  cEqHfBell,
  cEqHmfGain,
  cEqHmfFreq,
  cEqHmfQ,
  cEqLmfGain,
  cEqLmfFreq,
  cEqLmfQ,
  cEqLfGain,
  cEqLfFreq,
  //cEqLfQ,
  cEqLfBell,
  cEqAmount,

  // RMS Compressor
  cCompRmsThresh,
  cCompRmsRatio,
  cCompRmsAttack,
  cCompRmsRelease,
  cCompRmsKneeWidthDb,
  cCompRmsMakeup,
  cCompRmsIsFeedback,
  cCompRmsIsExrSc,

  // Peak Compressor
  cCompPeakThresh,
  cCompPeakRatio,
  cCompPeakAttack,
  cCompPeakRelease,
  cCompPeakKneeWidthDb,
  cCompPeakMakeup,
  cCompPeakSidechainFilterFreq,
  cCompPeakIsFeedback,
  cCompPeakIsExtSc,

  // Compressor
  cCompIsParallel,
  cCompPeakRmsRatio,
  cCompDryWet,

  // Deesser
  cDeesserFreq,
  cDeesserQ,
  cDeesserThresh,
  cDeesserRatio,
  cDeesserAttack,
  cDeesserRelease,
  cDeesserMakeup,

  // Output Stage
  cPan,
  cPanFreq,
  cIsPanMonoLow,
  cLimiterThresh,
  cClipperThreshold,
  cAgc,
  cOutputGain,

  // Bypasses
  cInputBypass,
  cEqBypass,
  cCompBypass,
  cOutputBypass,
  cBypass,

  // Test Parameters
  //cTestParam1,
  //cTestParam2,
  //cTestParam3,
  //cTestParam4,
  //cTestParam5,

  // UI Controls
  cBitmapLogo,
  cPanelHeader,
  cPanelFooter,
  cPanelInput,
  cPanelEq,
  cPanelComp,
  cPanelPost,
  cPanelOutput,
  cPanelMeter,
  cInputMeter,
  cGrMeter,
  cOutputMeter,
  cScope,
  cChannelCount,

  // Number of Controls
  kNumCtrlTags
};



enum EStages {
  stageInput = 0,
  stageEq,
  stageComp,
  stagePost,
  stageOutput,
  stageMeter,
  stageHeader,
  stageFooter,
  kNumStages
};

enum ESectionRects {
  RectInput = 0,
  RectEq,
  RectComp,
  RectPost,
  RectOutput,
  RectMeter,
  RectHeader,
  RectFooter,
  kNumSectionRects
};

const int sectionRectGridCells[kNumSectionRects][2] = {
{20,4},
{20,4},
{20,4},
{20,4},
{20,4},
{20,4},
{1,24},
{1,24}
};

/* Here's the hidden attempt on parameterProperties class

class ParamProperties {
public:
  ParamProperties();
  ParamProperties(int numParams)
  {
    mNumParams = numParams;
  };

  virtual ~ParamProperties() {};

  void SetNumParams(int numParams) {
    this->mNumParams = numParams;
  }
  virtual void AddParameterProperties(
    int paramIdx,
    int ctrlTag,
    const char* name, // Name of parameter displayed in host
    const char* shortName, // Short name for GUI display
    double defaultVal, // Plugin loads in this state, return by double click
    double minVal, // Minimum value of parameter
    double maxVal, // Maximum value of parameter
    double stepValue, // Controls dial steps / accuracy
    double centerVal, // Value that void setShapeCentered() will center (knobs middle position)
    double centerPoint, // Value WHERE void setShapeCentered() will center centerVal (0.5 means: real middle position)
    const char* label, // Measuring unit of parameter
    const char* group, // Parameter group, not supported by every host
    int Type, // Data type of parameter
    int Knobs, // Used control bitmap
    int AttachToControlPanel, // Panel where control should appear
    int x, // Controls horizontal position within panel
    int y, // Controls vertical position within panel
    const char* labelMin = "", // GUI display string of minimum
    const char* labelMax = "", // GUI display string of maximum
    const char* labelCtr = "", // GUI display string of middle position
    const char* tooltip = "" // Tooltip on mouseover)
  ) {
    mParamValues[paramIdx].mParamIdx = paramIdx;
    mParamValues[paramIdx].mCtrlTag = ctrlTag;
    mParamValues[paramIdx].mName = name; // Name of parameter displayed in host
    mParamValues[paramIdx].mShortName = shortName; // Short name for GUI display
    mParamValues[paramIdx].mDefaultVal = defaultVal; // Plugin loads in this state, return by double click
    mParamValues[paramIdx].mMinVal = minVal; // Minimum value of parameter
    mParamValues[paramIdx].mMaxVal = maxVal; // Maximum value of parameter
    mParamValues[paramIdx].mStepValue = stepValue; // Controls dial steps / accuracy
    mParamValues[paramIdx].mCenterVal = centerVal; // Value that void setShapeCentered() will center (knobs middle position)
    mParamValues[paramIdx].mCenterPoint = centerPoint; // Value WHERE void setShapeCentered() will center centerVal (0.5 means: real middle position)
    mParamValues[paramIdx].mLabel = label; // Measuring unit of parameter
    mParamValues[paramIdx].mGroup = group; // Parameter group, not supported by every host
    mParamValues[paramIdx].mType = Type; // Data type of parameter
    mParamValues[paramIdx].mKnobs = Knobs; // Used control bitmap
    mParamValues[paramIdx].mAttachToControlPanel = AttachToControlPanel; // Panel where control should appear
    mParamValues[paramIdx].mX = x; // Controls horizontal position within panel
    mParamValues[paramIdx].mY = y; // Controls vertical position within panel
    mParamValues[paramIdx].mLabelMin = labelMin; // GUI display string of minimum
    mParamValues[paramIdx].mLabelMax = labelMax; // GUI display string of maximum
    mParamValues[paramIdx].mLabelCtr = labelCtr; // GUI display string of middle position
    mParamValues[paramIdx].mTooltip = tooltip; // Tooltip on mouseover
  }

protected:
  int mNumParams;
  struct ParamValues
  {
    int mParamIdx;
    int mCtrlTag;
    const char* mName; // Name of parameter displayed in host
    const char* mShortName; // Short name for GUI display
    double mDefaultVal; // Plugin loads in this state; return by double click
    double mMinVal; // Minimum value of parameter
    double mMaxVal; // Maximum value of parameter
    double mStepValue; // Controls dial steps / accuracy
    double mCenterVal; // Value that void setShapeCentered() will center (knobs middle position)
    double mCenterPoint; // Value WHERE void setShapeCentered() will center centerVal (0.5 means: real middle position)
    const char* mLabel; // Measuring unit of parameter
    const char* mGroup; // Parameter group; not supported by every host
    int mType; // Data type of parameter
    int mKnobs; // Used control bitmap
    int mAttachToControlPanel; // Panel where control should appear
    int mX; // Controls horizontal position within panel
    int mY; // Controls vertical position within panel
    const char* mLabelMin; // GUI display string of minimum
    const char* mLabelMax; // GUI display string of maximum
    const char* mLabelCtr; // GUI display string of middle position
    const char* mTooltip; // Tooltip on mouseover
  } mParamValues[kNumParams];
};
*/




/*
Actual properties struct.
Keep in order of the above listet parameters.
These are constants.
*/
const StructParameterProperties parameterProperties[kNumParams] = {
  // { "NAME", "SNAME" DEF, MIN, MAX, STEP, CENTER, CTRPNT, "LABEL",GROUP , TYPE, KNOB, Stage, X, Y, "LBLMIN", "LBLMAX", "LBLCTR", "TOOLTIP"},
  // Input Stage
  { kInputGain, cInputGain, "Input Gain", "Input", 0., -60., 12., 0.1, 0., 10. / 12.,"dB", "Global", typeDouble, Fader, RectInput, 0, 2, "-60", "12", "0", "Input Gain is applied before everything else" },
  { kSaturationType, cSaturationType, "Saturation Type", "Type", 0, 0, 5, -1, -1, -1, "", "Input", typeEnum, none, RectInput, 2, 6, "", "", "", "Type of Saturation"},
  { kSaturationDrive, cSaturationDrive, "Sat Drive", "Drive", 0., 0., 60., 0.1, 30., .5, "dB", "Input", typeDouble, SslRed, RectInput, 0, 0, "0", "60", "30", "Saturation input drive, hits the saturation module harder" },
  { kSaturationAmount, cSaturationAmount, "Sat Amount", "Saturation", 0., 0., 99., 0.01, 10., .5, "%", "Input", typeDouble, SslOrange,RectInput, 2, 0, "0", "100", "10", "Amount of Saturation" },
  { kSaturationHarmonics, cSaturationHarmonics, "Harmonics", "Harmonics", 50., 0., 100., .01, 50., .5, "%", "Input", typeDouble, SslBlue, RectInput, 2, 2, "Even", "Odd", "Mix", "Dial in even harmonics by turning the knob counter-clockwise" },
  { kSaturationSkew, cSaturationSkew, "Sat Skew", "Skew", 0, -100., 100., 0.01, 0., .5, "%", "Input", typeDouble, SslOrange, RectInput, 2, 4, "-100", "100", "0", "Saturations positive/negative skewness. Distorts waveform" },
  { kOversamplingRate, cOversamplingRate, "Oversampling", "", 0, 0, 4, -1, -1, -1, "X", "Input", typeEnum, Button, RectInput, 2, 8, "", "", "", "Oversampling Rate"},
  // EQ Stage
  // -- Filter
  { kEqHpFreq, cEqHpFreq, "Highpass Freq", "Highpass", 16., 16., 350., 1., 120., .5, "Hz", "EQ", typeDouble, SslWhite, RectEq, 0, 1, "16", "350", "120", "Frequency of the High Pass Filter, turn down to deactivate" },
  { kEqLpFreq, cEqLpFreq, "Lowpass Freq", "LowPass", 22000., 3000., 22000., 1., 5000., .5, "Hz", "EQ", typeDouble, SslWhite, RectEq, 2, 0, "3k", "22k", "5k", "Set the frequency of the Low Pass Filter, turn up to deactivate" },
  { kEqHpOrder, cEqHpOrder, "HP Order", "", 1, 0, 8, 1, 5, .5, "dB/oct", "EQ", typeEnum, none, RectEq, 0, 0, "6", "120", "36", "Order of the Highpass Filter or filter slope" },
  //{ kEqLpOrder, cEqLpOrder, "LP Order", "", 1, 0, 8, 1, 5, .5, "dB/oct", "EQ", typeEnum, none, RectEq, 2, 2, "6", "120", "36", "HIDED - Order of the Lowpass Filter or filter slope" },
  // -- EQ
  { kEqHfGain, cEqHfGain, "HF Gain", "Gain", 0., -12., 12., .1, 0., .5, "dB", "EQ", typeDouble, SslRed, RectEq, 0, 3, "-12", "12", "0", "Gain of the high frequency band" },
  { kEqHfFreq, cEqHfFreq, "HF Freq", "Freq", 8000., 1500., 16000., 1., 8000., .5, "Hz", "EQ", typeDouble, SslRed, RectEq, 2, 4, "1.5k", "16k", "8k", "Frequency of the high frequency band" },
  // { kEqHfQ, cEqHfQ, "HF Q", "Q", stQ, 0.1, 10., 0.01, stQ, .5, "", "EQ", typeDouble, SslRed, RectEq, -1, 4, "W", "N", "", "Set the Q value of the high frequency band" },
  { kEqHfBell, cEqHfBell, "Hf Bell", "Bell", 0, 0, 1, 1, 0.5, .5, "", "EQ", typeBool, Button, RectEq, 2, 3, "SLF", "BLL", "", "Switches the high frequency band between bell and shelf" },
  { kEqHmfGain, cEqHmfGain, "HMF Gain", "Gain", 0., -12., 12., 0.1, 0., .5, "dB", "EQ", typeDouble, SslBlue, RectEq, 0, 5, "-12", "12", "0", "Gain of the upper mid frequency band" },
  { kEqHmfFreq, cEqHmfFreq, "HMF Freq", "Freq", 3000., 600., 7000., 1., 3000., .5, "Hz", "EQ", typeDouble, SslBlue, RectEq, 2, 6, "600", "7k", "3k", "Frequency of the upper mid frequency band" },
  { kEqHmfQ, cEqHmfQ, "HMF Q", "Q", stQ, 0.1, 10., 0.01, stQ, .5, "", "EQ", typeDouble, SslBlue, RectEq, 0, 7, "W", "N", "", "Set the Q value of the upper mid frequency band" },
  { kEqLmfGain, cEqLmfGain, "LMF Gain", "Gain", 0., -12., 12., 0.1, 0., .5, "dB", "EQ", typeDouble, SslGreen, RectEq, 0, 9, "-12", "12", "0", "Gain of the lower mid frequency band" },
  { kEqLmfFreq, cEqLmfFreq, "LMF Freq", "Freq", 1000., 200., 2500., 1., 1000., .5, "Hz", "EQ", typeDouble, SslGreen, RectEq, 2, 10, "200", "2.5k", "1k", "Frequency of the lower mid frequency band" },
  { kEqLmfQ, cEqLmfQ, "LMF Q", "Q", stQ, 0.1, 10., 0.01, stQ, .5, "", "EQ", typeDouble, SslGreen, RectEq, 0, 11, "W", "N", "", "Set the Q value of the lower mid frequency band" },
  { kEqLfGain, cEqLfGain, "LF Gain", "Gain", 0., -12., 12., 0.1, 0., .5, "dB", "EQ", typeDouble, SslBlack, RectEq, 0, 13, "-12", "12", "0", "Gain of the low frequency band" },
  { kEqLfFreq, cEqLfFreq, "LF Freq", "Freq", 200., 30., 450., 1., 200., .5, "Hz", "EQ", typeDouble, SslBlack, RectEq, 2, 12, "30", "450", "200", "Set the frequency of the low frequency band" },
  // { kEqLfQ, cEqLfQ, "LF Q", "Q", stQ, 0.1, 10., 0.01, stQ, .5, "", "EQ", typeDouble, SslBlack, RectEq, -1, 12, "W", "N", "", "Set the Q value of the low frequency band" },
  { kEqLfBell, cEqLfBell, "Lf Bell", "Bell", 0, 0, 1, 0.1, 0.5, .5, "", "EQ", typeBool, Button, RectEq, 2, 14, "SLF", "BLL", "", "Switches the low frequency band between bell and shelf" },
  { kEqAmount, cEqAmount, "EQ Amount", "Amount", 100.0, -100.0, 100.0, 0.1, 0., .5, "%", "EQ", typeDouble, SslWhite, RectEq, 2, 16, "-100", "100", "0", "Sets Equalizers processong amount"},
  // RMS Compressor
  { kCompRmsThresh, cCompRmsThresh, "RMS Thresh", "Thresh", 0., -40., 0., 0.1, -20., .5, "dB", "Compressor",typeDouble, SslWhite, RectComp, 0, 0, "-40", "0", "-20", "Threshold of RMS Compressor" },
  { kCompRmsRatio, cCompRmsRatio, "RMS Ratio", "Ratio", 3., 0.5, 20., 0.01, 3., .5, ":1", "Compressor",typeDouble, SslWhite, RectComp, 2, 1, ".5:1", "20:1", "3:1", "Ratio of RMS Compressor" },
  { kCompRmsAttack, cCompRmsAttack, "RMS Attack", "Attack", 20., 10., 200., 0.01, 20., .5, "ms", "Compressor",typeDouble, SslWhite, RectComp, 0, 2, "10", "200", "20", "Attack of RMS Compressor" },
  { kCompRmsRelease, cCompRmsRelease, "RMS Release", "Release", 200., 100., 5000., 1., 200., .5, "ms", "Compressor",typeDouble, SslWhite, RectComp, 2, 3, "100", "5k", "200", "Release of RMS Compressor" },
  { kCompRmsKneeWidthDb, cCompRmsKneeWidthDb, "RMS Knee", "Knee", 0., 0., 30., 0.1, 10., .5, "dB", "Compressor",typeDouble, SslGreen, RectComp, 2, 5, "0", "30", "10", "RMS Compressors width of the soft knee" },
  { kCompRmsMakeup, cCompRmsMakeup, "RMS Makeup", "Makeup", 0., 0., 40., 0.1, 10., .5, "dB", "Compressor",typeDouble, SslBlack, RectComp, 0, 4, "0", "40", "10", "Makeup gain of RMS Compressor" },
  { kCompRmsIsFeedback, cCompRmsIsFeedback, "RMS Topology", "Topology", 0, 0, 1, 0.1, 0.5, .5, "", "Compressor",typeBool, Button, RectComp, 0, 6, "FWD", "BCK", "", "RMS Compressors Topology, either feed forward or feedback" },
  { kCompRmsIsExrSc, cCompRmsIsExrSc, "RMS Ext SC", "Ext", 0, 0, 1, 0.1, 0.5, .5, "", "Compressor",typeBool, Button,RectComp, 2, 0, "INT", "EXT", "", "External sidechain for RMS Compressor. Use tracks input channel 3/4" },
  // Peak Compressor
  { kCompPeakThresh, cCompPeakThresh, "Peak Thresh", "Thresh", 0., -40., 0., 0.1, -20., .5, "dB", "Compressor",typeDouble, SslWhite, RectComp, 0, 8, "-40", "0", "-20", "Threshold of Peak Compressor" },
  { kCompPeakRatio, cCompPeakRatio, "Peak Ratio", "Ratio", 3., 0.5, 20., 0.01, 3., .5, ":1", "Compressor",typeDouble, SslWhite, RectComp, 2, 9, ".5:1", "20:1", "3:1", "Ratio of Peak Compressor" },
  { kCompPeakAttack, cCompPeakAttack, "Peak Attack", "Attack", 20., 0.01, 200., 0.01, 20., .5, "ms", "Compressor",typeDouble, SslWhite, RectComp, 0, 10, "0.01", "200", "20", "Attack of Peak Compressor" },
  { kCompPeakRelease, cCompPeakRelease, "Peak Release", "Release", 200., 1., 2000., 0.01, 200., .5, "ms", "Compressor",typeDouble, SslWhite, RectComp, 2, 11, "1", "2k", "200", "Release of Peak Compressor" },
  { kCompPeakKneeWidthDb, cCompPeakKneeWidthDb, "Peak Knee", "Knee", 0., 0., 30., 0.1, 10., .5, "dB", "Compressor",typeDouble, SslGreen, RectComp, 2, 13, "0", "30", "10", "Peak Compressors width of the soft knee" },
  { kCompPeakMakeup, cCompPeakMakeup, "Peak Makeup", "Makeup", 0., 0., 40., 0.1, 10., .5, "dB", "Compressor",typeDouble, SslBlack, RectComp, 0, 12, "0", "40", "10", "Makeup Gain of Peak Compressor" },
  { kCompPeakSidechainFilterFreq, cCompPeakSidechainFilterFreq, "Peak SC Filter", "Filter", 16., 16., 5000., 1., 1000., .5, "Hz", "Compressor",typeDouble, SslBlue, RectComp, 0, 14, "16", "5k", "1k", "Frequency of the Compressors sidechain highpass filter" },
  { kCompPeakIsFeedback, cCompPeakIsFeedback, "Peak Topology", "Topology", 0, 0, 1, 0.1, 0.5, .5, "", "Compressor",typeBool, Button, RectComp, 2, 15, "FWD", "BCK", "", "Peak Compressors Topology, either feed forward or feedback" },
  { kCompPeakIsExtSc, cCompPeakIsExtSc, "Peak Ext SC", "Ext", 0, 0, 1, 0.1, 0.5, .5, "", "Compressor",typeBool, Button, RectComp, 2, 8, "INT", "EXT", "", "External sidechain for Peak Compressor. Use tracks input channel 3/4" },
  // Compressor
  { kCompIsParallel, cCompIsParallel, "Comp Ser/Par", "Parallel", 0, 0, 1, 0.1, 0.5, .5, "", "Compressor",typeBool, Button, RectFooter, 10, 0, "SER", "PAR", "", "RMS and Peak Compressor can be run serial or parallel" },
  { kCompPeakRmsRatio, cCompPeakRmsRatio, "RMS/Peak Ratio", "Ratio", 50., 0., 100., 0.1, 50., .5, "%", "Compressor",typeDouble, SslBlack, RectComp, 0, 18, "0", "100", "50", "Mix the signal of the RMS and Peak Compressor" },
  { kCompDryWet, cCompDryWet, "Dry/Wet", "Mix", 100., 0., 100., 1., 50., .5, "%", "Compressor",typeDouble, SslBlack, RectComp, 2, 18, "0", "100", "50", "Do parallel compression by dialing in the uncompressed signal" },
  // Deesser
  { kDeesserFreq, cDeesserFreq, "Deesser Freq", "Freq", 7000., 16., 22000., 1., 7000., .5, "Hz", "Deesser", typeDouble, SslYellow, RectPost, 0, 12, "16", "22k", "7k", "Deessers center frequency" },
  { kDeesserQ, cDeesserQ, "Deesser Q", "Q", stQ, 0.1, 10., 0.01, stQ, .5, "", "Deesser", typeDouble, SslYellow, RectPost, 2, 12, "W", "N", "", "Deessers width (Q)" },
  { kDeesserThresh, cDeesserThresh, "Deesser Thresh", "Thresh", 0., -100., 0., 0.1, -50., .5, "dB", "Deesser", typeDouble, SslYellow, RectPost, 0, 14, "-100", "0", "-50", "Deessers Threshold" },
  { kDeesserRatio, cDeesserRatio, "Deesser Ratio", "Ratio", 3., 0.5, 20., 0.01, 3., .5, ":1", "Deesser", typeDouble, SslYellow, RectPost, 2, 14, ".5:1", "20:1", "3:1", "Deessers Ratio" },
  { kDeesserAttack, cDeesserAttack, "Deesser Attack", "Attack", 20., 0.01, 200., 0.01, 20., .5, "ms", "Deesser", typeDouble, SslYellow, RectPost, 0, 16, "0.01", "200", "20", "Deessers Attack" },
  { kDeesserRelease, cDeesserRelease, "Deesser Release", "Release", 200., 1., 2000., 0.01, 200., .5, "ms", "Deesser", typeDouble, SslYellow, RectPost, 2, 16, "1", "2k", "200", "Deessers Release" },
  { kDeesserMakeup, cDeesserMakeup, "Deesser Makeup", "Makeup", 0., 0., 40., 0.1, 10., .5, "dB", "Deesser", typeDouble, SslYellow, RectPost, 0, 18, "0", "40", "10", "Deessers Makeup" },
  // Output Stage
  { kPan, cPan, "Pan", "Pan", 0., -100., 100., 1., 0., .5, "%", "Output", typeDouble, SslBlue, RectOutput, 0, 0, "L", "R", "C", "Pan" },
  { kPanFreq, cPanFreq, "Pan Freq", "XFreq", 150., 20., 1000., 1., 150., .5, "Hz", "Output", typeDouble, SslRed, RectOutput, 0, 2, "20", "1k", "200", "Frequencies below crossover will be not affected by panner" },
  { kIsPanMonoLow, cIsPanMonoLow, "Mono Bass", "Mono Bass", 0, 0, 1, 0.1, 0.5, .5, "", "Output", typeBool, Button, RectOutput, 0, 6, "STB", "MNB", "", "Bass Frequencies will left stereo or will be monoed" },
  { kLimiterThresh, cLimiterThresh, "Limiter", "Limiter", 10., -30., 10., 0.1, 0., .5, "dB", "Output", typeDouble, SslOrange, RectOutput, 0, 4, "-30", "10", "0", "Thresold of Output Limiter" },
  { kClipperThreshold, cClipperThreshold, "Clipper", "Clipper", 0., 0., 99., 0.01, 50., .5, "%", "Output", typeDouble, SslYellow, RectOutput, 2, 0, "0", "100", "50", "Amount of the Output Clipper (cuts amplitutde)" },
  { kAgc, cAgc, "Auto Gain Comp", "AGC", 0, 0, 1, 0.1, 0.5, .5, "", "Output", typeBool, Button, RectOutput, 0, 8, "AGC", "", "", "Automatic Gain Conpensation. Click to adjust output volume to input volume" },
  { kOutputGain, cOutputGain, "Output Gain", "Output", 0., -60., 12., 0.1, 0., 10. / 12.,"dB", "Global", typeDouble, Fader, RectOutput, 2, 2, "-60", "12", "0", "Output Gain" },
  // Bypasses
  { kInputBypass, cInputBypass, "Input Bypass", "In Byp", 0, 0, 1, 0.1, 0.5, .5, "", "Global", typeBool, Button, RectFooter, 0, 0, "IN ON", "IN OFF", "", "Bypass Input Section" },
  { kEqBypass, cEqBypass, "EQ Bypass", "EQ Byp", 0, 0, 1, 0.1, 0.5, .5, "", "Global", typeBool, Button, RectFooter, 4, 0, "EQ ON", "EQ OFF", "", "Bypass Equalizer Section" },
  { kCompBypass, cCompBypass, "Comp Bypass", "Comp Byp", 0, 0, 1, 0.1, 0.5, .5, "", "Global", typeBool, Button, RectFooter, 8, 0, "DYN ON", "DYN OFF", "", "Bypass Compressor Section" },
  { kOutputBypass, cOutputBypass, "Output Bypass", "Out Byp", 0, 0, 1, 0.1, 0.5, .5, "", "Global", typeBool, Button, RectFooter, 16, 0, "OUT ON", "OUT OFF", "", "Bypass Output Section" },
  { kBypass, cBypass, "Bypass", "Byp", 0, 0, 1, 0.1, 0.5, .5, "", "Global", typeBool, Button, RectFooter, 20, 0, "ON", "OFF", "", "Bypass Plugin" },
  // Test Parameters
  // { kTestParam1, cTestParam1, "TestParam 1", "T1", 0.2, 0.2, 5., .0001, .5, .5, "", "Test", typeDouble, SslRed, RectFooter, 0, 0, "0", "1", "0.5", "HIDED - Generic control for development tests" },
  // { kTestParam2, cTestParam2, "TestParam 2", "T2", 0.2, 0.2, 5., .0001, .5, .5, "", "Test", typeDouble, SslRed, RectFooter, 1, 0, "0", "1", "0.5", "HIDED - Generic control for development tests" },
  // { kTestParam3, cTestParam3, "TestParam 3", "T3", 0., 0., 1., .0001, .5, .5, "", "Test", typeDouble, SslRed, RectFooter, 2, 0, "0", "1", "0.5", "HIDED - Generic control for development tests" },
  // { kTestParam4, cTestParam4, "TestParam 4", "T4", 0., 0., 1., .0001, .5, .5, "", "Test", typeDouble, SslRed, RectFooter, 3, 0, "0", "1", "0.5", "HIDED - Generic control for development tests" },
  // { kTestParam5, cTestParam5, "TestParam 5", "T5", 0., 0., 1., .0001, .5, .5, "", "Test", typeDouble, SslRed, RectFooter, 4, 0, "0", "1", "0.5", "HIDED - Generic control for development tests" }
  // { "NAME", "SNAME" DEF, MIN, MAX, STEP, CENTER, CTRPNT, "LABEL",GROUP , TYPE, KNOB, X, Y, "LBLMIN", "LBLMAX", "LBLCTR", "TOOLTIP"},
};
