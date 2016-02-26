#include <arduinoFFT.h>
#include <FreeRTOS_ARM.h>

#include "FFTAudioSampler.h"

#define AUDIO_BUFFER_QUEUE_SIZE 5
QueueHandle_t xAudioBufferQueue;

arduinoFFT FFT = arduinoFFT();

double vReal[NUM_SAMPLES];
double vImag[NUM_SAMPLES];

void buffer_ready(double *audioSample, int bufferSize){
  BaseType_t xHigherPriorityTaskWoken;
  xQueueSendToBackFromISR(xAudioBufferQueue, &audioSample, &xHigherPriorityTaskWoken);
}

static void Task_FFT(void *arg){
  double *audioBuffer = NULL;
  while(1){
    if(xQueueReceive(xAudioBufferQueue, &audioBuffer, portMAX_DELAY)){
      if(audioBuffer != NULL){
        //copy data over to fft buffer
        for(int i=0; i < NUM_SAMPLES; i++){
          vReal[i] = audioBuffer[i];
          vImag[i] = 0;
        }
        // compute FFT
        if(LOCAL_FFT){
          FFT.Windowing(vReal, NUM_SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
          FFT.Compute(vReal, vImag, NUM_SAMPLES, FFT_FORWARD); /* Compute FFT */
          FFT.ComplexToMagnitude(vReal, vImag, NUM_SAMPLES); /* Compute magnitudes */
          double x = FFT.MajorPeak(vReal, NUM_SAMPLES, SAMPLING_RATE);
          
          SerialUSB.print("major peak: ");
          SerialUSB.println(x);
        }
      }
    }
  }
}

static void Task_SerialOutGateKeeper(void *arg){
  uint16_t buffer = 0;
  while(1){
    taskYIELD();
  }
}

void setup() {
  SerialUSB.begin(115200);
  while(!SerialUSB);
  SerialUSB.println("Ready!");

  AudioSampler.setBufferReadyCallback(buffer_ready);

  xAudioBufferQueue = xQueueCreate(AUDIO_BUFFER_QUEUE_SIZE, sizeof(uint16_t *));

  // xTaskCreate(Task_SerialOutGateKeeper, NULL, configMINIMAL_STACK_SIZE, NULL, 0, NULL);
  xTaskCreate(Task_FFT, NULL, configMINIMAL_STACK_SIZE, NULL, 0, NULL);

  SerialUSB.println("Tasks ready!");

  // // start scheduler
  SerialUSB.println("Start!");

  AudioSampler.startSampling(SAMPLING_RATE);
  vTaskStartScheduler();
  
  SerialUSB.println("Insufficient RAM\n");
  while(1);
}

void loop() {
  // put your main code here, to run repeatedly:
}
