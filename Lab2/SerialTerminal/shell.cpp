#include "shell.h"

String PWD = "/";

void sh_help(Stream *stream, const char *params[], int paramCount){
  stream->println("List of available commands:");
  stream->println("\tpwd - prints the current working directory");
  stream->println("\tcd - change directory");
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
  stream->println("Not yet implemented");
}

void sh_ls(Stream *stream, const char *params[], int paramCount){
  stream->println("Not yet implemented");
}

void sh_mkdir(Stream *stream, const char *params[], int paramCount){
  stream->println("Not yet implemented");
}

void sh_cat(Stream *stream, const char *params[], int paramCount){
  stream->println("Not yet implemented");
}

void sh_rm(Stream *stream, const char *params[], int paramCount){
  stream->println("Not yet implemented");
}

void sh_mv(Stream *stream, const char *params[], int paramCount){
  stream->println("Not yet implemented");
}

