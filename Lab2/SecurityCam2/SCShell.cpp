#include "SCShell.h"

#include "Arduino_Due_SD_HSCMI.h"
#include "FileUtils.h"

/******************
 *  SCShell_pwd 
 ******************/

SCShell_pwd::SCShell_pwd(){
  
}
    
int SCShell_pwd::execute(SCCommand *command, SCStream *in, SCStream *out){
  out->println(fUtils.getPWD());
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
    
int SCShell_cd::execute(SCCommand *command, SCStream *in, SCStream *out){
  
  if(command->getArgCount() == 0){
    fUtils.setPWD(ROOT_DIR);
    return 0;
  }

  // get target directory
  String target = command->getArg(0); // only honor first parameter
  String pwd = fUtils.getPWD();

  // special cases "." and ".."
  if(target == "."){
    // do nothing, keep current directory
    return 0;
  }

  if(target == ".."){
    // look for second to last "/"
    int level = pwd.lastIndexOf("/", pwd.length()-2);
    if(level > 0){ // only pop if not in root directory
      pwd.remove(level+1);
    } else { // reset to root
      pwd = String(ROOT_DIR);
    }
    fUtils.setPWD(pwd);
    return 0;
  }

  // check if path exists from PWD and then update PWD
  if(SD.PathExists(fUtils.combineName(pwd, target).c_str())){
    pwd.concat(target + "/");
    fUtils.setPWD(pwd);
  } else {
    out->println(target + " does not exist.");
    return -1;
  }
  
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
    
int SCShell_ls::execute(SCCommand *command, SCStream *in, SCStream *out){
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
    
int SCShell_mkdir::execute(SCCommand *command, SCStream *in, SCStream *out){
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
    
int SCShell_cat::execute(SCCommand *command, SCStream *in, SCStream *out){
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
    
int SCShell_rm::execute(SCCommand *command, SCStream *in, SCStream *out){
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
    
int SCShell_mv::execute(SCCommand *command, SCStream *in, SCStream *out){
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
