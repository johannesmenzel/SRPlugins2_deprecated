/*
Taken from WDL-OL "NChanDelay.h"
*/

#ifndef SRBUFFER_H
#define SRBUFFER_H
#include "IPlug_include_in_plug_hdr.h"
#include <cassert>
#include <vector>

namespace SR {
  namespace DSP {


    // A static delayline used to delay bypassed signals to match mLatency in RTAS/AAX/VST3/AU
    template<class T = double, int MAXNUMCHANNELS = 1, int MAXNUMFRAMES = 1024>
    class SRBuffer
    {
    public:
      SRBuffer(int nChannels = MAXNUMCHANNELS, int nFrames = MAXNUMFRAMES)
        : mNumChannels(nChannels)
        , mNumFrames(nFrames)
        , mWriteAddress(0)
        //, mDTSamples(0)
      {
        mBuffer.reserve(MAXNUMCHANNELS);
        mBuffer.resize(mNumChannels);
        for (int c = 0; c < mNumChannels; c++) {
          mBuffer[c].Resize(mNumFrames);
        }
      }

      // REPLACED:
      /*
      SRBuffer(int maxInputChans = 2, int maxOutputChans = 2)
        : mNumInChans(maxInputChans)
        , mNumOutChans(maxOutputChans)
        , mWriteAddress(0)
        , mDTSamples(0)
      {
      }
      */

      ~SRBuffer() {
        mBuffer.clear();
      }


      // we dont want to set channelsize, because we have MAXNUMCHANNELS
      //void SetNumChannels(int nChannels) {
      //  if (nChannels > mNumChannels) {
      //    for (c = mNumChannels; c < nChannels; c++) {
      //      mBuffer.Add(0.0);
      //    }
      //  }
      //  else
      //  {

      //  }
      //  mBuffer.Delete()

      //  SizeBuffer(nChannels, mNumFrames);
      //}

      void SetNumFrames(int nFrames = MAXNUMFRAMES) {
        for (int c = 0; c < mNumChannels; c++) {
          mNumFrames = nFrames;
          mBuffer[c].Resize(mNumFrames, true);
        }
      }

      void ResetBuffer(int nChannels = MAXNUMCHANNELS, int nFrames = MAXNUMFRAMES) {
        assert(nChannels <= MAXNUMCHANNELS);
        mNumChannels = nChannels;
        for (int c = 0; c < mNumChannels; c++) {
          mBuffer[c].Resize(mNumFrames, true);
        }
        SetNumFrames(nFrames);
        //ClearBuffer();
      }

      // REPLACED: 
      /*void SetDelayTime(int delayTimeSamples)
      {
        mDTSamples = delayTimeSamples;
        mBuffer.Resize(mNumInChans * delayTimeSamples);
        mWriteAddress = 0;
        ClearBuffer();
      }*/

      void ClearBuffer() {
        for (int c = 0; c < mNumChannels; c++) {
          //memset(mBuffer[c].Get(), 0, mNumChannels * mBuffer[c].GetSize() * sizeof(T));
          mBuffer.clear();
        }
      }

      // REPLACED:
      /*
      void ClearBuffer()
      {
        memset(mBuffer.Get(), 0, mNumInChans * mDTSamples * sizeof(double));
      }
      */

      void ProcessBuffer(T in, int channel, int sample) {
        mBuffer[channel].Get()[sample] = in;
      }

      void ProcessBuffer(T* in, int channel) {
        mBuffer[channel].Get() = in;
      }

      void ProcessBuffer(T** in) {
        for (int c = 0; c < mNumChannels; c++) {
          mBuffer[c].Get() = in[c];
        }
      }

      T GetBuffer(int channel, int sample) {
        return mBuffer[channel].Get()[sample];
      }

      T* GetBuffer(int channel) {
        return mBuffer[channel].Get();
      }

      T** GetBuffer() {
        //T** buffer = new T*[mNumChannels];
        //for (int c = 0; c < mNumChannels; c++) {
        //  buffer[c] = mBuffer[c].Get();
        //}
        //return buffer;
        return mBuffer.get_allocator().;

      }

      T SumBuffer() {
        double sum = (T)0.0;
        for (int c = 0; c < mNumChannels; c++) {
          for (int s = 0; s < mNumFrames; s++) {
            sum += mBuffer[c].Get()[s];
          }
        }
        return sum;
      }

      T AverageBuffer() {
        return SumBuffer() / (mNumFrames * mNumChannels);
      }



      // COMMENTED:
        /*
        void ProcessBlock(double** inputs, double** outputs, int nFrames)
        {
          double* buffer = mBuffer.Get();

          for (int s = 0; s < nFrames; ++s)
          {
            signed long readAddress = mWriteAddress - mDTSamples;
            readAddress %= mDTSamples;

            for (int chan = 0; chan < mNumInChans; chan++)
            {
              if (chan < mNumOutChans)
              {
                int offset = chan * mDTSamples;
                outputs[chan][s] = buffer[offset + readAddress];
                buffer[offset + mWriteAddress] = inputs[chan][s];
              }
            }

            mWriteAddress++;
            mWriteAddress %= mDTSamples;
          }
        }
        */



    private:
      std::vector<WDL_TypedBuf<T>> mBuffer;
      unsigned long mWriteAddress;
      unsigned int mNumFrames;
      unsigned int mNumChannels;
      //unsigned long mDTSamples;


      // REPLACED:
      /*
      WDL_TypedBuf<double> mBuffer;
      unsigned long mWriteAddress;
      unsigned int mNumInChans, mNumOutChans;
      unsigned long mDTSamples;
      */

    };

    //REPLACED:
    /*
    } WDL_FIXALIGN;
    */




    template<class T = double, int MAXNUMCHANNELS = 1, int MAXNUMFRAMES = 1024>
    class SRBuffer2 {
    public:
      SRBuffer2(int nChannels = MAXNUMCHANNELS, int nFrames = MAXNUMFRAMES)
        : mNumChannels(nChannels)
        , mNumFrames(nFrames)
        , mBuffer(new T*[nChannels])
      {
        for (int c = 0; c < mNumChannels; c++) {
          mBuffer[c] = new T[mNumFrames];
        }
      }


      ~SRBuffer2() {

      }

      void SetNumFrames(int nFrames) {
        if (nFrames != mNumFrames) ResetBuffer(mNumChannels, nFrames);
      }

      void SetNumChannels(int nChannels) {
        if (nChannels != mNumChannels) ResetBuffer(nChannels, mNumFrames);
      }

      void ResetBuffer(int nChannels = MAXNUMCHANNELS, int nFrames = MAXNUMFRAMES) {
        // delete dynamic 2D array
        for (int c = 0; c < mNumChannels; c++) {
          if (mBuffer[c]) delete[] mBuffer[c];
        }
        if (mBuffer) delete[] mBuffer;

        // change channel and frames count
        assert(nChannels <= MAXNUMCHANNELS);
        mNumChannels = nChannels;
        mNumFrames = nFrames;

        // create new 2D array;
        mBuffer = new T*[mNumChannels];
        for (int c = 0; c < mNumChannels; c++) {
          mBuffer[c] = new T[mNumFrames];
        }
      }

      void ProcessBuffer(T **in) { mBuffer = in; }
      void ProcessBuffer(T *in, int channel = 0) { mBuffer[channel] = in; }
      void ProcessBuffer(T in, int channel = 0, int sample = 0) { mBuffer[channel][sample] = in; }
      T** GetBuffer()                         { return mBuffer; }
      T*  GetBuffer(int channel)              { return mBuffer[channel]; }
      T   GetBuffer(int channel, int sample)  { return mBuffer[channel][sample]; }

      T AverageBuffer() {
        T sum = 0.0;
        for (int c = 0; c < mNumChannels; c++) {
          for (int s = 0; s < mNumFrames; s++) {
            sum += std::fabs(mBuffer[c][s]);
          }
        }
        return mNumChannels * sum / (T)mNumFrames;
      }

      T AverageBuffer(int channel) {
        T sum = 0.0;
          for (int s = 0; s < mNumFrames; s++) {
            sum += std::fabs(mBuffer[channel][s]);
          }
        return sum / (T)mNumFrames;
      }


    private:
      int mNumChannels;
      int mNumFrames;
      T **mBuffer;

    };
    // End of namespaces:
  }
}
#endif //SRBUFFER_H