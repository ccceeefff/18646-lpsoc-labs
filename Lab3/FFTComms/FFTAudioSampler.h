#ifndef FFTAUDIOSAMPLER_H
#define FFTAUDIOSAMPLER_H

#include "FFTConfig.h"

typedef void (*BufferReadyCallback)(double *audioBuffer, int bufferSize);

class FFTAudioSampler {
	public:
		FFTAudioSampler();
		void startSampling(unsigned int samplingFrequency);
		void stopSampling();

		double *getCurrentBuffer();
		unsigned int getBufferSize();

    void setBufferReadyCallback(BufferReadyCallback callback);

		void sample();

	private:
		double _audioBuffer[NUM_BUFFERS][NUM_SAMPLES];
		unsigned int _currentBuffer;
    unsigned int _currentIndex;
    BufferReadyCallback _callback;
};

extern FFTAudioSampler AudioSampler;

#endif //FFTAUDIOSAMPLER_H
