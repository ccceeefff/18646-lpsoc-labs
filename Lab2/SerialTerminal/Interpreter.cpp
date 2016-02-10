#include "Interpreter.h"

Interpreter::Interpreter(Stream *stream)
{
    _stream = stream;
    _argc = 0;
    for(int i=0; i < MAX_ARG_COUNT; i++){
      _argv[i] = "";
    }
    _inputBuffer = "";
    _registeredCommands = 0;
}

void Interpreter::prompt()
{
  _stream->print("cmd> ");
  while(!_stream->available());
  _inputBuffer = _stream->readStringUntil('\n');
  if(_inputBuffer.length() > 0){
    _inputBuffer.trim();
    // echo the entered command
    _stream->println(_inputBuffer);
    this->parse(_inputBuffer);
  }
}

void Interpreter::registerCommand(String command, commandBlock execBlock)
{
  /*
   * Ideally we should check if command was registered previously, but
   * let's not worry about that this time
   */

   // add command to map
   _commands[_registeredCommands] = command;
   _execBlocks[_registeredCommands] = execBlock;
   _registeredCommands++;
}

void Interpreter::parse(String line)
{
  //_stream->println(line);
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
  _argc = 0;
  int start = 0;
  space = 0;
  while( (start < line.length()) && _argc < MAX_ARG_COUNT ){
    space = line.indexOf(" ", space);
    _argv[_argc] = &args[start];
    _argc++;
    if(space != -1){ // more arguments later
      ((char *)args)[space] = '\0'; // add null terminator here
      start = space = space+1;
    } else { // we've reached the end
      break;
    }
  }

  this->execute(command, _argv, _argc);
}

void Interpreter::execute(String command, const char *params[], int paramCount)
{
  /*
   * Find registered command and execute with params
   */
 
  for(int i=0; i < _registeredCommands; i++){
    if(_commands[i] == command){
      _execBlocks[i](_stream, params, paramCount);
      return;
    }
  }

  // command was not found
  _stream->println("Unknown command: '" + command + "'");
}

