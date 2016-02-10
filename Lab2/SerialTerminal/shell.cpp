#include "shell.h"

#include "Arduino_Due_SD_HSCMI.h"

#define READ_CHUNK_SIZE 64

const String ROOT_DIR = "/";
String PWD = "/";

void sh_help(Stream *stream, const char *params[], int paramCount){
  stream->println("List of available commands:");
  stream->println("\tpwd - prints the current working directory");
  stream->println("\tcd - change directory (assumes one level at a time)");
  stream->println("\tls - list contents of directory");
  stream->println("\tcat - print contents of file");
  stream->println("\trm - delete file from filesystem");
  stream->println("\tmkdir - create directory in filesystem");
  stream->println("\tmv - move or rename file");
  stream->println("\thelp - print this list of commands");
}

void sh_pwd(Stream *stream, const char *params[], int paramCount){
  stream->println(PWD);
}

void sh_cd(Stream *stream, const char *params[], int paramCount){
  // if no params were given, go back to root "/"
  if(paramCount == 0){
    PWD = String(ROOT_DIR);
    return;
  }

  // get target directory
  String target = String(params[0]); // only honor first parameter

  // special cases "." and ".."
  if(target == "."){
    // do nothing, keep current directory
    return;
  }

  if(target == ".."){
    // look for second to last "/"
    int level = PWD.lastIndexOf("/", PWD.length()-2);
    if(level > 0){ // only pop if not in root directory
      PWD.remove(level+1);
    } else { // reset to root
      PWD = String(ROOT_DIR);
    }
    return;
  }
  
  // check if path exists from PWD and then update PWD
  if(SD.PathExists(PWD.c_str(), target.c_str())){
    PWD.concat(target + "/");
  } else {
    stream->println(target + " does not exist.");
  }
}

void sh_ls(Stream *stream, const char *params[], int paramCount){
  
  // SD library filters out "." and ".."
  // print them here
  stream->println(".");
  stream->println("..");

  FileInfo fInfo;
  // list first file
  if(SD.FindFirst(PWD.c_str(), fInfo)){
    stream->println(fInfo.fileName);
  } else {
    // directory has no files
    return;
  }

  // list other files
  while(SD.FindNext(fInfo)){
    stream->println(fInfo.fileName);
  }
  
}

void sh_mkdir(Stream *stream, const char *params[], int paramCount){
  // if no params, do nothing
  if(paramCount == 0){
    stream->println("Nothing to create");
    return;
  }

  // only get first parameter
  String target = String(params[0]);

  // check if directory exists
  if(SD.PathExists(PWD.c_str(), target.c_str())){
    stream->println("Directory already exists");
  } else {
    SD.MakeDirectory(PWD.c_str(), target.c_str());
  }
}

void sh_cat(Stream *stream, const char *params[], int paramCount){
  // if no params, do nothing
  if(paramCount != 1){
    stream->println("Must specify file to output");
    return;
  }

  // only get first parameter
  String target = String(params[0]);

  // check if file exists
  if(SD.FileExists(SD.CombineName(PWD.c_str(), target.c_str()))){
    FileStore fs = FileStore();
    fs.Init();
    if(fs.Open(PWD.c_str(), target.c_str(), FILE_READ)){
      unsigned long fileLength = fs.Length();
      char buf[READ_CHUNK_SIZE];
      while(fileLength > 0){
        int bytesToRead = min(fileLength, READ_CHUNK_SIZE); 
        int bytesRead = fs.Read(buf, bytesToRead);
        stream->write(buf, bytesRead);
        fileLength -= bytesRead;
      }
      fs.Close();
    } else {
      stream->println("Cannot open file");
    }
  } else {
    stream->println(target + " does not exist");
  }
}

void sh_rm(Stream *stream, const char *params[], int paramCount){
  // if no params, do nothing
  if(paramCount == 0){
    return;
  }

  // only get first parameter
  String target = String(params[0]);

  // check if file exists
  if(SD.FileExists(SD.CombineName(PWD.c_str(), target.c_str()))){
    SD.Delete(PWD.c_str(), target.c_str());
  } else {
    stream->println("Cannot delete " + target);
  }
}

void sh_mv(Stream *stream, const char *params[], int paramCount){
  // if no params, do nothing
  if(paramCount != 2){
    stream->println("Must specify source and destination");
    return;
  }

  // only get first parameter
  String source = String(params[0]);
  String destination = String(params[1]);

  // check if file exists
  if(SD.FileExists(SD.CombineName(PWD.c_str(), source.c_str()))){
    source = SD.CombineName(PWD.c_str(), source.c_str());
    destination = SD.CombineName(PWD.c_str(), destination.c_str());
    SD.Rename(source.c_str(), destination.c_str());
  } else {
    stream->println(source + " does not exist");
  }
}

