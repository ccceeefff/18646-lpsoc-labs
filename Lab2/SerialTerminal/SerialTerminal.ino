#define MAX_ARG_COUNT 5
String inputBuffer = "";
const char *argv[MAX_ARG_COUNT]; // support up to 5 parameters
int argc = 0;

void interpret(String command, const char *params[], int paramCount){
  SerialUSB.println("command: " + command);
  if(paramCount > 0){
    SerialUSB.println("parameters: ");
    for(int i=0; i<paramCount; i++){
      SerialUSB.println(params[i]);
    }
  }
}

void parse(String line){
  SerialUSB.println(line);
  int space = line.indexOf(" ");
  String command;
  if(space != 0){
    command = line.substring(0, space);
  } else {
    command = line;
  }

  // remove command from line
  line.remove(0, command.length());
  line.trim();
  
  // extract arguments
  const char *args = line.c_str();
  argc = 0;
  int start = 0;
  space = 0;
  while( (start < line.length()) && argc < MAX_ARG_COUNT){
    space = line.indexOf(" ", space);
    argv[argc] = &args[start];
    argc++;
    if(space != -1){ // more arguments later
      ((char *)args)[space] = '\0'; // add null terminator here
      start = space = space+1;
    } else { // we've reached the end
      break;
    }
  }

  interpret(command, argv, argc);
}

void setup() {
  // setup SerialUSB
  SerialUSB.begin(115200);
  while(!SerialUSB);
}

void loop() {
  SerialUSB.print("cmd> ");
  while(!SerialUSB.available());
  inputBuffer = SerialUSB.readStringUntil('\n');
  if(inputBuffer.length() > 0){
    inputBuffer.trim();
    parse(inputBuffer);
  }
}
