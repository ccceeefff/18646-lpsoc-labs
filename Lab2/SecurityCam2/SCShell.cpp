#include "SCShell.h"

/******************
 *  SCShell_pwd 
 ******************/

SCShell_pwd::SCShell_pwd(){
  
}
    
int SCShell_pwd::execute(SCCommand *command, Stream *in, Stream *out){
  return 0;
}

String SCShell_pwd::getCommand(){
  return "pwd";
}
    
String SCShell_pwd::getDescription(){
  return "prints the current working directory";
}

/******************
 * END SCShell_pwd 
 ******************/

/******************
 *  SCShell_cd
 ******************/

SCShell_cd::SCShell_cd(){
  
}
    
int SCShell_cd::execute(SCCommand *command, Stream *in, Stream *out){
  return 0;
}

String SCShell_cd::getCommand(){
  return "cd";
}
    
String SCShell_cd::getDescription(){
  return "change the current working directory";
}

/******************
 * END SCShell_cd 
 ******************/

/******************
 *  SCShell_ls 
 ******************/

SCShell_ls::SCShell_ls(){
  
}
    
int SCShell_ls::execute(SCCommand *command, Stream *in, Stream *out){
  return 0;
}

String SCShell_ls::getCommand(){
  return "ls";
}
    
String SCShell_ls::getDescription(){
  return "list contents of directory";
}

/******************
 * END SCShell_ls 
 ******************/

/******************
 *  SCShell_mkdir 
 ******************/

SCShell_mkdir::SCShell_mkdir(){
  
}
    
int SCShell_mkdir::execute(SCCommand *command, Stream *in, Stream *out){
  return 0;
}

String SCShell_mkdir::getCommand(){
  return "mkdir";
}
    
String SCShell_mkdir::getDescription(){
  return "create new directory";
}


/******************
 * END SCShell_mkdir 
 ******************/

/******************
 *  SCShell_cat 
 ******************/

SCShell_cat::SCShell_cat(){
  
}
    
int SCShell_cat::execute(SCCommand *command, Stream *in, Stream *out){
  return 0;
}

String SCShell_cat::getCommand(){
  return "cat";
}
    
String SCShell_cat::getDescription(){
  return "print contents of file";
}

/******************
 * END SCShell_cat 
 ******************/

/******************
 *  SCShell_rm 
 ******************/

SCShell_rm::SCShell_rm(){

}
    
int SCShell_rm::execute(SCCommand *command, Stream *in, Stream *out){
  return 0;
}

String SCShell_rm::getCommand(){
  return "rm";
}
    
String SCShell_rm::getDescription(){
  return "delete file";
}

/******************
 * END SCShell_rm 
 ******************/

/******************
 *  SCShell_mv 
 ******************/

SCShell_mv::SCShell_mv(){
  
}
    
int SCShell_mv::execute(SCCommand *command, Stream *in, Stream *out){
  return 0;
}

String SCShell_mv::getCommand(){
  return "mv";
}
    
String SCShell_mv::getDescription(){
  return "move or rename file";
}

/******************
 * END SCShell_mv 
 ******************/
