#include "SCCameraRegister.h"


SCCameraRegistry::SCCameraRegistry() : SCProgramRegistry() {

}

int SCCameraRegistry::execute(SCCommand *command, SCStream *in, SCStream *out){
  //shift out camera command and bring down arguments
  command->shift();
  
  // special command
  if(command->getCommand() == "camera" || 
      command->getCommand() == "help"){ 
      printCommandList(out);
      return 0;
  }
  
  for(int i=0; i < _registeredPrograms; i++){
    SCProgram *p = _registry[i];
    if(command->getCommand() == p->getCommand()){
      return p->execute(command, in, out);
    }
  }

  out->println("Unknown command: " + command->getCommand());

  return ERR_UNKNOWN_COMMAND;
}
    
String SCCameraRegistry::getCommand(){
  return "camera";
}
    
String SCCameraRegistry::getDescription(){
  return "sends commands to the camera driver. use 'camera help' for more commands.";
}

