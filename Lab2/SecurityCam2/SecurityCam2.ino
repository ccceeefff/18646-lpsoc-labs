#include <Arduino.h>
#include <FreeRTOS_ARM.h>
#include <basic_io_arm.h>

#include "Arduino_Due_SD_HSCMI.h"
#include "FileUtils.h"
#include "SCCameraRegister.h"
#include "SCProgramRegistry.h"
#include "SCShell.h"
#include "SCStream.h"

#define SCInputSerial SerialUSB

QueueHandle_t xCommandQueue;

#define PRINT_QUEUE_SIZE 10
#define PRINT_QUEUE_SIZE_TOTAL (2*PRINT_QUEUE_SIZE)

QueueSetHandle_t xPrintQueueSet;
QueueHandle_t xParserPrintQueue;
QueueHandle_t xProcessorPrintQueue;

SCProgramRegistry *shellRegistry;
SCCameraRegistry *cameraRegistry;

SCStream *inStream = new SCSerialStream(&SerialUSB);
SCStream *parserOutStream = new SCSerialStream(&SerialUSB);
SCStream *processorOutStream = new SCSerialStream(&SerialUSB);

boolean interactiveShellMode = false; 

static void Task_Parser(void *arg){
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

        if(inputBuffer == "interactive"){
          // echo if interactive
          interactiveShellMode = !interactiveShellMode;
          if(interactiveShellMode){
            parserOutStream->println("interactive shell on");  
          } else {
            parserOutStream->println("interactive shell off");
          }
        } else {
          if(interactiveShellMode){
            parserOutStream->println(inputBuffer);
          }
          SCCommand *command = new SCCommand(inputBuffer);
          xQueueSendToBack(xCommandQueue, &command, portMAX_DELAY);
        }
      }
    } else {
      taskYIELD();
    }
  }
}

static void Task_Processor(void *arg){

  SCCommand *command = NULL;
  while(1){
    // wait for commands to come in
    xQueueReceive(xCommandQueue, &command, portMAX_DELAY);
        
    //process commands
    if(shellRegistry->execute(command, inStream, processorOutStream) != 0){
      //processorOutStream->println("Error occured...");
    }

    if(command != NULL){
      delete command;
    } 
    command = NULL;
  }
  
}

static void Task_SerialOutGateKeeper(void *arg){
  SCQueueBuffer *buffer = NULL;
  while(1){
    QueueHandle_t activeQueue = xQueueSelectFromSet( xPrintQueueSet, portMAX_DELAY );
    xQueueReceive(activeQueue, &buffer, 0);
    if(buffer != NULL){
      SerialUSB.write(buffer->getBuffer(), buffer->getSize());
      delete buffer; 
    }
    buffer = NULL;                                          
  }
}

static void Task_CameraDriver(void *arg){
  while(1){
    
  }
}

void register_camera_commands(SCProgramRegistry *registry){
  
}

void register_shell_commands(SCProgramRegistry *registry){
  registry->registerProgram(new SCShell_pwd());
  registry->registerProgram(new SCShell_cd());
  registry->registerProgram(new SCShell_ls());
  registry->registerProgram(new SCShell_cat());
  registry->registerProgram(new SCShell_mkdir());
  registry->registerProgram(new SCShell_rm());
  registry->registerProgram(new SCShell_mv());
}

void setup() {
  

  // initialize objects
  SCInputSerial.begin(115200);
  SD.Init();
  fUtils.Init();
  cameraRegistry = new SCCameraRegistry();
  shellRegistry = new SCProgramRegistry();
  register_shell_commands(shellRegistry);
  shellRegistry->registerProgram(cameraRegistry);
  // setup queues
  xCommandQueue = xQueueCreate(5, sizeof(SCCommand *));

  xParserPrintQueue = xQueueCreate(PRINT_QUEUE_SIZE, sizeof(SCQueueBuffer *));
  xProcessorPrintQueue = xQueueCreate(PRINT_QUEUE_SIZE, sizeof(SCQueueBuffer *));

  xPrintQueueSet = xQueueCreateSet( PRINT_QUEUE_SIZE_TOTAL );

  xQueueAddToSet( xParserPrintQueue, xPrintQueueSet );
  xQueueAddToSet( xProcessorPrintQueue, xPrintQueueSet );

  parserOutStream = new SCQueueStream(xParserPrintQueue);
  processorOutStream = new SCQueueStream(xProcessorPrintQueue);
  
  // setup tasks

  // create the parser task to wait for inputs from the serial port
  xTaskCreate(Task_Parser, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(Task_Processor, NULL, 400, NULL, 1, NULL);
  xTaskCreate(Task_SerialOutGateKeeper, NULL, 200, NULL, 1, NULL);

  // start scheduler
  vTaskStartScheduler();
  
  vPrintString("Insufficient RAM\n");
  while(1);
}

void loop() {
  // not used in FreeRTOS
  // if we reach here, it means all tasks are blocked
}
