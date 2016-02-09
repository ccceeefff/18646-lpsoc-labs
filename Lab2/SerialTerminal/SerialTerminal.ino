String command = "";

void setup() {
   // setup SerialUSB
   SerialUSB.begin(115200);
   while(!SerialUSB);
}

void loop() {
  SerialUSB.print("cmd> ");
  while(!SerialUSB.available());
  command = SerialUSB.readStringUntil('\n');
  if(command.length() > 0){
    SerialUSB.println(command);
  }
}
