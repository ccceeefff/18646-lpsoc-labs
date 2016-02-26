#include <FreeRTOS_ARM.h>

#include "FFTAudioSampler.h"

#define AUDIO_BUFFER_QUEUE_SIZE 5
QueueHandle_t xAudioBufferQueue;
uint16_t _buffer = 1;

void buffer_ready(double *audioSample, int bufferSize){
  BaseType_t xHigherPriorityTaskWoken;
  xQueueSendToBackFromISR(xAudioBufferQueue, &_buffer, &xHigherPriorityTaskWoken);
}

static void Task_SerialOutGateKeeper(void *arg){
  uint16_t buffer = 0;
  while(1){
    xQueueReceive(xAudioBufferQueue, &buffer, portMAX_DELAY);
    SerialUSB.println("Buffer full!");
  }
}

void setup() {
  SerialUSB.begin(115200);
  while(!SerialUSB);
  SerialUSB.println("Ready!");

  AudioSampler.setBufferReadyCallback(buffer_ready);

  xAudioBufferQueue = xQueueCreate(AUDIO_BUFFER_QUEUE_SIZE, sizeof(uint16_t *));

  xTaskCreate(Task_SerialOutGateKeeper, NULL, configMINIMAL_STACK_SIZE, NULL, 0, NULL);

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
