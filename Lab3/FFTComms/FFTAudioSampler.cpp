#include "FFTAudioSampler.h"

#include <assert.h>
#include <Arduino.h>
#include <DueTimer.h>

#define MICROS_PER_SEC (1 * 1000 * 1000)

void FFTAudioSamplerISR(){
  AudioSampler.sample();
}

FFTAudioSampler::FFTAudioSampler(){
	_currentBuffer = 0;
  _currentIndex = 0;
  _callback = NULL;
  analogReadResolution(12);
}

void FFTAudioSampler::startSampling(unsigned int samplingFrequency){
	assert(samplingFrequency > 0);

  _currentBuffer = 0;
  _currentIndex = 0;
  
	Timer3.attachInterrupt(FFTAudioSamplerISR);
	Timer3.start(MICROS_PER_SEC / samplingFrequency);
}

void FFTAudioSampler::stopSampling(){
	Timer3.stop();
}

void FFTAudioSampler::sample(){
  int value = analogRead(ADC_PORT);

  _audioBuffer[_currentBuffer][_currentIndex++] = (double)value;///4096.0) * 3.3;
  
  if(_currentIndex >= NUM_SAMPLES){
     // trigger callback 
     if(_callback != NULL){
      _callback(getCurrentBuffer(), getBufferSize());
     }
    
     // move to next buffer
     _currentBuffer = (_currentBuffer + 1) % NUM_BUFFERS;
     // reset index
     _currentIndex = 0;
  }
}

double *FFTAudioSampler::getCurrentBuffer(){
  return &_audioBuffer[_currentBuffer][0];
}

unsigned int FFTAudioSampler::getBufferSize(){
  return NUM_SAMPLES;
}

void FFTAudioSampler::setBufferReadyCallback(BufferReadyCallback callback){
  _callback = callback;
}

FFTAudioSampler AudioSampler = FFTAudioSampler();

