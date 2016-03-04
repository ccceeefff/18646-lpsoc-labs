#ifndef FFTCONFIG_H
#define FFTCONFIG_H

#define NUM_BUFFERS 2
#define NUM_SAMPLES 256

#define SAMPLING_RATE 1000

#define ADC_PORT 0

#define AUDIO_BUFFER_QUEUE_SIZE 5
#define SENDER_QUEUE_SIZE 5

#define LOCAL_FFT 0

/*
 * WiFi ESP8266 Properties
 */
#define NET_SUBMIT 1
#define HTTP_TRANSFER_ENCODING_CHUNKED 1
 
#define PDWiFiSerial Serial1
#define PDWiFiSerialBaudRate 1

#endif //FFTCONFIG_H
