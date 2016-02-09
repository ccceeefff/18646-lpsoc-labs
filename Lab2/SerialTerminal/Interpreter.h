#include <Arduino.h>

#define MAX_ARG_COUNT 5

class Interpreter {
  public:
    Interpreter(Stream *stream);
    void prompt();
   
  protected:
    void parse(String line);
    void execute(String command, const char *params[], int paramCount);

  private: 
    Stream *_stream;
    String _inputBuffer;
    const char *_argv[MAX_ARG_COUNT];
    int _argc;
};
