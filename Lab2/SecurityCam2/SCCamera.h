#ifndef SCCAMERA_H
#define SCCAMERA_H

#include "Adafruit_VC0706.h"
#include "SCStream.h"

class SCCamera {
  public:
    SCCamera(Adafruit_VC0706 *cam);
    void Init(SCStream *outStream);

    String getImageDirectory();
    void setImageDirectory(String dir);

    void takePicture();
    void checkMotionDetected();
    boolean setMotionDetect(boolean detect);
    
    boolean setImageSize(uint8_t size);

    void setLogging(boolean enable);

  private:
    Adafruit_VC0706 *_cam;
    String _imageDirectory;
    boolean _logging;  
    SCStream *_outStream;
};

#endif //SCCAMERA_H
