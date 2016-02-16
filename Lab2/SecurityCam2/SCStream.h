#ifndef SCSTREAM_H
#define SCSTREAM_H

#include <Arduino.h>

#include <FreeRTOS_ARM.h>

// small wrapper for stream objects for conveniently writing to queues
class SCStream {
  public:
    virtual size_t print(String s);
    virtual size_t print(char c[]);
    virtual size_t println(String s);
    virtual size_t println(char c[]);
    virtual size_t write(const uint8_t *buffer, size_t size);
};

class SCSerialStream : public SCStream {
  public:
    SCSerialStream(Stream *stream);
    size_t print(String s);
    size_t print(char c[]);
    size_t println(String s);
    size_t println(char c[]);
    size_t write(const uint8_t *buffer, size_t size);

  private:
    Stream *_stream;
};

class SCQueueStream : public SCStream {
  public: 
    SCQueueStream(QueueHandle_t streamQueue);
    size_t print(String s);
    size_t print(char c[]);
    size_t println(String s);
    size_t println(char c[]);
    size_t write(const uint8_t *buffer, size_t size);

  private:
    QueueHandle_t _streamQueue;
};

class SCQueueBuffer {
  public:
    SCQueueBuffer(const uint8_t *buffer, size_t size);
    ~SCQueueBuffer();
    uint8_t *getBuffer();
    size_t getSize();
    
  private:
    uint8_t *_buffer;
    size_t _size;
};

#endif //SCSTREAM_H
