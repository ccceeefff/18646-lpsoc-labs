void setup() {
  // put your setup code here, to run once:
    analogWriteResolution(8);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=1; i <= 4; i++){
    analogWrite(DAC0, 0);
    delayMicroseconds(250);
    analogWrite(DAC0, (i*32 + 64));
    delayMicroseconds(750);
  }
  
}

