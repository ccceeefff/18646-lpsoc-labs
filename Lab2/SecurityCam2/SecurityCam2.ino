#include <Arduino.h>
#include <FreeRTOS_ARM.h>
#include <basic_io_arm.h>

#include "Arduino_Due_SD_HSCMI.h"
#include "FileUtils.h"
#include "PowerDue.h"
#include "SCCamera.h"
#include "SCCameraCommand.h"
#include "SCCameraRegister.h"
#include "SCProgramRegistry.h"
#include "SCShell.h"
#include "SCStream.h"

#define SCInputSerial SerialUSB

#define COMMAND_QUEUE_SIZE 10

QueueHandle_t xCommandQueue;

#define CAMERA_QUEUE_SIZE 5
#define CAMERA_QUEUE_SIZE_TOTAL (5*CAMERA_QUEUE_SIZE)

QueueSetHandle_t xCameraDriverQueueSet;
QueueHandle_t xCameraDriverCommandQueue;
QueueHandle_t xCameraDriverRetryQueue;

#define PRINT_QUEUE_SIZE 10
#define PRINT_QUEUE_SIZE_TOTAL (3*PRINT_QUEUE_SIZE)

QueueSetHandle_t xPrintQueueSet;
QueueHandle_t xParserPrintQueue;
QueueHandle_t xProcessorPrintQueue;
QueueHandle_t xCameraDriverPrintQueue;

Adafruit_VC0706 cam = Adafruit_VC0706(&PDSerialCamera);
SCCamera cameraDriver = SCCamera(&cam);

SCProgramRegistry *shellRegistry;
SCCameraRegistry *cameraRegistry;

SCStream *inStream = new SCSerialStream(&SerialUSB);
SCStream *parserOutStream = new SCSerialStream(&SerialUSB);
SCStream *processorOutStream = new SCSerialStream(&SerialUSB);
SCStream *cameraDriverOutStream = new SCSerialStream(&SerialUSB);

boolean interactiveShellMode = false; 

static void Task_Parser(void *arg){
  // parser task cannot start unless SCInputSerial is ready
  while(!SCInputSerial){
    taskYIELD();
  }

  parserOutStream->println("Serial opened");
  
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
  cameraDriver.Init(cameraDriverOutStream);
  cameraDriverOutStream->println("Camera Driver ready!");
  
  SCCameraCommand *command = NULL;
  while(1){
    // check the queue for any incoming commands
    QueueHandle_t activeQueue = xQueueSelectFromSet( xCameraDriverQueueSet, 0 ); // do not block
    if(activeQueue != NULL){
      if(xQueueReceive(activeQueue, &command, 0)){
        
        boolean retry = false;
        // at this time, the assumption is that all commands are properly formatted
        // no need to check for improper parameters, etc.
        switch(command->getCommand()){
          case CAMERA_SET_LOGGING:
            cameraDriver.setLogging(command->getParameter() == "enable");
            break;
          case CAMERA_TAKE_PICTURE:
            cameraDriver.takePicture();
            break;
          case CAMERA_SET_IMAGE_SIZE: {
            if(command->getParameter() == "640x480"){
              retry = !cameraDriver.setImageSize(VC0706_640x480);
            } else if(command->getParameter() == "320x240"){
              retry = !cameraDriver.setImageSize(VC0706_320x240);
            } else if(command->getParameter() == "160x120"){
              retry = !cameraDriver.setImageSize(VC0706_160x120);
            }
          }
            break;
          case CAMERA_SET_MOTION_DETECT:
            retry = !cameraDriver.setMotionDetect(command->getParameter() == "enable");
            break;
          case CAMERA_SET_IMAGE_DIRECTORY:
            cameraDriver.setImageDirectory(command->getParameter());
            break;
        }

        if(retry){
          xQueueSendToBack(xCameraDriverRetryQueue, &command, portMAX_DELAY);
        } else if(command != NULL){
          delete command;
        }
        command = NULL;
      }
    } else {
      // if no command, try to check if motion was detected
      cameraDriver.checkMotionDetected();
    }
  }
}

void register_camera_commands(SCProgramRegistry *registry){
  registry->registerProgram(new SCCamera_log(xCameraDriverCommandQueue));
  registry->registerProgram(new SCCamera_snapshot(xCameraDriverCommandQueue));
  registry->registerProgram(new SCCamera_imageSize(xCameraDriverCommandQueue));
  registry->registerProgram(new SCCamera_motionDetect(xCameraDriverCommandQueue));
  registry->registerProgram(new SCCamera_cd(xCameraDriverCommandQueue));
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

  pd_rtos_init();
  
  // initialize objects
  SCInputSerial.begin(115200);
  while(!SCInputSerial);
  SCInputSerial.println("Ready!");
  SD.Init();
  fUtils.Init();

  SCInputSerial.println("Filesystem Ready!");
  
  // setup queues

  // Command Queue
  xCommandQueue = xQueueCreate(COMMAND_QUEUE_SIZE, sizeof(SCCommand *));

  // Print Queues
  xParserPrintQueue = xQueueCreate(PRINT_QUEUE_SIZE, sizeof(SCQueueBuffer *));
  xProcessorPrintQueue = xQueueCreate(PRINT_QUEUE_SIZE, sizeof(SCQueueBuffer *));
  xCameraDriverPrintQueue = xQueueCreate(PRINT_QUEUE_SIZE, sizeof(SCQueueBuffer *));

  xPrintQueueSet = xQueueCreateSet( PRINT_QUEUE_SIZE_TOTAL );

  xQueueAddToSet( xParserPrintQueue, xPrintQueueSet );
  xQueueAddToSet( xProcessorPrintQueue, xPrintQueueSet );
  xQueueAddToSet( xCameraDriverPrintQueue, xPrintQueueSet );

  // Camera Queues
  xCameraDriverCommandQueue = xQueueCreate(CAMERA_QUEUE_SIZE, sizeof(SCCameraCommand *));
  xCameraDriverRetryQueue = xQueueCreate(CAMERA_QUEUE_SIZE, sizeof(SCCameraCommand *));

  xCameraDriverQueueSet = xQueueCreateSet( CAMERA_QUEUE_SIZE_TOTAL );
  
  xQueueAddToSet( xCameraDriverCommandQueue, xCameraDriverQueueSet );
  xQueueAddToSet( xCameraDriverRetryQueue, xCameraDriverQueueSet );

  parserOutStream = new SCQueueStream(xParserPrintQueue);
  processorOutStream = new SCQueueStream(xProcessorPrintQueue);
  cameraDriverOutStream = new SCQueueStream(xCameraDriverPrintQueue);

  SCInputSerial.println("Queues ready!");
  
  cameraRegistry = new SCCameraRegistry();
  shellRegistry = new SCProgramRegistry();
  register_shell_commands(shellRegistry);
  shellRegistry->registerProgram(cameraRegistry);
  register_camera_commands(cameraRegistry);

  SCInputSerial.println("Registry ready!");
  
  // setup tasks

  // create the parser task to wait for inputs from the serial port
  xTaskCreate(Task_Parser, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(Task_Processor, NULL, 800, NULL, 1, NULL);
  xTaskCreate(Task_SerialOutGateKeeper, NULL, 200, NULL, 1, NULL);
  xTaskCreate(Task_CameraDriver, NULL, 800, NULL, 1, NULL);

  SCInputSerial.println("Tasks ready!");

  // start scheduler
  SCInputSerial.println("Start!");
  vTaskStartScheduler();
  
  vPrintString("Insufficient RAM\n");
  while(1);
}

void loop() {
  // not used in FreeRTOS
  // if we reach here, it means all tasks are blocked
}
