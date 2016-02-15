#include "SCCommand.h"

SCCommand::SCCommand(String line){
  parse(line);
}

void SCCommand::parse(String line){
  int space = line.indexOf(" ");
  if(space != 0){
    _command = line.substring(0, space);
  } else {
    _command = String(line);
  }

  // remove command from line
  line.remove(0, _command.length());
  line.trim();
  
  // extract arguments
  _argCount = 0;
  space = 0;
  while( (line.length() > 0) && _argCount < MAX_ARG_COUNT ){
    space = line.indexOf(" ");
    if(space != 0){
      _args[_argCount] = line.substring(0, space);  
    } else {
      _args[_argCount] = String(line);
    }
    line.remove(0, _args[_argCount].length());
    _argCount++;
    line.trim();
  }
}

String SCCommand::getCommand(){
  return _command;  
}

int SCCommand::getArgCount(){
  return _argCount;
}
    
String SCCommand::getArg(int argIndex){
  if(argIndex < _argCount){
    return _args[argIndex];
  }
  return String();
}

