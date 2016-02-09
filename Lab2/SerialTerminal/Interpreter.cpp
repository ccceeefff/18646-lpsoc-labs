#include "Interpreter.h"

Interpreter::Interpreter(Stream *stream)
{
    _stream = stream;
    _argc = 0;
    for(int i=0; i < MAX_ARG_COUNT; i++){
      _argv[i] = "";
    }
    _inputBuffer = "";
}

void Interpreter::prompt()
{
  _stream->print("cmd> ");
  while(!_stream->available());
  _inputBuffer = _stream->readStringUntil('\n');
  if(_inputBuffer.length() > 0){
    _inputBuffer.trim();
    this->parse(_inputBuffer);
  }
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
  _stream->println("command: " + command);
  if(paramCount > 0){
    _stream->println("parameters: ");
    for(int i=0; i<paramCount; i++){
      _stream->println(params[i]);
    }
  }
}

