#ifndef SCCOMMAND_H
#define SCCOMMAND_H

#include <Arduino.h>

#define MAX_ARG_COUNT 5

class SCCommand {
  public:
    SCCommand(String line);

    String getCommand();
    int getArgCount();
    String getArg(int argIndex);

    // shift the command out and move in the first argument
    void shift();

  protected:
    void parse(String line);

  private:
    String _command;
    String _args[MAX_ARG_COUNT];
    int _argCount;
};

#endif //SCCOMMAND_H
