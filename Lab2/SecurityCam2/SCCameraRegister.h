#ifndef SCCAMERAREGISTRY_H
#define SCCAMERAREGISTRY_H

#include "SCProgramRegistry.h"

class SCCameraRegistry : public SCProgramRegistry {
  public:
    SCCameraRegistry();
    void registerProgram(SCProgram *program);

    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();
};

#endif //SCCAMERAREGISTRY_H
