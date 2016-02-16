#ifndef SCPROGRAMREGISTRY_H
#define SCPROGRAMREGISTRY_H

#include "SCProgram.h"

#define MAX_COMMAND_COUNT 10

#define ERR_UNKNOWN_COMMAND -99

class SCProgramRegistry : public SCProgram {
  public:
    SCProgramRegistry();
    ~SCProgramRegistry();
    void registerProgram(SCProgram *program);

    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();

  protected:
    void printCommandList(SCStream *out);

  private:
    SCProgram *_registry[MAX_COMMAND_COUNT];
    int _registeredPrograms;
};

#endif //SCPROGRAMREGISTRY_H
