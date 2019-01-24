#pragma once
#include "IPlug_include_in_plug_hdr.h"

// PLUG ELEMENTS
// -------------

// All parameters possible data types listed here
enum EParamDataType {
  typeDouble = 0,
  typeBool,
  typeInt,
  typeEnum,
  kNumDataType
};

//inline double SetShapeCentered(double cMinValue, double cMaxValue, double cCenteredValue, double cControlPosition) {
//  return log((cCenteredValue - cMinValue) / (cMaxValue - cMinValue)) / log(cControlPosition);
//}
//
//void InitDoubleShapeFromMiddlePosition(IPluginBase* plug, int paramIdx, const char* name, double defaultVal = 0., double minVal = 0., double maxVal = 1., double step = 0.001, const char* label = "", int flags = 0, const char* group = "", double centerVal = 0.5, double centerPoint = 0.5, IParam::EParamUnit unit = IParam::kUnitCustom, IParam::DisplayFunc displayFunc = nullptr) {
//  plug->GetParam(paramIdx)->InitDouble(name, defaultVal, minVal, maxVal, step, label, flags, group, new IParam::ShapePowCurve(SetShapeCentered(minVal, maxVal, centerVal, centerPoint)), unit, displayFunc);
//}

// Struct object containing possible parameters properties
struct SRParamProperties {
  const int paramIdx;
  const int ctrlTag;
  const char* name; // Name of parameter displayed in host
  const char* shortName; // Short name for GUI display
  const double defaultVal; // Plugin loads in this state, return by double click
  const double minVal; // Minimum value of parameter
  const double maxVal; // Maximum value of parameter
  const double stepValue; // Controls dial steps / accuracy
  const double centerVal; // Value that void setShapeCentered() will center (knobs middle position)
  const double centerPoint = .5; // Value WHERE void setShapeCentered() will center centerVal (0.5 means: real middle position)
  const char* label; // Measuring unit of parameter
  const char* group; // Parameter group, not supported by every host
  const int Type; // Data type of parameter
  const int Knobs; // Used control bitmap
  const int AttachToControlPanel; // Panel where control should appear
  const int x; // Controls horizontal position within panel
  const int y; // Controls vertical position within panel
  const char* labelMin = ""; // GUI display string of minimum
  const char* labelMax = ""; // GUI display string of maximum
  const char* labelCtr = ""; // GUI display string of middle position
  const char* tooltip = ""; // Tooltip on mouseover
/*
  StructParameterProperties() :
    paramIdx(-1),
    ctrlTag(-1),
    name(""),
    shortName(""),
    defaultVal(0.0),
    minVal(0.0),
    maxVal(1.0),
    stepValue(0.001),
    centerVal(0.5),
    centerPoint(0.5),
    label(""),
    group(""),
    Type(-1),
    Knobs(-1),
    AttachToControlPanel(-1),
    x(0),
    y(0),
    labelMin(""),
    labelMax(""),
    labelCtr(""),
    tooltip("") {}
  const char* GetName(int paramIdx) { return parameterProperties[paramIdx].name; };
  const char* GetShortName(int paramIdx) { return parameterProperties[paramIdx].shortName; }; // Short name for GUI display
  const double GetDefaultVal(int paramIdx) { return parameterProperties[paramIdx].defaultVal; }; // Plugin loads in this state, return by double click
  const double GetMinVal(int paramIdx) { return parameterProperties[paramIdx].minVal; }; // Minimum value of parameter
  const double GetMaxVal(int paramIdx) { return parameterProperties[paramIdx].maxVal; }; // Maximum value of parameter
  const double GetStepValue(int paramIdx) { return parameterProperties[paramIdx].stepValue; };// Controls dial steps / accuracy
  const double GetCenterVal(int paramIdx) { return parameterProperties[paramIdx].centerVal; }; // Value that void setShapeCentered() will center (knobs middle position)
  const double GetCenterPoint(int paramIdx) { return parameterProperties[paramIdx].centerPoint; }; // Value WHERE void setShapeCentered() will center centerVal (0.5 means: real middle position)
  const char* GetLabel(int paramIdx) { return parameterProperties[paramIdx].label; }; // Measuring unit of parameter
  const char* GetGroup(int paramIdx) { return parameterProperties[paramIdx].group; }; // Parameter group, not supported by every host
  const int GetType(int paramIdx) { return parameterProperties[paramIdx].Type; }; // Data type of parameter
  const int GetKnobs(int paramIdx) { return parameterProperties[paramIdx].Knobs; }; // Used control bitmap
  const int GetAttachToControlPanel(int paramIdx) { return parameterProperties[paramIdx].AttachToControlPanel; }; // Panel where control should appear
  const int GetX(int paramIdx) { return parameterProperties[paramIdx].x; }; // Controls horizontal position within panel
  const int GetY(int paramIdx) { return parameterProperties[paramIdx].y; }; // Controls vertical position within panel
  const char* GetLabelMin(int paramIdx) { return parameterProperties[paramIdx].labelMin; }; // GUI display string of minimum
  const char* GetLabelMax(int paramIdx) { return parameterProperties[paramIdx].labelMax; }; // GUI display string of maximum
  const char* GetLabelCtr(int paramIdx) { return parameterProperties[paramIdx].labelCtr; }; // GUI display string of middle position
  const char* GetTooltip(int paramIdx) { return parameterProperties[paramIdx].tooltip; }; // Tooltip on mouseover
 */
};

// DSP ELEMENTS
// ------------

// Pass Filter Slopes in dB/oct listed here
enum EFilterSlope {
  dbo6 = 0,
  dbo12,
  dbo18,
  dbo24,
  dbo36,
  dbo48,
  dbo60,
  dbo72,
  dbo120,
  kNumOrders
};

const double halfpi = PI / 2;								// Half pi defined
const double dcoff = 1e-15;									// amount of DC offset added and subtracted respectivly at the beginning and the end of DoubleReplacing loop

// Setting the Q constants for higher order low- and highpass filter.
const double mEqPassQ_O2_F1 = sqrt(0.5);
const double stQ = mEqPassQ_O2_F1; // standard Q value
const double mEqPassQ_O3_F1 = 1.;
const double mEqPassQ_O4_F1 = 0.54119610;
const double mEqPassQ_O4_F2 = 1.3065630;
const double mEqPassQ_O6_F1 = 0.51763809;
const double mEqPassQ_O6_F2 = 0.70710678;
const double mEqPassQ_O6_F3 = 1.9318517;
const double mEqPassQ_O8_F1 = 0.50979558;
const double mEqPassQ_O8_F2 = 0.60134489;
const double mEqPassQ_O8_F3 = 0.89997622;
const double mEqPassQ_O8_F4 = 2.5629154;
const double mEqPassQ_O10_F1 = 0.50623256;
const double mEqPassQ_O10_F2 = 0.56116312;
const double mEqPassQ_O10_F3 = 0.70710678;
const double mEqPassQ_O10_F4 = 1.1013446;
const double mEqPassQ_O10_F5 = 3.1962266;
const double mEqPassQ_O12_F1 = 0.50431448;
const double mEqPassQ_O12_F2 = 0.54119610;
const double mEqPassQ_O12_F3 = 0.63023621;
const double mEqPassQ_O12_F4 = 0.82133982;
const double mEqPassQ_O12_F5 = 1.3065630;
const double mEqPassQ_O12_F6 = 3.8306488;
const double mEqPassQ_O20_F1 = 0.50154610;
const double mEqPassQ_O20_F2 = 0.51420760;
const double mEqPassQ_O20_F3 = 0.54119610;
const double mEqPassQ_O20_F4 = 0.58641385;
const double mEqPassQ_O20_F5 = 0.65754350;
const double mEqPassQ_O20_F6 = 0.76988452;
const double mEqPassQ_O20_F7 = 0.95694043;
const double mEqPassQ_O20_F8 = 1.3065630;
const double mEqPassQ_O20_F9 = 2.1418288;
const double mEqPassQ_O20_F10 = 6.3727474;


// UI ELEMENTS
// -----------

// All possible controls listed here
enum EControlImages {
  SslBlue = 0,
  SslGreen,
  SslRed,
  SslOrange,
  SslYellow,
  SslBlack,
  SslWhite,
  AbbeyChicken,
  Button,
  Fader,
  none,
  kNumKnobs
};

// Plugin constants
const struct {
  const IColor colorPanelBG = IColor(255, 37, 53, 69);
  const IColor colorPluginBG = IColor(255, 13, 18, 23);
  const IColor colorFG = IColor(255, 48, 166, 186);
  const IColor colorPR = IColor(255, 200, 200, 200);
  const IColor colorFR = IColor(255, 70, 70, 70);
  const IColor colorHL = IColor(30, 255, 255, 255);
  const IColor colorSH = IColor(100, 0, 0, 0);
  const IColor colorX1 = IColor(255, 249, 206, 34);
  const IColor colorX2 = IColor(255, 249, 206, 34);
  const IColor colorX3 = IColor(255, 249, 206, 34);
  const IColor colorKnobSslBlue = IColor(255, 62, 100, 121);
  const IColor colorKnobSslRed = IColor(255, 131, 18, 18);
  const IColor colorKnobSslGreen = IColor(255, 103, 141, 52);
  const IColor colorKnobSslOrange = IColor(255, 234, 158, 19);
  const IColor colorKnobSslYellow = IColor(255, 219, 181, 30);
  const IColor colorKnobSslBlack = IColor(255, 23, 23, 23);
  const IColor colorKnobSslWhite = IColor(255, 243, 243, 243);
  const IColor colorDefaultText = COLOR_LIGHT_GRAY;
  const int textSize = 14;
  const IText textKnobLabel = IText(textSize, colorDefaultText, nullptr, IText::kStyleBold, IText::kAlignCenter, IText::kVAlignBottom, 0, IText::kQualityClearType, colorPanelBG, colorDefaultText);
  const IText textKnobValue = IText(textSize, colorDefaultText, nullptr, IText::kStyleNormal, IText::kAlignCenter, IText::kVAlignBottom, 0, IText::kQualityClearType, colorPanelBG, colorDefaultText);
  const IText textVersionString = IText(20, colorDefaultText, nullptr, IText::kStyleNormal, IText::kAlignNear, IText::kVAlignMiddle, 0, IText::kQualityClearType, colorPanelBG, colorDefaultText);
} pluginLayout;

const IVColorSpec SR_SPEC = {
  DEFAULT_BGCOLOR,      // Background (DEFAULT_BGCOLOR = COLOR_TRANSPARENT(0, 0, 0, 0))
  pluginLayout.colorFG, // Foreground (DEFAULT_FGCOLOR = COLOR_MID_GRAY(255, 200, 200, 200))
  pluginLayout.colorPR, // Pressed    (DEFAULT_PRCOLOR = COLOR_LIGHT_GRAY(255, 240, 240, 240))
  pluginLayout.colorFR, // Frame      (DEFAULT_FRCOLOR = COLOR_DARK_GRAY(255, 70, 70, 70))
  pluginLayout.colorHL, // Higlight   (DEFAULT_HLCOLOR = COLOR_TRANSLUCENT(10, 0, 0, 0))
  pluginLayout.colorSH, // Shadow     (DEFAULT_SHCOLOR = IColor(60, 0, 0, 0)
  pluginLayout.colorX1, // Extra 1    (DEFAULT_X1COLOR = COLOR_RED(255, 255, 0, 0))
  pluginLayout.colorX2, // Extra 2    (DEFAULT_X2COLOR = COLOR_GREEN(255, 0, 255, 0))
  pluginLayout.colorX3  // Extra 3    (DEFAULT_X3COLOR = COLOR_BLUE(255, 0, 0, 255))
};