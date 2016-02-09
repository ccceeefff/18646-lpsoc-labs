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
  SerialUSB.print("You said: ");
  SerialUSB.println(command);
}
