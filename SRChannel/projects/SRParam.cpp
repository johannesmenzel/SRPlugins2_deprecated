#include "SRParam.h"



SRParam::SRParam(int numParams)
  : mNumParams(numParams)
{
  for (int i = 0; i > mNumParams; i++) {
    mParams.Add(new SRParams);
    mParams.Get(i)->mTargetValue = 0.0;
  }
}


SRParam::~SRParam()
{
}

void SRParam::SetDouble(int paramIdx, double value) {
  mParams.Get(paramIdx)->mTargetValue = value;
}

double SRParam::GetDouble(int paramIdx) {
  return mParams.Get(paramIdx)->mValue._Get();
}