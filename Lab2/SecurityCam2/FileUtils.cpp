#include "FileUtils.h"

#include "Arduino_Due_SD_HSCMI.h"

String ROOT_DIR = "/";

FileUtils::FileUtils(){
}

void FileUtils::Init(){  
  _pwd = String(ROOT_DIR);
  _xSemaphore = xSemaphoreCreateCounting(1,1);
}

void FileUtils::setPWD(String pwd){
  _pwd = pwd;
}
    
String FileUtils::getPWD(){
  return _pwd;
}

String FileUtils::combineName(String directory, String filename){
  String out;
  xSemaphoreTake(_xSemaphore, portMAX_DELAY);
  out = String(SD.CombineName(directory.c_str(), filename.c_str()));
  xSemaphoreGive(_xSemaphore);
  return out;
}

FileUtils fUtils = FileUtils();
