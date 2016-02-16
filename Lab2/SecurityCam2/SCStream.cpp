#include "SCStream.h"

SCSerialStream::SCSerialStream(Stream *stream){
  _stream = stream;
}

size_t SCSerialStream::print(char c[]){
  return _stream->print(c);
}

size_t SCSerialStream::println(char c[]){
  return _stream->println(c);
}

size_t SCSerialStream::print(String s){
  return _stream->print(s);
}

size_t SCSerialStream::println(String s){
  return _stream->println(s);
}

size_t SCSerialStream::write(const uint8_t *buffer, size_t size){
  return _stream->write(buffer, size);
}

SCQueueStream::SCQueueStream(QueueHandle_t streamQueue){
  _streamQueue = streamQueue;
}

size_t SCQueueStream::print(char c[]){
  return this->write((const uint8_t *)c, strlen(c));
}

size_t SCQueueStream::println(char c[]){
  size_t n = this->write((const uint8_t *)c, strlen(c));
  n += this->write((const uint8_t *)"\r\n", 2);
  return n;
}

size_t SCQueueStream::print(String s){
  return this->write((const uint8_t *)s.c_str(), s.length());
}

size_t SCQueueStream::println(String s){
  String s2 = s + "\r\n";
  return this->write((const uint8_t *)s2.c_str(), s2.length());
}

size_t SCQueueStream::write(const uint8_t *buffer, size_t size){
  SCQueueBuffer *qBuf = new SCQueueBuffer(buffer, size);
  xQueueSendToBack(_streamQueue, &qBuf, portMAX_DELAY);
  return qBuf->getSize();
}

SCQueueBuffer::SCQueueBuffer(const uint8_t *buffer, size_t size){
  if(size > 0){
    _buffer = new uint8_t[size];
    _size = size;
    memcpy(_buffer, buffer, size);
  } else {
    _buffer = NULL;
    _size = 0;
  }
}

SCQueueBuffer::~SCQueueBuffer(){
  if(_buffer != NULL){
    delete _buffer;  
  }
}

uint8_t *SCQueueBuffer::getBuffer(){
  return _buffer;
}

size_t SCQueueBuffer::getSize(){
  return _size;
}
