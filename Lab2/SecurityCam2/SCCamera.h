#ifndef SCCAMERA_H
#define SCCAMERA_H

#include "Adafruit_VC0706.h"

class SCCamera {
  public:
    SCCamera(Adafruit_VC0706 *cam);
    void Init();

    String getImageDirectory();
    void setImageDirectory(String dir);

    void takePicture();
    void checkMotionDetected();
    boolean setMotionDetect(boolean detect);
    
    boolean setImageSize(uint8_t size);

  private:
    Adafruit_VC0706 *_cam;
    String _imageDirectory;
  
};

#endif //SCCAMERA_H
