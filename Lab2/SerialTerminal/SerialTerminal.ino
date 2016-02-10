#include "Adafruit_VC0706.h"
#include "Arduino_Due_SD_HSCMI.h"
#include "Interpreter.h"
#include "PowerDue.h"
#include "shell.h"

Adafruit_VC0706 cam = Adafruit_VC0706(&PDSerialCamera);
Interpreter *interpreter;

// Note: need to create re-entrant version of SD library

void takePicture(Stream *stream){
  if(cam.takePicture()){
    stream->println("Picture taken!");
  } else {
    stream->println("Failed to snap!");
    pd_rgb_led(PD_RED);
    return;
  }

  // initialize files here...
  // assume that this directory exists
  // create it using the command prompt
  char directory[20] = "Cef/lab2images";

  // Find unused filename to create new file
  char filename[14];
  strcpy(filename, "IMGSM0000.JPG");
  for (int i = 0; i < 100; i++) {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.FileExists(SD.CombineName(directory, filename))) {
      break;
    }
  }

  // create file for writing
  FileStore theFile = FileStore();
  theFile.Init();
  if(!theFile.Open(directory, filename, FILE_WRITE)){
    stream->println("Failed to open file for writing.");
    cam.resumeVideo();
    return;
  }

  // dump data into stream
  uint16_t jpglen = cam.frameLength();
  stream->print("picture size: ");
  stream->print(jpglen, DEC);
  stream->println(" byte image.");

  while (jpglen > 0) {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = cam.readPicture(bytesToRead); // request chunk from camera
    theFile.Write((char *)buffer, bytesToRead); // write chunk to SD card
    jpglen -= bytesToRead;
  }
  theFile.Close();
  stream->println("Done!");

  cam.resumeVideo();
}

// commands to capture using camera
void cam_takePicture(Stream *stream, const char *params[], int paramCount){ 
  pd_rgb_led(PD_GREEN);
  takePicture(stream);
  pd_rgb_led(PD_RED);
}

void cam_motionDetect(Stream *stream, const char *params[], int paramCount){
  cam.setMotionDetect(true);
  pd_rgb_led(PD_BLUE);
  while(!cam.motionDetected());
  pd_rgb_led(PD_GREEN);
  cam.setMotionDetect(false);
  takePicture(stream);
  pd_rgb_led(PD_RED);
}

void setup() {

  pd_rgb_led_init();

  // setup SerialUSB
  SerialUSB.begin(115200);
  while(!SerialUSB);

  // initialize MassStorage
  SD.Init();

  // configure camera
  // Try to locate the camera
  if (cam.begin()) {
    SerialUSB.println("Camera ready");
  } else {
    SerialUSB.println("No camera found?");
    return;
  }

  // the camera takes a while to start up
  // wait a while before sending any configuration commands
  delay(500);
  
  cam.setImageSize(VC0706_160x120);
  cam.setMotionDetect(false);
  pd_rgb_led(PD_RED);

  uint8_t imgsize = cam.getImageSize();
  SerialUSB.print("Image size: ");
  if (imgsize == VC0706_640x480) SerialUSB.println("640x480");
  if (imgsize == VC0706_320x240) SerialUSB.println("320x240");
  if (imgsize == VC0706_160x120) SerialUSB.println("160x120");

  interpreter = new Interpreter(&SerialUSB);
  interpreter->registerCommand("help", &sh_help);
  interpreter->registerCommand("pwd", &sh_pwd);
  interpreter->registerCommand("cd", &sh_cd);
  interpreter->registerCommand("ls", &sh_ls);
  interpreter->registerCommand("mkdir", &sh_mkdir);
  interpreter->registerCommand("cat", &sh_cat);
  interpreter->registerCommand("rm", &sh_rm);
  interpreter->registerCommand("mv", &sh_mv);
  interpreter->registerCommand("takePicture", &cam_takePicture);
  interpreter->registerCommand("takeMotionDetect", &cam_motionDetect);
}

void loop() {
  interpreter->prompt();
}
