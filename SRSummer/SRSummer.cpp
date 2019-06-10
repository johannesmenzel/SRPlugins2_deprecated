#include "SRSummer.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

SRSummer::SRSummer(IPlugInstanceInfo instanceInfo)
  : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
  , mSamplerate(44100.)
  , mIsBypass(false)
  , mDrive(1.0)
  , mConsoleType(0)
{
  GetParam(kDrive)->InitDouble("Drive", 0., -50., 50., 0.01, "dB");
  GetParam(kIsBypass)->InitBool("Bypass", false, "", 0, "", "Active", "Bypassed");
  GetParam(kConsoleType)->InitEnum("Type", 0, 2, "", 0, "", "Console 6", "Console 5");

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(kUIResizerScale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new IVKnobControl(b.GetGridCell(0, 0, 2, 2), kDrive, "Drive", true), cDrive);
    pGraphics->AttachControl(new IVSwitchControl(b.GetGridCell(0, 1, 2, 2).SubRectVertical(2,0), kIsBypass), cIsBypass);
    pGraphics->AttachControl(new IVSwitchControl(b.GetGridCell(0, 1, 2, 2).SubRectVertical(2,1), kConsoleType), cConsoleType);
    pGraphics->AttachControl(new SR::Graphics::SRMeter<MAXNINCHANNELS>(b.GetGridCell(1, 0, 2, 1).FracRectHorizontal(.9f, false), false, false, -90.f, 0.f, SR::Utils::SetShapeCentered(-90., 0., -45., 0.5), 3, 6), cInputMeter);
    pGraphics->AttachControl(new SR::Graphics::SRMeter<MAXNOUTCHANNELS>(b.GetGridCell(1, 0, 2, 1).FracRectHorizontal(.1f, true), false, false, -90.f, 0.f, SR::Utils::SetShapeCentered(-90., 0., -45., 0.5), 3, 6), cOutputMeter);
  };
#endif
}

#if IPLUG_DSP
void SRSummer::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const int nInChannels = std::min(NInChansConnected(), MAXNINCHANNELS);
  const int nOutChannels = std::min(NOutChansConnected(), MAXNOUTCHANNELS);
  if (!mIsBypass) {
    fNoiseShaper.ProcessBlock(inputs, inputs, nInChannels, nFrames);
    fChannels.ProcessBlock(inputs, inputs, nInChannels, nFrames);
  }
  mInputMeterBallistics.ProcessBlock(inputs, nFrames);
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nOutChannels; c++) {
      outputs[c][s] = 0.0;
    }
    for (int c = 0; c < nInChannels; c++) {
      outputs[c % MAXNOUTCHANNELS][s] += inputs[c][s];
    }
  }
  if (!mIsBypass) {
    fBus.ProcessBlock(outputs, outputs, nOutChannels, nFrames);
    fDither.ProcessBlock(outputs, outputs, nOutChannels, nFrames);
  }
  mOutputMeterBallistics.ProcessBlock(outputs, nFrames);
}

void SRSummer::OnReset()
{
  mSamplerate = GetSampleRate();
  SetConsoleType(mConsoleType);
  fChannels.Reset(mSamplerate);
  fBus.Reset(mSamplerate);
  fDither.Reset();
}

void SRSummer::SetConsoleType(int type) {
  switch (type)
  {
  case 0:
    fChannels.SetType(AW::AWConsoleChannel<MAXNINCHANNELS>::EConsoleType::kConsole6);
    fBus.SetType(AW::AWConsoleBuss<MAXNOUTCHANNELS>::EConsoleType::kConsole6);
    break;
  case 1:
    fChannels.SetType(AW::AWConsoleChannel<MAXNINCHANNELS>::EConsoleType::kConsole5);
    fBus.SetType(AW::AWConsoleBuss<MAXNOUTCHANNELS>::EConsoleType::kConsole5);
    break;
  default:
    break;
  }
}

void SRSummer::OnParamChange(int paramIdx)
{
  switch (paramIdx)
  {
  case kDrive:
    mDrive = SR::Utils::DBToAmp(GetParam(paramIdx)->Value());
    fChannels.SetInput(mDrive);
    fBus.SetInput(1. / mDrive);
    break;
  case kIsBypass:
    mIsBypass = GetParam(paramIdx)->Bool();
    break;
  case kConsoleType:
    mConsoleType = GetParam(paramIdx)->Int();
    SetConsoleType(mConsoleType);
    break;
  default:
    break;
  }
}

void SRSummer::OnIdle()
{
  mInputMeterBallistics.TransmitData(*this);
  mOutputMeterBallistics.TransmitData(*this);
}

#endif
