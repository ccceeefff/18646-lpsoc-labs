#include "SCCameraCommand.h"

#include "Arduino_Due_SD_HSCMI.h"

SCCameraCommand::SCCameraCommand(uint8_t command){
  _command = command; 
}

SCCameraCommand::SCCameraCommand(uint8_t command, String parameter){
  _command = command;
  _parameter = parameter.substring(0); //copy
}

uint8_t SCCameraCommand::getCommand(){
  return _command;
}

String SCCameraCommand::getParameter(){
  return _parameter;
}

/*
 * Camera Log
 */
SCCamera_log::SCCamera_log(QueueHandle_t commandQueue){
  _commandQueue = commandQueue;
}

int SCCamera_log::execute(SCCommand *command, SCStream *in, SCStream *out){
  if(command->getArgCount() == 0){
    out->println("Must specify enable or disable");
    return -1;
  }
  String arg = command->getArg(0);
  if(arg == "enable" || arg == "disable"){
    SCCameraCommand *cmd = new SCCameraCommand(CAMERA_SET_LOGGING, arg);
    xQueueSendToBack(_commandQueue, &cmd, portMAX_DELAY);  
  } else {
    out->println("Must specify enable or disable");
    return -2;
  }
  
  return 0;
}

String SCCamera_log::getCommand(){
  return "log";
}

String SCCamera_log::getDescription(){
  return "use to enable logging (camera log enable)";
}

/*
 * Camera Snapshot
 */

SCCamera_snapshot::SCCamera_snapshot(QueueHandle_t commandQueue){
  _commandQueue = commandQueue;
}

int SCCamera_snapshot::execute(SCCommand *command, SCStream *in, SCStream *out){
  out->println("Taking snapshot now!");
  SCCameraCommand *cmd = new SCCameraCommand(CAMERA_TAKE_PICTURE);
  xQueueSendToBack(_commandQueue, &cmd, portMAX_DELAY); 
  return 0;
}

String SCCamera_snapshot::getCommand(){
  return "snapshot";
}

String SCCamera_snapshot::getDescription(){
  return "take a snapshot using the camera";
}

/*
 * Camera Image Size
 */

SCCamera_imageSize::SCCamera_imageSize(QueueHandle_t commandQueue){
  _commandQueue = commandQueue;
}

int SCCamera_imageSize::execute(SCCommand *command, SCStream *in, SCStream *out){
  if(command->getArgCount() == 0){
    out->println("Must specify size!");
    return -1;
  }

  String arg = command->getArg(0);
  if(arg == "640x480" || arg == "320x240" || arg == "160x120"){
    SCCameraCommand *cmd = new SCCameraCommand(CAMERA_SET_IMAGE_SIZE, arg);
    xQueueSendToBack(_commandQueue, &cmd, portMAX_DELAY); 
  } else {
    out->println("Invalid image size specified.");
    return -2;
  }
  
  return 0;
}

String SCCamera_imageSize::getCommand(){
  return "imagesize";
}

String SCCamera_imageSize::getDescription(){
  return "set the size of the image to capture. use preset sizes e.g. 640x480, 320x240, etc";
}

/*
 * Camera Motion Detect
 */

SCCamera_motionDetect::SCCamera_motionDetect(QueueHandle_t commandQueue){
  _commandQueue = commandQueue;
}

int SCCamera_motionDetect::execute(SCCommand *command, SCStream *in, SCStream *out){
  if(command->getArgCount() == 0){
    out->println("Must specify enable or disable");
    return -1;
  }
  String arg = command->getArg(0);
  if(arg == "enable" || arg == "disable"){
    SCCameraCommand *cmd = new SCCameraCommand(CAMERA_SET_MOTION_DETECT, arg);
    xQueueSendToBack(_commandQueue, &cmd, portMAX_DELAY); 
  } else {
    out->println("Must specify enable or disable");
    return -2;
  }
  
  return 0;
}

String SCCamera_motionDetect::getCommand(){
  return "motiondetect";
}

String SCCamera_motionDetect::getDescription(){
  return "enable or disable motion detection. use with 'enable' to turn on motion detection";
}

/*
 * Camera Change Directory
 */

SCCamera_cd::SCCamera_cd(QueueHandle_t commandQueue){
  _commandQueue = commandQueue;
}

int SCCamera_cd::execute(SCCommand *command, SCStream *in, SCStream *out){

  // must have one string paramter
  if(command->getArgCount() == 0){
    out->println("Must specify target directory!");
    return -1;
  }

  String dir = command->getArg(0);
  if(SD.PathExists(dir.c_str())){
    SCCameraCommand *cmd = new SCCameraCommand(CAMERA_SET_IMAGE_DIRECTORY, dir);
    xQueueSendToBack(_commandQueue, &cmd, portMAX_DELAY); 
  } else {
    out->println(dir + " does not exists.");
    return -1;
  }
  
  return 0;
}

String SCCamera_cd::getCommand(){
  return "cd";
}

String SCCamera_cd::getDescription(){
  return "change where the camera should store images";
}

/*
 * Camera TV
 */

SCCamera_tv::SCCamera_tv(QueueHandle_t commandQueue){
  _commandQueue = commandQueue;
}

int SCCamera_tv::execute(SCCommand *command, SCStream *in, SCStream *out){
if(command->getArgCount() == 0){
    out->println("Must specify enable or disable");
    return -1;
  }
  String arg = command->getArg(0);
  if(arg == "enable" || arg == "disable"){
    SCCameraCommand *cmd = new SCCameraCommand(CAMERA_SET_TV_ENABLED, arg);
    xQueueSendToBack(_commandQueue, &cmd, portMAX_DELAY);  
  } else {
    out->println("Must specify enable or disable");
    return -2;
  }
  
  return 0;
}

String SCCamera_tv::getCommand(){
  return "tv";
}

String SCCamera_tv::getDescription(){
  return "enable or disable the camera tv output";
}