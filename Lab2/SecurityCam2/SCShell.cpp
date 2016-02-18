#include "SCShell.h"

#include "Arduino_Due_SD_HSCMI.h"
#include "FileUtils.h"

#define READ_CHUNK_SIZE 64

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
  int ret = 0;
  String *path = fUtils.combineName(pwd, target);
  if(SD.PathExists(path->c_str())){
    pwd.concat(target + "/");
    fUtils.setPWD(pwd);
  } else {
    out->println(target + " does not exist.");
    ret = -1;
  }
  delete path;
  return ret;
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

  // SD library filters out "." and ".."
  // print them here
  out->println(".");
  out->println("..");
  
  FileInfo fInfo;
  // list first file
  String pwd = fUtils.getPWD();
  if(SD.FindFirst(pwd.c_str(), fInfo)){
    out->println(fInfo.fileName);
  } else {
    // directory has no files
    return 0;
  }

  // list other files
  while(SD.FindNext(fInfo)){
    out->println(fInfo.fileName);
  }
  
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

  // if no params, do nothing
  if(command->getArgCount() == 0){
    out->println("Must specify directory name");
    return -1;
  }

  // only get first parameter
  String target = command->getArg(0);

  // check if directory exists
  int ret = 0;
  String *path = fUtils.combineName(fUtils.getPWD(), target);
  if(SD.PathExists(path->c_str())){
    out->println("Directory already exists");
    ret = -2;
  } else {
    SD.MakeDirectory(path->c_str());
  }
  delete path;
  
  return ret;
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

  // if no params, do nothing
  if(command->getArgCount() != 1){
    out->println("Must specify file to output");
    return -1;
  }

  // only get first parameter
  String pwd = fUtils.getPWD();
  String target = command->getArg(0);
  String *path = fUtils.combineName(pwd, target);

  // check if file exists
  int ret = 0;
  if(SD.FileExists(path->c_str())){
    FileStore fs = FileStore();
    fs.Init();
    if(fs.Open(NULL, path->c_str(), FILE_READ)){
      unsigned long fileLength = fs.Length();
      char buf[READ_CHUNK_SIZE];
      while(fileLength > 0){
        int bytesToRead = min(fileLength, READ_CHUNK_SIZE); 
        int bytesRead = fs.Read(buf, bytesToRead);
        out->write((const uint8_t *)buf, bytesRead);
        fileLength -= bytesRead;
      }
      fs.Close();
    } else {
      out->println("Cannot open file");
      ret = -2;
    }
  } else {
    out->println(target + " does not exist");
    ret = -1;
  }
  delete path;
  
  return ret;
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

  // if no params, do nothing
  if(command->getArgCount() == 0){
    return 0;
  }

  // only get first parameter
  String pwd = fUtils.getPWD();
  String target = command->getArg(0);
  String *path = fUtils.combineName(pwd, target);

  // check if file exists
  int ret = 0;
  if(SD.FileExists(path->c_str())){
    SD.Delete(NULL, path->c_str());
  } else {
    out->println("Cannot delete " + target);
    ret = -1;
  }
  delete path;
  
  return ret;
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

  // if no params, do nothing
  if(command->getArgCount() != 2){
    out->println("Must specify source and destination");
    return -1;
  }

  // only get first parameter
//  out->println("pwd " + fUtils.getPWD());
//  out->println("source " + command->getArg(0));
//  out->println("destination " + command->getArg(1));
//  return 0;
  String pwd = fUtils.getPWD();
  String *source = fUtils.combineName(pwd, command->getArg(0));
  String *destination = fUtils.combineName(pwd, command->getArg(1));
  
  // check if file exists
  int ret = 0;
  if(SD.FileExists(source->c_str())){
    SD.Rename(source->c_str(), destination->c_str());
  } else {
    out->println(command->getArg(0) + " does not exist");
    ret = -2;
  }

  delete source;
  delete destination;
  
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
