#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "AWClasses/AWConsole6.h"
#include "Utils/SRHelpers.h"
#include "Graphics/SRControls.h"
#if defined VST3_API
#define MAXNINCHANNELS 16
#else
#define MAXNINCHANNELS 64
#endif
#define MAXNOUTCHANNELS 2


const int kNumPrograms = 1;

enum EParams
{
  kIsBypass = 0,
  kDrive,
  kConsoleType,
  kNumParams
};

enum ECtrlTags
{
  cIsBypass = 0,
  cDrive,
  cConsoleType,
  // ---------
  cInputMeter,
  cOutputMeter,
  kNumCtrlTags
};

class SRSummer : public IPlug
{
public:
  SRSummer(IPlugInstanceInfo instanceInfo);

#if IPLUG_DSP // All DSP methods and member variables should be within an IPLUG_DSP guard, should you want distributed UI
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;
  void SetConsoleType(int type);
  void OnParamChange(int paramIdx) override;
  void OnIdle() override;

private:
  double mSamplerate;
  double mDrive;
  bool mIsBypass;
  int mConsoleType;
  AW::AWNoiseShaper fNoiseShaper;
  AW::AWDither<MAXNOUTCHANNELS> fDither;
  AW::AWConsoleChannel<MAXNINCHANNELS> fChannels;
  AW::AWConsoleBuss<MAXNOUTCHANNELS> fBus;
  SR::Graphics::SRMeter<MAXNINCHANNELS>::SRMeterBallistics mInputMeterBallistics { cInputMeter };
  SR::Graphics::SRMeter<MAXNOUTCHANNELS>::SRMeterBallistics mOutputMeterBallistics { cOutputMeter };
#endif
};
