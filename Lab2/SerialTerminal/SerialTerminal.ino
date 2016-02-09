#include "Interpreter.h"

Interpreter *interpreter;

void setup() {
  // setup SerialUSB
  SerialUSB.begin(115200);
  while(!SerialUSB);

  interpreter = new Interpreter(&SerialUSB);
}

void loop() {
  interpreter->prompt();
}
