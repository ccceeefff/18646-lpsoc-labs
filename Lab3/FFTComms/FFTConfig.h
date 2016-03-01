#ifndef FFTCONFIG_H
#define FFTCONFIG_H

#define NUM_BUFFERS 2
#define NUM_SAMPLES 16

#define SAMPLING_RATE 100

#define ADC_PORT 0

#define AUDIO_BUFFER_QUEUE_SIZE 5
#define SENDER_QUEUE_SIZE 5

#define LOCAL_FFT 1

/*
 * WiFi ESP8266 Properties
 */
#define NET_SUBMIT 0
 
#define PDWiFiSerial Serial1
#define PDWiFiSerialBaudRate 9600

#endif //FFTCONFIG_H
