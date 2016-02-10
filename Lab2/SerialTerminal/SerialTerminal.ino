#include "Interpreter.h"

#include "Arduino_Due_SD_HSCMI.h"
#include "shell.h"

Interpreter *interpreter;

// Note: need to create re-entrant version of SD library

void setup() {

  // initialize MassStorage
  SD.Init();
  
  // setup SerialUSB
  SerialUSB.begin(115200);
  while(!SerialUSB);

  interpreter = new Interpreter(&SerialUSB);
  interpreter->registerCommand("help", &sh_help);
  interpreter->registerCommand("pwd", &sh_pwd);
  interpreter->registerCommand("cd", &sh_cd);
  interpreter->registerCommand("ls", &sh_ls);
  interpreter->registerCommand("mkdir", &sh_mkdir);
  interpreter->registerCommand("cat", &sh_cat);
  interpreter->registerCommand("rm", &sh_rm);
  interpreter->registerCommand("mv", &sh_mv);
}

void loop() {
  interpreter->prompt();
}
