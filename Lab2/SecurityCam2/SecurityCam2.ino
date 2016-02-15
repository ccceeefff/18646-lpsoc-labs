#include <Arduino.h>
#include <FreeRTOS_ARM.h>
#include <basic_io_arm.h>

#include "SCCommand.h"

#define SCInputSerial SerialUSB

QueueHandle_t xCommandQueue;

void Task_Parser(void *arg){
  // parser task cannot start unless SCInputSerial is ready
  while(!SCInputSerial){
    taskYIELD();
  }

  SCInputSerial.println("Serial opened");
  
  while(1){
    if(SCInputSerial.available()){
      String inputBuffer = SCInputSerial.readStringUntil('\n');
      if(inputBuffer.length() > 0){
        inputBuffer.trim();
        SCCommand *command = new SCCommand(inputBuffer);
        xQueueSendToBack(xCommandQueue, &command, portMAX_DELAY);
      }
    } else {
      taskYIELD();
    }
  }
}

void Task_Processor(void *arg){

  SCCommand *command = NULL;
  while(1){
    // wait for commands to come in
    xQueueReceive(xCommandQueue, &command, portMAX_DELAY);
        
    //process commands
    SCInputSerial.println(command->getCommand());
    
    if(command->getArgCount() > 0){
      SCInputSerial.println("parameters:");
      for(int i=0; i < command->getArgCount(); i++){
        SCInputSerial.println(command->getArg(i));
      }
    }

    if(command != NULL){
      delete command;
    } 
    command = NULL;
  }
  
}

void setup() {

  // initialize objects
  SCInputSerial.begin(115200);

  // setup queues
  xCommandQueue = xQueueCreate(5, sizeof(SCCommand *));
  // setup tasks

  // create the parser task to wait for inputs from the serial port
  xTaskCreate(Task_Parser, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(Task_Processor, NULL, 200, NULL, 1, NULL);

  // start scheduler
  vTaskStartScheduler();
  
  vPrintString("Insufficient RAM\n");
  while(1);
}

void loop() {
  // not used in FreeRTOS
  // if we reach here, it means all tasks are blocked
}