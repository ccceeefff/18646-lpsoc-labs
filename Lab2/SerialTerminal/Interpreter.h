#include <Arduino.h>

#define MAX_ARG_COUNT 5
#define MAX_COMMAND_COUNT 10

typedef void (*commandBlock)(Stream *stream, const char *params[], int paramCount);

class Interpreter {
  public:
    Interpreter(Stream *stream);
    void prompt();
    void registerCommand(String command, commandBlock execBlock);
   
  protected:
    void parse(String line);
    void execute(String command, const char *params[], int paramCount);

  private: 
    Stream *_stream;
    String _inputBuffer;
    const char *_argv[MAX_ARG_COUNT];
    int _argc;

    String _commands[MAX_COMMAND_COUNT];
    commandBlock _execBlocks[MAX_COMMAND_COUNT];
    int _registeredCommands;
};
