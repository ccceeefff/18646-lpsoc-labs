#ifndef SCSHELL_H
#define SCSHELL_H

#include "SCProgram.h"

class SCShell_pwd : public SCProgram {
  public:
    SCShell_pwd();
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();
};

class SCShell_cd : public SCProgram {
  public:
    SCShell_cd();
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();
};

class SCShell_ls : public SCProgram {
  public:
    SCShell_ls();
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();
};

class SCShell_mkdir : public SCProgram {
  public:
    SCShell_mkdir();
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();
};

class SCShell_cat : public SCProgram {
  public:
    SCShell_cat();
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();
};

class SCShell_rm : public SCProgram {
  public:
    SCShell_rm();
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();
};

class SCShell_mv : public SCProgram {
  public:
    SCShell_mv();
    int execute(SCCommand *command, SCStream *in, SCStream *out);
    String getCommand();
    String getDescription();
};

#endif //SCSHELL_H
