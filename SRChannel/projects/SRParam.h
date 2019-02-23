#pragma once
#include "IPlug_include_in_plug_hdr.h"
#include <variant>

using Values = std::variant<double, float, int, bool>;

class SRParam
{
public:
  SRParam(int numParams);
  ~SRParam();
  void SetDouble(int paramIdx, double value);
  void SetFloat(int paramIdx, float value);
  void SetInt(int paramIdx, int value);
  void SetBool(int paramIdx, bool value);
  double GetDouble(int paramIdx);
  int GetInt(int paramIdx);
  float GetFloat(int paramIdx);
  bool GetBool(int paramIdx);

private:

  struct SRParams
  {

    Values mValue;
    Values mTargetValue;
    bool mSmooth;
    int mNumSmoothSamples;
    bool mIsCurrentlySmoothing;
    SRParams(double value = 0.0, double targetValue = 0.0, bool smooth = false)
      : mValue(value)
      , mTargetValue(targetValue)
      , mSmooth(smooth)
    {
    }
  };
  int mNumParams;
  WDL_PtrList<SRParams> mParams;
};

