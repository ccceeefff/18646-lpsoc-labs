#ifndef SCPROGRAM_H
#define SCPROGRAM_H

#include "SCCommand.h"
#include "SCStream.h"

class SCProgram {
   public:
    virtual int execute(SCCommand *command, SCStream *in, SCStream *out);
    virtual String getCommand();
    virtual String getDescription();
};

#endif //SCPROGRAM_H
