/*
Taken from WDL-OL "NChanDelay.h"
*/

#ifndef SRBUFFER_H
#define SRBUFFER_H
#include "IPlug_include_in_plug_hdr.h"
#include <assert.h>

namespace SR {
  namespace DSP {


    //    // A static delayline used to delay bypassed signals to match mLatency in RTAS/AAX/VST3/AU
    //    //template<int MAXNUMCHANNELS = 1, int MAXNUMFRAMES = 1024>
    //    class SRBuffer
    //    {
    //    public:
    //      SRBuffer(int nChannels = 1, int nFrames = 1024)
    //        : mNumChannels(nChannels)
    //        , mNumFrames(nFrames)
    //        , mWriteAddress(0)
    //        //, mDTSamples(0)
    //      {
    //      }
    //
    //      // REPLACED:
    //      /*
    //      SRBuffer(int maxInputChans = 2, int maxOutputChans = 2)
    //        : mNumInChans(maxInputChans)
    //        , mNumOutChans(maxOutputChans)
    //        , mWriteAddress(0)
    //        , mDTSamples(0)
    //      {
    //      }
    //      */
    //
    //      ~SRBuffer() {}
    //
    //      void SetNumChannels(int nChannels) {
    //        SizeBuffer(nChannels, mNumFrames);
    //      }
    //
    //      void SetFrameSize(int nFrames) {
    //        SizeBuffer(mNumChannels, nFrames);
    //      }
    //
    //      void SizeBuffer(int nChannels, int nFrames) {
    //        //assert(nChannels <= MAXNUMCHANNELS);
    //        mNumChannels = nChannels;
    //        mNumFrames = nFrames;
    //        mBuffer.Resize(mNumChannels * mNumFrames);
    //        mWriteAddress = 0;
    //        ClearBuffer();
    //      }
    //
    //      // REPLACED: 
    //      /*void SetDelayTime(int delayTimeSamples)
    //      {
    //        mDTSamples = delayTimeSamples;
    //        mBuffer.Resize(mNumInChans * delayTimeSamples);
    //        mWriteAddress = 0;
    //        ClearBuffer();
    //      }*/
    //
    //      void ClearBuffer() {
    //        memset(mBuffer.Get(), 0, mNumChannels * mNumFrames * sizeof(double));
    //      }
    //
    //      // REPLACED:
    //      /*
    //      void ClearBuffer()
    //      {
    //        memset(mBuffer.Get(), 0, mNumInChans * mDTSamples * sizeof(double));
    //      }
    //      */
    //
    //      void SetBufferSample(double in, int channel, int sample) {
    //        mBuffer.Get()[channel * mNumFrames + sample] = in;
    //      }
    //
    //      void SetBufferChannel(double* in, int channel) {
    //        for (int s = 0; s < mNumFrames; s++) {
    //          mBuffer.Get()[channel * mNumFrames + s] = in[s];
    //        }
    //      }
    //
    //      void SetBuffer(double** in) {
    //        for (int s = 0; s < mNumFrames; s++) {
    //          for (int c = 0; c < mNumChannels; c++) {
    //            mBuffer.Get()[c * mNumFrames + s] = in[c][s];
    //          }
    //        }
    //      }
    //      double GetBufferSample(int channel, int sample) {
    //        return mBuffer.Get()[channel * mNumFrames + sample];
    //      }
    //
    //      double* GetBufferChannel(int channel) {
    //        double* returnbuffer;
    //        for (int s = 0; s < mNumFrames; s++) {
    //          returnbuffer[s] = mBuffer.Get()[channel * mNumFrames + s];
    //        }
    //        return returnbuffer;
    //      }
    //
    //      double** GetBuffer() {
    //        double *bufferChannel = new double[mNumFrames];
    //        double **buffer = &bufferChannel;
    //        for (int s = 0; s < mNumFrames; s++) {
    //          for (int c = 0; c < mNumChannels; c++) {
    //            buffer[c][s] = mBuffer.Get()[mNumChannels * mNumFrames + s];
    //          }
    //        }
    //        return buffer;
    //      }
    //
    //      double SumBuffer() {
    //        double sum = 0.0;
    //        for (int i = 0; i < mBuffer.GetSize(); i++) {
    //          sum += mBuffer.Get()[i];
    //        }
    //        return sum;
    //      }
    //
    //      double AverageBuffer() {
    //        return SumBuffer() / (double)mBuffer.GetSize();
    //      }
    //
    //
    //      //void ProcessBlock(double** inputs, double** outputs, int nFrames)
    //      //{
    //      //  double *buffer = mBuffer.Get();
    //
    //      //  for (int s = 0; s < nFrames; ++s)
    //      //  {
    //      //    signed long readAddress = mWriteAddress - mNumFrames;
    //      //    readAddress %= mNumFrames;
    //
    //      //    for (int c = 0; c < mNumChannels; c++)
    //      //    {
    //      //      if (c < mNumChannels)
    //      //      {
    //      //        int offset = c * mNumFrames;
    //      //        outputs[c][s] = buffer[offset + readAddress];
    //      //        buffer[offset + mWriteAddress] = inputs[c][s];
    //      //      }
    //      //    }
    //
    //      //    mWriteAddress++;
    //      //    mWriteAddress %= mNumFrames;
    //      //  }
    //      //}
    //
    //      // COMMENTED:
    //        /*
    //        void ProcessBlock(double** inputs, double** outputs, int nFrames)
    //        {
    //          double* buffer = mBuffer.Get();
    //
    //          for (int s = 0; s < nFrames; ++s)
    //          {
    //            signed long readAddress = mWriteAddress - mDTSamples;
    //            readAddress %= mDTSamples;
    //
    //            for (int chan = 0; chan < mNumInChans; chan++)
    //            {
    //              if (chan < mNumOutChans)
    //              {
    //                int offset = chan * mDTSamples;
    //                outputs[chan][s] = buffer[offset + readAddress];
    //                buffer[offset + mWriteAddress] = inputs[chan][s];
    //              }
    //            }
    //
    //            mWriteAddress++;
    //            mWriteAddress %= mDTSamples;
    //          }
    //        }
    //        */
    //
    //
    //
    //    private:
    //      WDL_TypedBuf<double> mBuffer;
    //      unsigned long mWriteAddress;
    //      unsigned int mNumFrames;
    //      unsigned int mNumChannels;
    //      //unsigned long mDTSamples;
    //
    //
    //      // REPLACED:
    //      /*
    //      WDL_TypedBuf<double> mBuffer;
    //      unsigned long mWriteAddress;
    //      unsigned int mNumInChans, mNumOutChans;
    //      unsigned long mDTSamples;
    //      */
    //
    //    };
    //
    //    //REPLACED:
    //    /*
    //    } WDL_FIXALIGN;
    //    */
    //
    //


    template<int MAXNUMCHANNELS = 1>
    class SRBuffer {
    public:
      SRBuffer(int nChannels = MAXNUMCHANNELS, int nFrames = 1024)
        : mNumChannels(nChannels)
        , mNumFrames(nFrames)
      {
        mBuffer = new double*[mNumChannels];
        for (int c = 0; c < mNumChannels; c++) {
          mBuffer[c] = new double[mNumFrames];
        }
      }

      void SetNumFrames(int nFrames) {
        if (nFrames != mNumFrames) ResetBuffer(mNumChannels, nFrames);
      }

      void SetNumChannels(int nChannels) {
        if (nChannels != mNumChannels) ResetBuffer(nChannels, mNumFrames);
      }

      void ResetBuffer(int nChannels = MAXNUMCHANNELS, int nFrames = 1024) {
        // delete dynamic 2D array
        for (int c = 0; c < mNumChannels; c++) {
          if (mBuffer[c]) delete[]mBuffer[c];
        }
        if (mBuffer) delete[]mBuffer;

        // change channel and frames count
        assert(nChannels <= MAXNUMCHANNELS);
        mNumChannels = nChannels;
        mNumFrames = nFrames;

        // create new 2D array;
        mBuffer = new double*[mNumChannels];
        for (int c = 0; c < mNumChannels; c++) {
          mBuffer[c] = new double[mNumFrames];
        }
      }

      void ProcessBuffer(double **in) { mBuffer = in; }
      void ProcessBuffer(double *in, int channel) { mBuffer[channel] = in; }
      void ProcessBuffer(double in, int channel, int sample) { mBuffer[channel][sample] = in; }
      double** GetBuffer() { return mBuffer; }
      double* GetBuffer(int channel) { return mBuffer[channel]; }
      double GetBuffer(int channel, int sample) { return mBuffer[channel][sample]; }

      double AverageBuffer() {
        double sum = 0.0;
        for (int c = 0; c < mNumChannels; c++) {
          for (int s = 0; s < mNumFrames; s++) {
            sum += std::fabs(mBuffer[c][s]);
          }
        }
        return mNumChannels * sum / (double)mNumFrames;
      }

      double AverageBuffer(int channel) {
        double sum = 0.0;
        for (int c = 0; c < mNumChannels; c++) {
          for (int s = 0; s < mNumFrames; s++) {
            sum += std::fabs(mBuffer[channel][s]);
          }
        }
        return sum / (double)mNumFrames;
      }


    private:
      int mNumChannels;
      int mNumFrames;
      double **mBuffer;

    };
    // End of namespaces:
  }
}
#endif //SRBUFFER_H