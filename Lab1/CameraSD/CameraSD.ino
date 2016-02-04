#include <Arduino.h>
#include <Adafruit_VC0706.h>

#include "Configuration.h"
#include "MassStorage.h"
#include "PowerDue.h"

#define FILE_WRITE true
#define FILE_READ false

// Initialize connection to Camera on Serial2
Adafruit_VC0706 cam = Adafruit_VC0706(&PDSerialCamera);

// Need to declare SD as global variable to be accessible inside MassStorage.cpp
MassStorage *SD = new MassStorage();

void setup() {
  // put your setup code here, to run once:

  // initialize Serial connection
  SerialUSB.begin(115200);
  pd_rgb_led_init();

  pd_rgb_led(PD_RED);

  // wait for Serial terminal to open
  while(!SerialUSB);
  SerialUSB.println("VC0706 Camera snapshot test");

  if(cam.begin()){
    SerialUSB.println("Camera found");
      pd_rgb_led(PD_BLUE);
  } else {
    SerialUSB.println("No camera found?");
    pd_rgb_led(PD_RED);   
    return;
  }
  
  // initialize files here...
  char directory[10] = "Cef";
  SD->Init();

  // Make sure the target directory exists
  if(!SD->PathExists(directory)){
    SD->MakeDirectory(directory);
  }
  
  // Find unused filename to create new file
  char filename[14];
  strcpy(filename, "IMGSM0000.JPG");
  for (int i = 0; i < 100; i++) {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD->FileExists(SD->CombineName(directory, filename))) {
      break;
    }
  }

  // create file for writing
  FileStore *theFile = new FileStore();
  theFile->Init();
  if(!theFile->Open(directory, filename, FILE_WRITE)){
    SerialUSB.println("Failed to open file for writing.");
    pd_rgb_led(PD_RED);
    return;
  }

  // setting picture sizes
  cam.setImageSize(VC0706_640x480);   //biggest
//  cam.setImageSize(VC0706_320x240);   //medium
//  cam.setImageSize(VC0706_160x120);   //small

  // better signal indicators than watching serial terminal
  SerialUSB.println("Snap in 3 secs...");
  pd_rgb_led(PD_BLUE);
  delay(500);
  pd_rgb_led(PD_OFF);
  delay(500);
  pd_rgb_led(PD_BLUE);
  delay(500);
  pd_rgb_led(PD_OFF);
  delay(500);
  pd_rgb_led(PD_BLUE);
  delay(500);
  pd_rgb_led(PD_OFF);

  // Isolate take picture operation in LED toggle window
  // to see power associated to just this operation
  delay(750);
  pd_rgb_led(PD_GREEN);
  if( !cam.takePicture() ){
    SerialUSB.println("Failed to snap!");
    pd_rgb_led(PD_RED); 
  } else {
    SerialUSB.println("Picture taken!");
  }  
  pd_rgb_led(PD_OFF);
  delay(750);

  uint16_t jpglen = cam.frameLength();
  SerialUSB.print("Storing ");
  SerialUSB.print(jpglen, DEC);
  SerialUSB.print(" byte image.");

  delay(500);
  pd_rgb_led(PD_BLUE);

  int32_t time = millis();
  // Read all the data up to # bytes!
  byte wCount = 0; // For counting # of writes
  while (jpglen > 0) {
    // read 32 bytes at a time;
    uint8_t *buffer;
    uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
    buffer = cam.readPicture(bytesToRead); // request chunk from camera
    theFile->Write((char *)buffer, bytesToRead); // write chunk to SD card
    jpglen -= bytesToRead;
  }
  
  theFile->Close();

  time = millis() - time;
  SerialUSB.println("done!");
  SerialUSB.print(time); Serial.println(" ms elapsed");
  pd_rgb_led(PD_RED);

  // END
}

void loop() {
}
