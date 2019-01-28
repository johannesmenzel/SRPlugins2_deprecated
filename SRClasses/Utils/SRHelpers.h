#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <string>

namespace SR {
  namespace Utils {

    // CONVERSION
    // ----------

    // Decibel - Linear conversion
    static inline double DBToAmp(double dB) { return exp(0.11512925464970 * dB); }
    static inline double AmpToDB(double amp) { return 8.685889638065036553 * log(fabs(amp)); }

    // double to char conversion
    // DOESNT WORK
    inline char* setCharFromDouble(double doubleValue) {
      std::string stringValue = std::to_string(doubleValue);
      char *charValue = new char[stringValue.length() + 1];
      return strcpy(charValue, stringValue.c_str());
    }

    inline double SetShapeCentered(double cMinValue, double cMaxValue, double cCenteredValue, double cControlPosition) {
      return log((cCenteredValue - cMinValue) / (cMaxValue - cMinValue)) / log(cControlPosition);
    }

    // DSP
    // ---

    // Compressor auto makeup calculation
    // SHOULD BE MOVED TO SRDYNAMICS CLASS
    inline double calcAutoMakeup(double threshDb, double ratio, double referenceDb, double attackMs, double releaseMs) {
      return	1. + (1. / (DBToAmp(((ratio - 1.) * -threshDb) / 2)) - 1.) * sqrt(threshDb / referenceDb) * (sqrt(30.) / sqrt(attackMs)) * (sqrt(releaseMs) / sqrt(5000.));
    }

    // tanh
    inline double fast_tanh(double x) {
      x = exp(x + x);
      return (x - 1) / (x + 1);
    }

  }
}
// end namespaces