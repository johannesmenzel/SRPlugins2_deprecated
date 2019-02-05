#pragma once
#include "IPlug_include_in_plug_hdr.h"

// PLUG ELEMENTS
// -------------

//inline double SetShapeCentered(double cMinValue, double cMaxValue, double cCenteredValue, double cControlPosition) {
//  return log((cCenteredValue - cMinValue) / (cMaxValue - cMinValue)) / log(cControlPosition);
//}
//
//void InitDoubleShapeFromMiddlePosition(IPluginBase* plug, int paramIdx, const char* name, double defaultVal = 0., double minVal = 0., double maxVal = 1., double step = 0.001, const char* label = "", int flags = 0, const char* group = "", double centerVal = 0.5, double centerPoint = 0.5, IParam::EParamUnit unit = IParam::kUnitCustom, IParam::DisplayFunc displayFunc = nullptr) {
//  plug->GetParam(paramIdx)->InitDouble(name, defaultVal, minVal, maxVal, step, label, flags, group, new IParam::ShapePowCurve(SetShapeCentered(minVal, maxVal, centerVal, centerPoint)), unit, displayFunc);
//}


  /** custom parameter shaping from middle position */
struct ShapeFromMiddle : public IParam::Shape
{
  ShapeFromMiddle(double minVal = 0.0, double maxVal = 1.0, double centeredVal = 0.5, double pos = 0.5) : mShape(log((centeredVal - minVal) / (maxVal - minVal)) / log(pos))
  {
  };
  Shape* Clone() const override { return new ShapeFromMiddle(); };
  IParam::EDisplayType GetDisplayType() const override
  {
    if (mShape > 2.5) return IParam::EDisplayType::kDisplayCubeRoot;
    if (mShape > 1.5) return IParam::EDisplayType::kDisplaySquareRoot;
    if (mShape < (2.0 / 5.0)) return IParam::EDisplayType::kDisplayCubed;
    if (mShape < (2.0 / 3.0)) return IParam::EDisplayType::kDisplaySquared;

    return IParam::kDisplayLinear;
  }
  double NormalizedToValue(double value, const IParam& param) const override {
    return param.GetMin() + std::pow(value, mShape) * (param.GetMax() - param.GetMin());
  };
  double ValueToNormalized(double value, const IParam& param) const override {
    return std::pow((value - param.GetMin()) / (param.GetMax() - param.GetMin()), 1.0 / mShape);
  };

  double mShape;
};

// Struct object containing possible parameters properties
struct SRParamProperties {
  const int paramIdx = -1;
  const int ctrlTag = -1;
  const char* name = ""; // Name of parameter displayed in host
  const char* label = ""; // Short name for GUI display
  const double defaultVal = 0.0; // Plugin loads in this state, return by double click
  const double minVal = 0.0; // Minimum value of parameter
  const double maxVal = 1.0; // Maximum value of parameter
  const double stepValue = 0.001; // Controls dial steps / accuracy
  const double centerVal = 0.5; // Value that void setShapeCentered() will center (knobs middle position)
  const double centerPoint = 0.5; // Value WHERE void setShapeCentered() will center centerVal (0.5 means: real middle position)
  const char* unit = ""; // Measuring unit of parameter
  const char* group = ""; // Parameter group, not supported by every host
  const IParam::EParamType Type = IParam::EParamType::kTypeNone; // Data type of parameter
  const IParam::EFlags Flags = IParam::EFlags::kFlagsNone;
  const int Knobs = 0; // Used control bitmap
  const struct Position
  {
    const int AttachToControlPanel = 0; // Panel where control should appear
    const int x = 0; // Controls horizontal position within panel
    const int y = 0; // Controls vertical position within panel
    const float w = 2.f; // Controls horizontal position within panel
    const float h = 2.f; // Controls vertical position within panel
  } position;
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
const double stQ = sqrt(0.5);

// Setting the Q constants for higher order low- and highpass filter.
// [Orders 2, 3, 4, 6, 8, 10, 12, 20][Filter]
const double cascadedFilterQs[EFilterSlope::kNumOrders][10] = {
  {0.}, // dummy for order 1
  {sqrt(0.5)}, // order 2
  {1.}, // order 3
  {0.54119610, 1.3065630}, // order 4
  {0.51763809, 0.70710678, 1.9318517}, // order 6
  {0.50979558, 0.60134489, 0.89997622, 2.5629154}, // order 8
  {0.50623256, 0.56116312, 0.70710678, 1.1013446, 3.1962266}, // order 10
  {0.50431448, 0.54119610, 0.63023621, 0.82133982, 1.3065630, 3.8306488}, // order 12
  {0.50154610, 0.51420760, 0.54119610, 0.58641385, 0.65754350, 0.76988452, 0.95694043, 1.3065630, 2.1418288, 6.3727474} // order 20
};

//const double mEqPassQ_O2_F1 = sqrt(0.5);
//const double stQ = mEqPassQ_O2_F1; // standard Q value
//const double mEqPassQ_O3_F1 = 1.;
//const double mEqPassQ_O4_F1 = 0.54119610;
//const double mEqPassQ_O4_F2 = 1.3065630;
//const double mEqPassQ_O6_F1 = 0.51763809;
//const double mEqPassQ_O6_F2 = 0.70710678;
//const double mEqPassQ_O6_F3 = 1.9318517;
//const double mEqPassQ_O8_F1 = 0.50979558;
//const double mEqPassQ_O8_F2 = 0.60134489;
//const double mEqPassQ_O8_F3 = 0.89997622;
//const double mEqPassQ_O8_F4 = 2.5629154;
//const double mEqPassQ_O10_F1 = 0.50623256;
//const double mEqPassQ_O10_F2 = 0.56116312;
//const double mEqPassQ_O10_F3 = 0.70710678;
//const double mEqPassQ_O10_F4 = 1.1013446;
//const double mEqPassQ_O10_F5 = 3.1962266;
//const double mEqPassQ_O12_F1 = 0.50431448;
//const double mEqPassQ_O12_F2 = 0.54119610;
//const double mEqPassQ_O12_F3 = 0.63023621;
//const double mEqPassQ_O12_F4 = 0.82133982;
//const double mEqPassQ_O12_F5 = 1.3065630;
//const double mEqPassQ_O12_F6 = 3.8306488;
//const double mEqPassQ_O20_F1 = 0.50154610;
//const double mEqPassQ_O20_F2 = 0.51420760;
//const double mEqPassQ_O20_F3 = 0.54119610;
//const double mEqPassQ_O20_F4 = 0.58641385;
//const double mEqPassQ_O20_F5 = 0.65754350;
//const double mEqPassQ_O20_F6 = 0.76988452;
//const double mEqPassQ_O20_F7 = 0.95694043;
//const double mEqPassQ_O20_F8 = 1.3065630;
//const double mEqPassQ_O20_F9 = 2.1418288;
//const double mEqPassQ_O20_F10 = 6.3727474;


// UI ELEMENTS
// -----------

// All possible controls listed here
enum EControlImages {
  kKnobNone = 0,
  SslBlue,
  SslGreen,
  SslRed,
  SslOrange,
  SslYellow,
  SslBlack,
  SslWhite,
  AbbeyChicken,
  Button,
  Fader,
  kNumKnobs
};

// Plugin constants
const struct {
  const IColor colorPanelBG = IColor(255, 37, 53, 69);
  const IColor colorPluginBG = IColor(255, 13, 18, 23);
  const IColor colorKnobSslBlue = IColor(255, 62, 100, 121);
  const IColor colorKnobSslRed = IColor(255, 131, 18, 18);
  const IColor colorKnobSslGreen = IColor(255, 103, 141, 52);
  const IColor colorKnobSslOrange = IColor(255, 234, 158, 19);
  const IColor colorKnobSslYellow = IColor(255, 219, 181, 30);
  const IColor colorKnobSslBlack = IColor(255, 23, 23, 23);
  const IColor colorKnobSslWhite = IColor(255, 243, 243, 243);
  const IColor colorDefaultText = COLOR_LIGHT_GRAY;
  const IVColorSpec colorSpec = {
    DEFAULT_BGCOLOR,            // Background (DEFAULT_BGCOLOR = COLOR_TRANSPARENT(0, 0, 0, 0))
    DEFAULT_FGCOLOR,            // Foreground (DEFAULT_FGCOLOR = COLOR_MID_GRAY(255, 200, 200, 200))
    IColor(255, 200, 200, 200), // Pressed    (DEFAULT_PRCOLOR = COLOR_LIGHT_GRAY(255, 240, 240, 240))
    IColor(255, 70, 70, 70),    // Frame      (DEFAULT_FRCOLOR = COLOR_DARK_GRAY(255, 70, 70, 70))
    IColor(30, 255, 255, 255),  // Higlight   (DEFAULT_HLCOLOR = COLOR_TRANSLUCENT(10, 0, 0, 0))
    IColor(100, 0, 0, 0),       // Shadow     (DEFAULT_SHCOLOR = IColor(60, 0, 0, 0)
    IColor(255, 249, 206, 34),  // Extra 1    (DEFAULT_X1COLOR = COLOR_RED(255, 255, 0, 0))
    IColor(255, 48, 166, 186),  // Extra 2    (DEFAULT_X2COLOR = COLOR_GREEN(255, 0, 255, 0))
    IColor(255, 249, 206, 34),  // Extra 3    (DEFAULT_X3COLOR = COLOR_BLUE(255, 0, 0, 255))
  };
  const int textSize = 14;
  const IText textKnobLabel = IText(textSize, colorDefaultText, nullptr, IText::kStyleBold, IText::kAlignCenter, IText::kVAlignBottom, 0, IText::kQualityClearType, colorPanelBG, colorDefaultText);
  const IText textKnobValue = IText(textSize, colorDefaultText, nullptr, IText::kStyleNormal, IText::kAlignCenter, IText::kVAlignBottom, 0, IText::kQualityClearType, colorPanelBG, colorDefaultText);
  const IText textVersionString = IText(20, colorDefaultText, nullptr, IText::kStyleNormal, IText::kAlignNear, IText::kVAlignMiddle, 0, IText::kQualityClearType, colorPanelBG, colorDefaultText);
  const IText textPresetMenu = IText(30, colorDefaultText, nullptr, IText::kStyleNormal, IText::kAlignCenter, IText::kVAlignMiddle, 0, IText::kQualityClearType, colorPanelBG, colorDefaultText);
} SRLayout;

