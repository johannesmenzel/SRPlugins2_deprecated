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


// DSP ELEMENTS
// ------------

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


// UI ELEMENTS
// -----------

// Plugin constants
const struct {
  const IColor colorPanelBG;
  const IColor colorPluginBG;
  const IColor colorFG;
  const IColor colorHL;
  const IColor colorX1;
  const IColor colorKnobSslBlue;
  const IColor colorKnobSslRed;
  const IColor colorKnobSslGreen;
  const IColor colorKnobSslOrange;
  const IColor colorKnobSslYellow;
  const IColor colorKnobSslBlack;
  const IColor colorKnobSslWhite;
  const IText tKnobLabel;
  const IText tKnobValue;
} pluginLayout = {
  IColor(255, 37, 53, 69),
  IColor(255, 13, 18, 23),
  IColor(255, 48, 166, 186),
  IColor(30, 255, 255, 255),
  IColor(255, 249, 206, 34),
  //Knob Colors
  IColor(255, 62, 100, 121),
  IColor(255, 131, 18, 18),
  IColor(255, 103, 141, 52),
  IColor(255, 234, 158, 19),
  IColor(255, 219, 181, 30),
  IColor(255, 23, 23, 23),
  IColor(255, 243, 243, 243),
  // Texts
  IText(DEFAULT_TEXT_SIZE, COLOR_WHITE, DEFAULT_FONT, IText::kStyleBold, IText::kAlignCenter, IText::kVAlignTop, 0, IText::kQualityNonAntiAliased),
  IText(DEFAULT_TEXT_SIZE, COLOR_WHITE, DEFAULT_FONT, IText::kStyleNormal, IText::kAlignCenter, IText::kVAlignBottom, 0, IText::kQualityNonAntiAliased)
};

const IVColorSpec SR_SPEC = {
  DEFAULT_BGCOLOR, // Background
  pluginLayout.colorFG, // Foreground DEFAULT_FGCOLOR
  DEFAULT_PRCOLOR, // Pressed
  DEFAULT_FRCOLOR, // Frame 
  pluginLayout.colorHL, // Higlight
  DEFAULT_SHCOLOR, // Shadow
  pluginLayout.colorX1, // Extra 1
  DEFAULT_X2COLOR, // Extra 2
  DEFAULT_X3COLOR // Extra 3
};

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