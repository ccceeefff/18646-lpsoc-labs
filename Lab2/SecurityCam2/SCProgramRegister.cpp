#include "SCProgramRegistry.h"


SCProgramRegistry::SCProgramRegistry(){
  for(int i=0; i < MAX_COMMAND_COUNT; i++){
    _registry[i] = NULL;  
  }
  _registeredPrograms = 0;
}

SCProgramRegistry::~SCProgramRegistry(){
  for(int i=0; i < _registeredPrograms; i++){
    delete _registry[i];
  }
}

void SCProgramRegistry::registerProgram(SCProgram *program){
  _registry[_registeredPrograms] = program;
  _registeredPrograms++;
}

int SCProgramRegistry::execute(SCCommand *command, Stream *in, Stream *out){
  // special command
  if(command->getCommand() == "help"){ 
      printCommandList(out);
      return 0;
  }
  
  for(int i=0; i < _registeredPrograms; i++){
    SCProgram *p = _registry[i];
    if(command->getCommand() == p->getCommand()){
      return p->execute(command, in, out);
    }
  }

  return ERR_UNKNOWN_COMMAND;
}

void SCProgramRegistry::printCommandList(Stream *out){
  out->println("List of available commands:");
  out->println("\thelp - print this list of commands");
  for(int i=0; i < _registeredPrograms; i++){
    SCProgram *p = _registry[i];
    out->print("\t");
    out->print(p->getCommand());
    out->print(" - ");
    out->print(p->getDescription());
    out->println("");
  }
}
    
String SCProgramRegistry::getCommand(){
  return "";
}
    
String SCProgramRegistry::getDescription(){
  return "";
}

