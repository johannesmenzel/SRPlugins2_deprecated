#include "SROverdrive.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

SROverdrive::SROverdrive(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPrograms))
  , mSampleRate(44100.0)
  , mDrive(0.0)
  , mDriveEnv(0.0)
  , mLowpassFreq(22000.0)
  , mFilterFreq(22000.0)
  , cMeterBoost(cBoostMeter)
{
  GetParam(kDrive)->InitDouble("Drive", 1.0, 1.0, 500.0, 0.01, "%", 0, "", IParam::ShapeLinear(), IParam::kUnitPercentage);
  GetParam(kGain)->InitDouble("Gain", 0.0, -12.0, 12.0, 0.01, "dB", 0, "", IParam::ShapeLinear(), IParam::kUnitDB);
  GetParam(kTone)->InitDouble("Tone", 0.0, -10.0, 10.0, 0.01, "", 0, "", IParam::ShapeLinear(), IParam::kUnitCustom);
  GetParam(kMix)->InitDouble("Mix", 100.0, 0.0, 100.0, 0.01, "%", 0, "", IParam::ShapeLinear(), IParam::kUnitPercentage);
  GetParam(kLowpassFreq)->InitDouble("Lowpass", 22000., 50., 22000., 1.0, "Hz", 0, "", IParam::ShapePowCurve(5.0), IParam::kUnitFrequency);
  GetParam(kBypass)->InitBool("Bypass", false, "", 0, "", "Off", "On");
  OnReset();

#if IPLUG_EDITOR // All UI methods and member variables should be within an IPLUG_EDITOR guard, should you want distributed UI
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    const IRECT controls = b.GetPadded(-10.f).GetFromTop(200.f);
    const IRECT button = b.GetFromBottom(100.f).GetCentredInside(40.f);
    pGraphics->AttachControl(new IVMeterControl<1, 1024>(b, "Boost"), cBoostMeter);
    pGraphics->AttachControl(new IVKnobControl(controls.GetGridCell(0, 0, 2, 3), kDrive, "DRIVE", true), cDrive);
    pGraphics->AttachControl(new IVKnobControl(controls.GetGridCell(0, 1, 2, 3), kGain, "GAIN", true), cGain);
    pGraphics->AttachControl(new IVKnobControl(controls.GetGridCell(0, 2, 2, 3), kMix, "MIX", true), cMix);
    pGraphics->AttachControl(new IVKnobControl(controls.GetGridCell(1, 0, 2, 3), kLowpassFreq, "LP", true), cLowpassFreq);
    pGraphics->AttachControl(new IVSwitchControl(button, kBypass), cBypass);
  };
#endif
}

#if IPLUG_DSP
void SROverdrive::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  meterValues = new sample*[1];
  meterValues[0] = new sample[nFrames];
  //const double drive = GetParam(kDrive)->Value() / 100.;
  const int nChans = NOutChansConnected();
  fLowpassDriven.SetFreq(std::max(0.01, 0.5 * (1. - 5. * mDriveEnv)));
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      if (!mBypass) {
        outputs[c][s] = inputs[c][s];
        outputs[c][s] = fHighpass.Process(outputs[c][s], c);
        const double sidechain = std::fabs(outputs[c][s]);
        mDriveEnv = fSidechainSmoother[c].Process(10. * sidechain);
        const double momDrive = mDrive + (1. - 5. * mDriveEnv);
        outputs[c][s] = std::tanh(momDrive * outputs[c][s]) / std::tanh(momDrive);
        outputs[c][s] = std::tanh(momDrive * outputs[c][s]) / std::tanh(momDrive);
        outputs[c][s] = std::tanh(momDrive * outputs[c][s]) / std::tanh(momDrive);
        outputs[c][s] = fLowpass.Process(outputs[c][s], c);
        //outputs[c][s] = fLowpassDriven.Process(outputs[c][s], c);
        outputs[c][s] *= mGain;
        outputs[c][s] = mMix * outputs[c][s] + (1. - mMix) * inputs[c][s];
        meterValues[0][s] = mDriveEnv;
      }
      else {
        outputs[c][s] = inputs[c][s];
      }
    }
  }





  cMeterBoost.ProcessBlock(meterValues, nFrames);
  delete[] meterValues[0];
  delete[] meterValues;


}

void SROverdrive::OnReset()
{
  mSampleRate = GetSampleRate();
  for (int c = 0; c < 2; c++) {
    fSidechainSmoother[c].Reset(5000., mSampleRate);
  }
  fLowpassDriven.SetFilter(SR::DSP::SRFilterIIR<sample, 2>::EFilterType::BiquadLowpass, 22000. / mSampleRate, 0.707, 0, mSampleRate);
  fHighpass.SetFilter(SR::DSP::SRFilterIIR<sample, 2>::EFilterType::BiquadHighpass, 120. / mSampleRate, 0.707, 0, mSampleRate);
}

void SROverdrive::OnIdle()
{
  cMeterBoost.TransmitData(*this);
}

void SROverdrive::OnParamChange(int paramIdx)
{
  switch (paramIdx)
  {
  case kDrive:
    mDrive = GetParam(paramIdx)->Value() * 0.01;
    break;
  case kGain:
    mGain = DBToAmp(GetParam(paramIdx)->Value());
    break;
  case kMix:
    mMix = GetParam(paramIdx)->Value() * 0.01;
    break;
  case kBypass:
    mBypass = GetParam(paramIdx)->Bool();
    break;
  case kLowpassFreq:
    mLowpassFreq = GetParam(paramIdx)->Value();
    fLowpass.SetFreq(mLowpassFreq / mSampleRate);
    break;
  default:
    break;
  }
}
#endif
