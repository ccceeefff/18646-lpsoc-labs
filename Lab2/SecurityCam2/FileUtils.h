#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <Arduino.h>
#include <FreeRTOS_ARM.h>

class FileUtils {
  public:
    FileUtils();
    void Init();
    void setPWD(String pwd);
    String getPWD();

    // mutex protected version of SD library
    String *combineName(String directory, String filename);

  private:
    String _pwd;
    SemaphoreHandle_t _xSemaphore;
};

extern String ROOT_DIR;
extern FileUtils fUtils;

#endif //FILEUTILS_H
