//  SRSaturation.h

// Implementation:
//   Note that you might need a class for each channel
//   (so 2 for stereo processing.)
//
// Header:
//   private:
//     Impementation as object:
//       SRSaturation name;
//     Implementation as pointer:
//       SRSaturation *name = new SRSaturation();
//
// Class: Constructor, Reset()
//     Implementation as object:
//       name.setClass(pType, pVar1, pVar2, pVar3); 
//     Implementation as pointer:
//       name->setClass(pType, pVar1, pVar2, pVar3);
//
// Class: ProcessDoubleReplacing()
//   Per sample and channel:
//     Implementation as object:
//       *out1 = name.process(*in1);
//     Implementation as pointer:
//       *out1 = name->process(*in1);

#pragma once

#include "../Utils/SRHelpers.h"
#include <functional>
//#include "Oversampler.h"
// #include this and that


namespace SR {
	namespace DSP {



		class SRSaturation {
		public:
      // If type definitions of type int needed:
      enum SaturationTypes {
        typeMusicDSP = 0,
        typeZoelzer,
        typePirkle,
        typePirkleModified,
        typeRectHalf,
        typeRectFull,
        numTypes
        // ...
      };

			// constructor
			SRSaturation();
			// class initializer
			SRSaturation(
				int pType,
				double pDriveDb,
				double pAmountNormalized,
				double pHarmonicsNormalized,
				bool pPositiveSide,
				double pSkewNormalized,
				double pWet,
        double pSamplerate
			);
			// destructor
			~SRSaturation(); // destructor

			// public functions that need to be accessed from outside
			void setType(int pType);
			void setDrive(double pDriveDb);
			void setAmount(double pAmountNormalized);
			void setHarmonics(double pHarmonicsNormalized);
			void setPositive(bool pPositive);
			void setSkew(double pSkewNormalized);
			void setWet(double pWetNormalized);
      void setSamplerate(double mSamplerate);

			void setSaturation(
				int pType,
				double pDriveDb,
				double pAmountNormalized,
				double pHarmonicsNormalized,
				bool pPositive,
				double pSkewNormalized,
				double pWetNormalized,
        double pSamplerate
			);
			// inline process function, if needed
      double process(double in);



		protected:
			// Protected functions that do internal calculations and that are called from other funcions
      double processMusicDSP(double in);
      double processZoelzer(double in);
      double processPirkle(double in);
      double processPirkleModified(double in);
      double processRectHalf(double in);
      double processRectFull(double in);
			void calcSaturation(void);

			// Internal member and internal variables
			int mType;
			// member variables
			double mDriveNormalized;
			double mAmountNormalized;
			double mAmount;
			double mHarmonicsNormalized;
			bool mPositive; // if aiming for even harmonics, the positive side of the envelope will be affected if true, otherwise the negative side
			double mSkewNormalized;
			double mWetNormalized;
      double mSamplerate;
      //OverSampler<double> mOversampler;
      // internal variables
			double prev;
			double dry;


		}; // end of class

		inline double SRSaturation::process(double in) {

			// apply drive
			in *= mDriveNormalized;

			// create driven dry samples
			dry = in;

			// call specific inline functions
			switch (this->mType) {
			case typeMusicDSP: in = processMusicDSP(in); break;
			case typePirkle: in = processPirkle(in); break;
			case typeZoelzer: in = processZoelzer(in); break;
			case typePirkleModified: in = processPirkleModified(in); break;
			case typeRectHalf: in = processRectHalf(in); break;
			case typeRectFull: in = processRectFull(in); break;
			default: break;
			}

			if (!mPositive && in < 0.) in = in * mHarmonicsNormalized + dry * (1. - mHarmonicsNormalized);
			if (mPositive && in > 0.) in = in * mHarmonicsNormalized + dry * (1. - mHarmonicsNormalized);


			prev = dry;

			// return to old drive level
			in *= (1. / mDriveNormalized);

      return in;
		}

		inline double SRSaturation::processMusicDSP(double in) {
			if (fabs(in) > mAmount) {
				in = (in > 0.)
					? (mAmount + (fabs(in) - mAmount) / (1. + pow((fabs(in) - mAmount) / (1 - mAmount), 2))) * (in / fabs(in))
					: (mAmount + (fabs(in) - mAmount) / (1. + pow((fabs(in) - mAmount) / (1 - mAmount), 2))) * (in / fabs(in));
			}

			// Soften by (1 - Amount)
			in = (1. - mAmount) * in + mAmount * dry;

			// Saturation Normalization
			in *= (1. / ((mAmount + 1.) / 2.));

      return in;
		}

		inline double SRSaturation::processZoelzer(double in) {
			if (mAmountNormalized > 0.) {
				in = (in > 0.)
					? (1 - exp(-in)) * mAmountNormalized + dry * (1. - mAmountNormalized)
					: ((-1 + exp(in)) * mAmountNormalized + dry * (1. - mAmountNormalized));
			}
      return in;
		}

		inline double SRSaturation::processPirkle(double in) {

			if (mAmountNormalized > .001) {
				double mAmountModified = pow(mAmountNormalized, 3.);
				in = (in >= 0)
					? tanh(mAmountModified * in) / tanh(mAmountModified)
					: tanh(mAmountModified * in) / tanh(mAmountModified);
			}
      return in;
		}

		inline double SRSaturation::processPirkleModified(double in) {
			if (mAmountNormalized > .001) {
				double mAmountModified = pow(mAmountNormalized, 3.) * (1. + (in - prev) * (1. / mDriveNormalized) * mSkewNormalized);
				in = (in >= 0)
					? tanh(mAmountModified * in) / tanh(mAmountModified)
					: tanh(mAmountModified * in) / tanh(mAmountModified);
			}
      return in;
		}

		inline double SRSaturation::processRectHalf(double in) {
			in = (in < 0.) ? 0. : in;
      return in;
		}

		inline double SRSaturation::processRectFull(double in) {
			in = fabs(in);
      return in;
		}
	}
}