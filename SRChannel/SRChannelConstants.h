#pragma once
#include "../SRClasses/SRConstants.h"

const int kNumPrograms = 1;

enum EParams
{
  // Input Stage
  kInputGain = 0,
  kSaturationType,
  kInputDrive,
  kSaturationAmount,
  kSaturationHarmonics,
  kSaturationSkew,
  // EQ Stage
  // -- Filter
  kEqHpFreq,
  kEqLpFreq,
  kEqHpOrder,
//  kEqLpOrder,
  kEqHfGain,
  // -- EQ
  kEqHfFreq,
  kEqHfQ,
  kEqHfBell,
  kEqHmfGain,
  kEqHmfFreq,
  kEqHmfQ,
  kEqLmfGain,
  kEqLmfFreq,
  kEqLmfQ,
  kEqLfGain,
  kEqLfFreq,
  kEqLfQ,
  kEqLfBell,
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
  kCompPeakIsFeedback,
  kCompPeakIsExtSc,
  kCompPeakSidechainFilterFreq,
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

enum ECtrlTags {
  // Input Stage
  cInputGain = 0,
  cSaturationType,
  cInputDrive,
  cSaturationAmount,
  cSaturationHarmonics,
  cSaturationSkew,
  // EQ Stage
  // -- Filter
  cEqHpFreq,
  cEqLpFreq,
  cEqHpOrder,
//  cEqLpOrder,
  // -- EQ
  cEqHfGain,
  cEqHfFreq,
  cEqHfQ,
  cEqHfBell,
  cEqHmfGain,
  cEqHmfFreq,
  cEqHmfQ,
  cEqLmfGain,
  cEqLmfFreq,
  cEqLmfQ,
  cEqLfGain,
  cEqLfFreq,
  cEqLfQ,
  cEqLfBell,
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
  cCompPeakIsFeedback,
  cCompPeakIsExtSc,
  cCompPeakSidechainFilterFreq,
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
  cPanelInput,
  cPanelEq,
  cPanelComp,
  cPanelPost,
  cPanelOutput,
  cPanelMeter,
  // Number of Controls
  cNumParams
};



enum EStages {
  stageInput = 0,
  stageEq,
  stageComp,
  stagePost,
  stageOutput,
  stageMeter,
  stageHeader,
  stageFooter
};

// Struct object containing possible parameters properties
struct structParameterProperties {
  const char* name;				// Name of parameter displayed in host
  const char* shortName;			// Short name for GUI display
  const double defaultVal;		// Plugin loads in this state, return by double click
  const double minVal;			// Minimum value of parameter
  const double maxVal;			// Maximum value  of parameter
  const double stepValue;			// Controls dial steps / accuracy
  const double centerVal;			// Value that void setShapeCentered() will center (knobs middle position)
  const double centerPoint = .5;	// Value WHERE void setShapeCentered() will center centerVal (0.5 means: real middle position)
  const char* label;				// Measuring unit of parameter
  const char* group;				// Parameter group, not supported by every host
  const int Type;					// Data type of parameter
  const int Knobs;				// Used control bitmap
  const int Stage;
  const int x;					// Horizontal position of control
  const int y;					// Vertical position of control
  const char* labelMin;			// GUI display string of minimum
  const char* labelMax;			// GUI display string of maximum
  const char* labelCtr;			// GUI display string of middle position
  const char* tooltip;			// Tooltip on mouseover
};

/*
Actual properties struct.
Keep in order of the above listet parameters.
These are constants.
*/
const structParameterProperties parameterProperties[kNumParams] = {
  //	{ "NAME",			"SNAME"	DEF,	MIN,	MAX,	STEP,	CENTER,	CTRPNT,	"LABEL",GROUP ,		TYPE,		KNOB,	Stage, X,		Y, "LBLMIN", "LBLMAX", "LBLCTR", "TOOLTIP"},
  // Input Stage
  { "Input Gain",      "IN",   0.,		-60.,	12.,	0.1,	0.,		10. / 12.,"dB",	"Global",	typeDouble, Fader, stageInput, 0,		1,		"-60", "12", "0",		"Input Gain is applied before everything else" },
  { "Saturation Type", "TPE",  0,		0,		3,		-1,		-1,		-1,		"",		"Input",	typeEnum,	none,	stageInput,	1,		3,		"", "", "",				"Type of Saturation"},
  { "Sat Drive",       "DRV",  0.,		0.,		60.,	0.1,	30.,	.5,		"dB",	"Input",	typeDouble, SslRed,	stageInput,		0,		0,		"0", "60", "30",		"Saturation input drive, hits the saturation module harder" },
  { "Sat Amount",      "AMT",  0.,		0.,		99.,	0.01,	10.,	.5,		"%",	"Input",	typeDouble, SslOrange,stageInput,	1,		0,		"0", "100", "10",		"Amount of Saturation" },
  { "Harmonics",       "HRM",  50.,	0.,		100.,	.01,	50.,	.5,		"%",	"Input",	typeDouble, SslBlue, stageInput,	1,		1,		"Even", "Odd", "Mix",	"Dial in even harmonics by turning the knob counter-clockwise" },
  { "Sat Skew",        "SKW",  0,		-100.,	100.,	0.01,	0.,		.5,		"%",	"Input",	typeDouble, SslOrange, stageInput,	1,		2,		"-100", "100", "0",		"Saturations positive/negative skewness. Distorts waveform" },
  // EQ Stage
  // -- Filter
  { "Highpass Freq",   "HP",   16.,	16.,	350.,	1.,		120.,	.5,		"Hz",	"Filter",		typeDouble, SslWhite,	stageEq, 0,		0,		"16", "350", "120",		"Frequency of the High Pass Filter, turn down to deactivate" },
  { "Lowpass Freq",    "LP",   22000., 3000.,	22000.,	1.,		5000.,	.5,		"Hz",	"Filter",		typeDouble, SslWhite,	stageEq, 1,		0,		"3k", "22k", "5k",		"Set the frequency of the Low Pass Filter, turn up to deactivate" },
  { "HP Order",        "HPO",  1,		0,		8,		1,		5,		.5,		"dB/oct", "Filter",		typeEnum,	none, stageEq, 0,		0,		"6", "120", "36",		"Order of the Highpass Filter or filter slope" },
  //{ "LP Order",        "LPO",  1,		0,		8,		1,		5,		.5,		"dB/oct", "Filter",		typeEnum,	none,	stageEq,	1,		1,		"6", "120", "36",		"HIDED - Order of the Lowpass Filter or filter slope" },
  // -- EQ
  { "HF Gain",         "HF",   0.,		-12.,	12.,	.1,		0.,		.5,		"dB",	"EQ",		typeDouble, SslRed, stageEq, 		0,		1,		"-12", "12", "0",		"Gain of the high frequency band" },
  { "HF Freq",         "FQ",   8000.,	1500.,	16000.,	1.,		8000.,	.5,		"Hz",	"EQ",		typeDouble, SslRed, stageEq, 		1,		2,		"1.5k", "16k", "8k",	"Frequency of the high frequency band" },
  { "HF Q",            "Q",    stQ,	0.1,	10.,	0.01,	stQ,	.5,		"",		"EQ",		typeDouble, SslRed, stageEq, 		8,		2,		"W", "N", "",			"Set the Q value of the high frequency band" },
  { "Hf Bell",         "Bell", 0,		0,		1,		1,		0.5,	.5,		"",		"EQ",		typeBool,	Button, stageEq, 		1,		1,		"SLF", "BLL", "",		"Switches the high frequency band between bell and shelf" },
  { "HMF Gain",        "HMF",  0.,		-12.,	12.,	0.1,	0.,		.5,		"dB",	"EQ",		typeDouble, SslBlue, stageEq, 	0,		2,		"-12", "12", "0",		"Gain of the upper mid frequency band" },
  { "HMF Freq",        "FQ",   3000.,	600.,	7000.,	1.,		3000.,	.5,		"Hz",	"EQ",		typeDouble, SslBlue, stageEq, 	1,		3,		"600", "7k", "3k",		"Frequency of the upper mid frequency band" },
  { "HMF Q",           "Q",    stQ,	0.1,	10.,	0.01,	stQ,	.5,		"",		"EQ",		typeDouble, SslBlue, stageEq, 	0,		3,		"W", "N", "",			"Set the Q value of the upper mid frequency band" },
  { "LMF Gain",        "LMF",	0.,		-12.,	12.,	0.1,	0.,		.5,		"dB",	"EQ",		typeDouble, SslGreen, stageEq, 	0,		4,		"-12", "12", "0",		"Gain of the lower mid frequency band" },
  { "LMF Freq",        "FQ",	1000.,	200.,	2500.,	1.,		1000.,	.5,		"Hz",	"EQ",		typeDouble, SslGreen, stageEq, 	1,		5,		"200", "2.5k", "1k",	"Frequency of the lower mid frequency band" },
  { "LMF Q",           "Q",	stQ,	0.1,	10.,	0.01,	stQ,	.5,		"",		"EQ",		typeDouble, SslGreen, stageEq, 	0,		5,		"W", "N", "",			"Set the Q value of the lower mid frequency band" },
  { "LF Gain",         "LF",	0.,		-12.,	12.,	0.1,	0.,		.5,		"dB",	"EQ",		typeDouble, SslBlack, stageEq, 	0,		6,		"-12", "12", "0",		"Gain of the low frequency band" },
  { "LF Freq",         "FQ",	200.,	30.,	450.,	1.,		200.,	.5,		"Hz",	"EQ",		typeDouble, SslBlack, stageEq, 	1,		6,		"30", "450", "200",		"Set the frequency of the low frequency band" },
  { "LF Q",            "Q",	stQ,	0.1,	10.,	0.01,	stQ,	.5,		"",		"EQ",		typeDouble, SslBlack, stageEq, 	8,		6,		"W", "N", "",			"Set the Q value of the low frequency band" },
  { "Lf Bell",         "Bell",	0,		0,		1,		0.1,	0.5,	.5,		"",		"EQ",		typeBool,	Button, stageEq, 		1,		7,		"SLF", "BLL", "",		"Switches the low frequency band between bell and shelf" },
  // RMS Compressor
  { "RMS Thresh",      "THR",	0.,		-40.,	0.,		0.1,	-20.,	.5,		"dB",	"Compressor",typeDouble, SslWhite, stageComp, 	0,		0,		"-40", "0", "-20",		"Threshold of RMS Compressor" },
  { "RMS Ratio",       "RAT",	3.,		0.5,	20.,	0.01,	3.,		.5,		":1",	"Compressor",typeDouble, SslWhite, stageComp, 	1,		0,		".5:1", "20:1", "3:1",	"Ratio of RMS Compressor" },
  { "RMS Attack",      "ATT",	20.,	10.,	200.,	0.01,	20.,	.5,		"ms",	"Compressor",typeDouble, SslWhite, stageComp, 	0,		1,		"10", "200", "20",		"Attack of RMS Compressor" },
  { "RMS Release",     "REL",	200.,	100.,	5000.,	1.,		200.,	.5,		"ms",	"Compressor",typeDouble, SslWhite, stageComp, 	1,		1,		"100", "5k", "200",		"Release of RMS Compressor" },
  { "RMS Knee",        "SKN",	0.,		0.,		30.,	0.1,	10.,	.5,		"dB",	"Compressor",typeDouble, SslGreen,	stageComp, 	1,		2,		"0", "30", "10",		"RMS Compressors width of the soft knee" },
  { "RMS Makeup",      "MK",	0.,		0.,		40.,	0.1,	10.,	.5,		"dB",	"Compressor",typeDouble, SslBlack, stageComp, 	0,		2,		"0", "40", "10",		"Makeup gain of RMS Compressor" },
  { "RMS Topology",    "TOP",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Compressor",typeBool,	Button,		stageComp, 0,		6 + 32,	"FWD", "BCK", "",		"RMS Compressors Topology, either feed forward or feedback" },
  { "RMS Ext SC",      "Ext",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Compressor",typeBool,	Button,stageComp,		0,		3,		"INT", "EXT", "",		"External sidechain for RMS Compressor. Use tracks input channel 3/4" },
  // Peak Compressor
  { "Peak Thresh",     "THR",	0.,		-40.,	0.,		0.1,	-20.,	.5,		"dB",	"Compressor",typeDouble, SslWhite, stageComp, 	0,		4,		"-40", "0", "-20",		"Threshold of Peak Compressor" },
  { "Peak Ratio",      "RAT",	3.,		0.5,	20.,	0.01,	3.,		.5,		":1",	"Compressor",typeDouble, SslWhite, stageComp, 	1,		4,		".5:1", "20:1", "3:1",	"Ratio of Peak Compressor" },
  { "Peak Attack",     "ATT",	20.,	0.01,	200.,	0.01,	20.,	.5,		"ms",	"Compressor",typeDouble, SslWhite, stageComp, 	0,		5,		"0.01", "200", "20",	"Attack of Peak Compressor" },
  { "Peak Release",    "REL",	200.,	1.,		2000.,	0.01,	200.,	.5,		"ms",	"Compressor",typeDouble, SslWhite, stageComp, 	1,		5,		"1", "2k", "200",		"Release of Peak Compressor" },
  { "Peak Knee",       "SKN",	0.,		0.,		30.,	0.1,	10.,	.5,		"dB",	"Compressor",typeDouble, SslGreen,	stageComp, 1,		6,		"0", "30", "10",		"Peak Compressors width of the soft knee" },
  { "Peak Makeup",     "MK",	0.,		0.,		40.,	0.1,	10.,	.5,		"dB",	"Compressor",typeDouble, SslBlack, stageComp, 	0,		6,		"0", "40", "10",		"Makeup Gain of Peak Compressor" },
  { "Peak Topology",   "TOP",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Compressor",typeBool,	Button, stageComp, 		1,		15 + 32,	"FWD", "BCK", "",		"Peak Compressors Topology, either feed forward or feedback" },
  { "Peak Ext SC",     "Ext",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Compressor",typeBool,	Button, stageComp, 1,		7,		"INT", "EXT", "",		"External sidechain for Peak Compressor. Use tracks input channel 3/4" },
  { "Peak SC Filter",  "SCF",	16.,	16.,	5000.,	1.,		1000.,	.5,		"Hz",	"Compressor",typeDouble, SslBlue, stageComp,	0,		7,		"16", "5k", "1k",		"Frequency of the Compressors sidechain highpass filter" },
  // Compressor
  { "Comp Ser/Par",    "Par",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Compressor",typeBool,	Button,	stageComp,	9,		8,		"SER", "PAR", "",		"RMS and Peak Compressor can be run serial or parallel" },
  { "RMS/Peak Ratio",  "CR",	50.,	0.,		100.,	0.1,	50.,	.5,		"%",	"Compressor",typeDouble, SslBlack, stageComp, 	0,		8,		"0", "100", "50",		"Mix the signal of the RMS and Peak Compressor" },
  { "Dry/Wet",         "MIX",	100.,	0.,		100.,	1.,		50.,	.5,		"%",	"Compressor",typeDouble, SslBlack, stageComp, 	1,		8,		"0", "100", "50",		"Do parallel compression by dialing in the uncompressed signal" },
  // Deesser
  { "Deesser Freq",    "DSF",	7000.,	16.,	22000.,	1.,		7000.,	.5,		"Hz",	"Deesser",	typeDouble, SslYellow, stagePost, 0,		0,		"16", "22k", "7k",		"Deessers center frequency" },
  { "Deesser Q",       "DSQ",	stQ,	0.1,	10.,	0.01,	stQ,	.5,		"",		"Deesser",	typeDouble, SslYellow,	stagePost, 0,		1,		"W", "N", "",			"Deessers width (Q)" },
  { "Deesser Thresh",  "DST",	0.,		-100.,	0.,		0.1,	-50.,	.5,		"dB",	"Deesser",	typeDouble, SslYellow, stagePost, 	0,		2,		"-100", "0", "-50",		"Deessers Threshold" },
  { "Deesser Ratio",   "DSR",	3.,		0.5,	20.,	0.01,	3.,		.5,		":1",	"Deesser",	typeDouble, SslYellow,	stagePost, 0,		3,		".5:1", "20:1", "3:1",	"Deessers Ratio" },
  { "Deesser Attack",  "DSA",	20.,	0.01,	200.,	0.01,	20.,	.5,		"ms",	"Deesser",	typeDouble, SslYellow,	stagePost, 0,		4,		"0.01", "200", "20",	"Deessers Attack" },
  { "Deesser Release", "DSD",	200.,	1.,		2000.,	0.01,	200.,	.5,		"ms",	"Deesser",	typeDouble, SslYellow,	stagePost, 0,		5,		"1", "2k", "200",		"Deessers Release" },
  { "Deesser Makeup",  "DSM",	0.,		0.,		40.,	0.1,	10.,	.5,		"dB",	"Deesser",	typeDouble, SslYellow,	stagePost, 0,		6,		"0", "40", "10",		"Deessers Makeup" },
  // Output Stage
  { "Pan",             "PAN",	0.,		-100.,	100.,	1.,		0.,		.5,		"%",	"Output",	typeDouble, SslBlue, stageOutput,	0,		0,		"L", "R", "C",			"Pan" },
  { "Pan Freq",        "PNF",	150.,	20.,	1000.,	1.,		150.,	.5,		"Hz",	"Output",	typeDouble, SslRed, stageOutput,		0,		1,		"20", "1k", "200",		"Frequencies below crossover will be not affected by panner" },
  { "Mono Bass",       "MNB",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Output",	typeBool,	Button,	stageOutput,	0,		3,		"STB", "MNB", "",		"Bass Frequencies will left stereo or will be monoed" },
  { "Limiter",         "LMT",	10.,	-30.,	10.,	0.1,	0.,		.5,		"dB",	"Output",	typeDouble, SslOrange, stageOutput,	0,		2,		"-30", "10", "0",		"Thresold of Output Limiter" },
  { "Clipper",         "CLP",	0.,		0.,		99.,	0.01,	50.,	.5,		"%",	"Output",	typeDouble, SslYellow, stageOutput,	1,		0,		"0", "100", "50",		"Amount of the Output Clipper (cuts amplitutde)" },
  { "Auto Gain Comp",  "AGC",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Output",	typeBool,	Button,	stageOutput,	0,		4,		"AGC", "", "",			"Automatic Gain Conpensation. Click to adjust output volume to input volume" },
  { "Output Gain",     "OUT",	0.,		-60.,	12.,	0.1,	0.,		10. / 12.,"dB",	"Global",	typeDouble, Fader, stageOutput,		1,		1,		"-60", "12", "0",		"Output Gain" },
  // Bypasses
  { "Input Bypass",    "In Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button, stageInput, 		3 - 32,	0 - 36,	"ACT", "BYP", "",		"Bypass Input Section" },
  { "EQ Bypass",       "EQ Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button, stageEq,		7 - 32,	0 - 36,	"ACT", "BYP", "",		"Bypass Equalizer Section" },
  { "Comp Bypass",     "Comp Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button, stageComp,		13 - 32,	0 - 36,	"ACT", "BYP", "",		"Bypass Compressor Section" },
  { "Output Bypass",   "Out Byp",	0,	0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button, stageOutput,		17 - 32,	0 - 36,	"ACT", "BYP", "",		"Bypass Output Section" },
  { "Bypass",          "Byp",	0,		0,		1,		0.1,	0.5,	.5,		"",		"Global",	typeBool,	Button,	stageFooter,	4,								4,								"ACT", "BYP", "",		"Bypass Plugin" },
  // Test Parameters
  //{ "TestParam 1",     "T1",	0.2,	0.2,	5.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,	stageFooter,	0,		0,								"0", "1", "0.5",		"HIDED - Generic control for development tests" },
  //{ "TestParam 2",     "T2",	0.2,	0.2,	5.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed, stageFooter, 1,		0,								"0", "1", "0.5",		"HIDED - Generic control for development tests" },
  //{ "TestParam 3",     "T3",	0.,		0.,		1.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,	stageFooter,	2,		0,								"0", "1", "0.5",		"HIDED - Generic control for development tests" },
  //{ "TestParam 4",     "T4",	0.,		0.,		1.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,	stageFooter,		3,		0,								"0", "1", "0.5",		"HIDED - Generic control for development tests" },
  //{ "TestParam 5",     "T5",	0.,		0.,		1.,		.0001,	.5,		.5,		"",		"Test",		typeDouble, SslRed,		stageFooter,	4,		0,								"0", "1", "0.5",		"HIDED - Generic control for development tests" }
  //	{ "NAME",		"SNAME"	DEF,	MIN,	MAX,	STEP,	CENTER,	CTRPNT,	"LABEL",GROUP ,		TYPE,		KNOB,		X,		Y,		"LBLMIN", "LBLMAX", "LBLCTR", "TOOLTIP"},
};

