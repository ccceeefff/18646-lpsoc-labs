#ifndef SCPROGRAM_H
#define SCPROGRAM_H

#include "SCCommand.h"

class SCProgram {
   public:
    virtual int execute(SCCommand *command, Stream *in, Stream *out);
    virtual String getCommand();
    virtual String getDescription();
};

#endif //SCPROGRAM_H
