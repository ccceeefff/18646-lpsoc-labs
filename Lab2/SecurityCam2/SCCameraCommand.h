#ifndef SCCAMERACOMMAND_H
#define SCCAMERACOMMAND_H

#include <Arduino.h>
#include <FreeRTOS_ARM.h>

#include "SCProgram.h"

#define CAMERA_SET_LOGGING 0x01
#define CAMERA_TAKE_PICTURE 0x02
#define CAMERA_SET_IMAGE_SIZE 0x03
#define CAMERA_SET_MOTION_DETECT 0x04
#define CAMERA_SET_IMAGE_DIRECTORY 0x05
#define CAMERA_SET_TV_ENABLED 0x06

class SCCameraCommand {
  public:
    SCCameraCommand(uint8_t command);
    SCCameraCommand(uint8_t command, String parameter);

    uint8_t getCommand();
    String getParameter();

  private:
    uint8_t _command;
    String _parameter;
};

class SCCamera_log : public SCProgram {
  public:
    SCCamera_log(QueueHandle_t commandQueue);
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();

  private:
    QueueHandle_t _commandQueue;
};

class SCCamera_snapshot : public SCProgram {
  public:
    SCCamera_snapshot(QueueHandle_t commandQueue);
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();

  private:
    QueueHandle_t _commandQueue;
};

class SCCamera_imageSize : public SCProgram {
  public:
    SCCamera_imageSize(QueueHandle_t commandQueue);
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();

  private:
    QueueHandle_t _commandQueue;
};

class SCCamera_motionDetect : public SCProgram {
  public:
    SCCamera_motionDetect(QueueHandle_t commandQueue);
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();

  private:
    QueueHandle_t _commandQueue;
};

class SCCamera_cd : public SCProgram {
  public:
    SCCamera_cd(QueueHandle_t commandQueue);
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();

  private:
    QueueHandle_t _commandQueue;
};

class SCCamera_tv : public SCProgram {
  public:
    SCCamera_tv(QueueHandle_t commandQueue);
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();

  private:
    QueueHandle_t _commandQueue;
};

#endif //SCCAMERACOMMAND_H
