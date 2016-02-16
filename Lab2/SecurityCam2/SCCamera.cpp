#include "SCCamera.h"

#include "Arduino_Due_SD_HSCMI.h"
#include "FileUtils.h"
#include "PowerDue.h"

SCCamera::SCCamera(Adafruit_VC0706 *cam){
  _cam = cam;
}

void SCCamera::Init(){
  pd_rgb_led_init();
  _cam->begin();
//  _imageDirectory = fUtils.combineName(ROOT_DIR, String("Cef/lab2images"));
  _cam->setImageSize(VC0706_160x120); // default image size
  this->setMotionDetect(true);
}

String SCCamera::getImageDirectory(){
  return _imageDirectory;
}

void SCCamera::setImageDirectory(String dir){
  _imageDirectory = dir;
}

void SCCamera::checkMotionDetected(){
  if(_cam->motionDetected()){
    this->takePicture();
  }
}

boolean SCCamera::setMotionDetect(boolean detect){
  if(_cam->setMotionDetect(detect)){
    pd_rgb_led(PD_BLUE);
    return true;
  } else {
    pd_rgb_led(PD_RED);
    return false;
  }
}

void SCCamera::takePicture(){
  this->setMotionDetect(false);
  pd_rgb_led(PD_GREEN);
  if(_cam->takePicture()){
      String *path = NULL;
      // Find unused filename to create new file
      char filename[14];
      strcpy(filename, "IMGSM0000.JPG");
      for (int i = 0; i < 100; i++) {
        filename[7] = '0' + i/10;
        filename[8] = '0' + i%10;
        path = fUtils.combineName(_imageDirectory, String(filename));
        // create if does not exist, do not open existing, write, sync after write
        if (! SD.FileExists(path->c_str())) {
          break;
        } else {
          delete path;
          path = NULL;
        }
      }

      // make sure path is valid
      if(path != NULL){
        // create file for writing
        FileStore theFile = FileStore();
        theFile.Init();
        if(theFile.Open(NULL, path->c_str(), FILE_WRITE)){
          // dump data into stream
          uint16_t jpglen = _cam->frameLength();
          while (jpglen > 0) {
            // read 32 bytes at a time;
            uint8_t *buffer;
            uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
            buffer = _cam->readPicture(bytesToRead); // request chunk from camera
            theFile.Write((char *)buffer, bytesToRead); // write chunk to SD card
            jpglen -= bytesToRead;
          }
          theFile.Close();
        }
        delete path;
      }

      _cam->resumeVideo();
  }
  pd_rgb_led(PD_BLUE);
  this->setMotionDetect(true);
}

boolean SCCamera::setImageSize(uint8_t size){
  return _cam->setImageSize(size);
}
