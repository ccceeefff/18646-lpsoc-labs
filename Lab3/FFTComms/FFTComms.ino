//#include <SparkFunESP8266Client.h>
//#include <SparkFunESP8266Server.h>
#include <SparkFunESP8266WiFi.h>

#include <arduinoFFT.h>
#include <FreeRTOS_ARM.h>
#include <IPAddress.h>

#include "FFTAudioSampler.h"
#include "FFTConfig.h"

#if NET_SUBMIT
ESP8266Client client;
#else
#define client SerialUSB
#endif

QueueHandle_t xAudioBufferQueue;
QueueHandle_t xSenderQueue;

arduinoFFT FFT = arduinoFFT();

char wifi_ssid[] = "CMU-SV";            // your network SSID (name)
char wifi_pass[] = "";        // your network password
//char wifi_ssid[] = "Nexus5";            // your network SSID (name)
//char wifi_pass[] = "password";        // your network password

char server_host[] = "10.0.15.226";
int server_port = 80;

IPAddress server_ip;

#if LOCAL_FFT
char server_endpoint[] = "/CefRamirez/fft";
#else
char server_endpoint[] = "/CefRamirez/signal";
#endif

double vReal[NUM_SAMPLES];
double vImag[NUM_SAMPLES];

void buffer_ready(double *audioSample, int bufferSize){
  BaseType_t xHigherPriorityTaskWoken;
  AudioSampler.stopSampling();
  xQueueSendToBackFromISR(xAudioBufferQueue, &audioSample, &xHigherPriorityTaskWoken);
}

static void Task_FFT(void *arg){
  double *audioBuffer = NULL;
  while(1){
    if(xQueueReceive(xAudioBufferQueue, &audioBuffer, portMAX_DELAY)){
      if(audioBuffer != NULL){
        //copy data over to fft buffer
        for(int i=0; i < NUM_SAMPLES; i++){
          vReal[i] = audioBuffer[i];
          vImag[i] = 0;
        }
        
        // compute FFT
#if LOCAL_FFT
          FFT.Windowing(vReal, NUM_SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
          FFT.Compute(vReal, vImag, NUM_SAMPLES, FFT_FORWARD); /* Compute FFT */
          FFT.ComplexToMagnitude(vReal, vImag, NUM_SAMPLES); /* Compute magnitudes */
          double x = FFT.MajorPeak(vReal, NUM_SAMPLES, SAMPLING_RATE);
          SerialUSB.print("major peak: ");
          SerialUSB.println(x);
#else
          SerialUSB.println("Buffer full.. skipping FFT computation..");          
#endif

        // transmit packet
        xQueueSendToBack(xSenderQueue, &vReal, portMAX_DELAY);
      }
      audioBuffer = NULL;
    }
  }
}

static void Task_Sender(void *arg){
  double *values = NULL;
  while(1){
    if(xQueueReceive(xSenderQueue, &values, portMAX_DELAY)){

    int status = esp8266.status();
    SerialUSB.print("status: ");
    SerialUSB.println(status);
            
      SerialUSB.println("Sending packet....");
      String head = "";
      String packet = "";

       // send json encoded data
      packet.concat("{\"data\":[");
      for(int i=0; i < NUM_SAMPLES; i++){
        packet.concat("[");
        packet.concat(i);
        packet.concat(",");
        packet.concat(vReal[i]);
        packet.concat("]");
        if(i < NUM_SAMPLES-1){
          packet.concat(",");
        }
      }
      packet.concat("]");
#if !LOCAL_FFT
      packet.concat(", \"sampleRate\": ");
      packet.concat(SAMPLING_RATE);
#endif
      packet.concat("}");
      packet.concat("\r\n");
      
#if NET_SUBMIT
        int retval = client.connect(server_ip, server_port);
        if( retval > 0){
          // send post request headers
          SerialUSB.println("Connected to server...");
          head.concat("POST ");
          head.concat(server_endpoint);
          head.concat(" HTTP/1.0");
          head.concat("\r\n");
          head.concat("Content-Type: application/json");
          head.concat("\r\n");
//          head.concat("Transfer-Encoding: chunked");
//          head.concat("\r\n");
          head.concat("\r\n");
#endif         

          SerialUSB.print(head);
//          SerialUSB.print(packet.length(), HEX);
//          SerialUSB.print("\r\n");
          SerialUSB.print(packet);
//          SerialUSB.print("0\r\n");
//          SerialUSB.print("\r\n");
          client.print(head);
//          client.print(packet.length(), HEX);
//          client.print("\r\n");
          client.print(packet);
//          client.print("0\r\n");
//          client.print("\r\n");
      
#if NET_SUBMIT
          client.stop();

          // flush response
          while(client.available()){
            char c = client.read();
            SerialUSB.write(c);
          }

          if(client.connected()){
            SerialUSB.println();
            SerialUSB.println("Disconnecting from server...");
          }
        } // close client connection
        else {
          SerialUSB.println("Failed to connect to server.");
          SerialUSB.println(retval);
        }
#endif
      AudioSampler.startSampling(SAMPLING_RATE);
      
    }
  }
}

void setup() {
  SerialUSB.begin(115200);
  while(!SerialUSB);
  SerialUSB.println("Ready!");

  server_ip.fromString(server_host);
  SerialUSB.print("Server IP: ");
  SerialUSB.println(server_ip);

#if NET_SUBMIT
  initializeWiFi();
#else
  client = SerialUSB;  
#endif

  AudioSampler.setBufferReadyCallback(buffer_ready);

  xAudioBufferQueue = xQueueCreate(AUDIO_BUFFER_QUEUE_SIZE, sizeof(double *));
  xSenderQueue = xQueueCreate(SENDER_QUEUE_SIZE, sizeof(double *));

  xTaskCreate(Task_Sender, NULL, 400, NULL, 0, NULL);
  xTaskCreate(Task_FFT, NULL, 400, NULL, 0, NULL);

  SerialUSB.println("Tasks ready!");

  // // start scheduler
  SerialUSB.println("Start!");

  AudioSampler.startSampling(SAMPLING_RATE);
  vTaskStartScheduler();
  
  SerialUSB.println("Insufficient RAM\n");
  while(1);
}

void loop() {
  // put your main code here, to run repeatedly:
}

/* Utility Methods */
void initializeWiFi()
{
  int test = esp8266.begin();
  if (test != true)
  {
    SerialUSB.println(F("Error talking to ESP8266."));
    errorLoop(test);
  }
  SerialUSB.println(F("ESP8266 Shield Present"));
  esp8266.reset();
  esp8266.setMux(1);
  // The ESP8266 can be set to one of three modes:
  //  1 - ESP8266_MODE_STA - Station only
  //  2 - ESP8266_MODE_AP - Access point only
  //  3 - ESP8266_MODE_STAAP - Station/AP combo
  // Use esp8266.getMode() to check which mode it's in:
  int retVal = esp8266.getMode();
  if (retVal != ESP8266_MODE_STA)
  { // If it's not in station mode.
    // Use esp8266.setMode([mode]) to set it to a specified
    // mode.
    retVal = esp8266.setMode(ESP8266_MODE_STA);
    if (retVal < 0)
    {
      SerialUSB.println(F("Error setting mode."));
      errorLoop(retVal);
    }
  }
  SerialUSB.println(F("Mode set to station"));

  // esp8266.status() indicates the ESP8266's WiFi connect
  // status.
  // A return value of 1 indicates the device is already
  // connected. 0 indicates disconnected. (Negative values
  // equate to communication errors.)
//  retVal = esp8266.status();
//  if (retVal <= 0)
//  {
    SerialUSB.print(F("Connecting to "));
    SerialUSB.println(wifi_ssid);
    // esp8266.connect([ssid], [psk]) connects the ESP8266
    // to a network.
    // On success the connect function returns a value >0
    // On fail, the function will either return:
    //  -1: TIMEOUT - The library has a set 30s timeout
    //  -3: FAIL - Couldn't connect to network.
    retVal = esp8266.connect(wifi_ssid, wifi_pass);
    if (retVal < 0)
    {
      SerialUSB.println(F("Error connecting"));
      errorLoop(retVal);
    }
//  }

  printWifiStatus();
}

void printWifiStatus()
{
 char connectedSSID[24];
  memset(connectedSSID, 0, 24);
  // esp8266.getAP() can be used to check which AP the
  // ESP8266 is connected to. It returns an error code.
  // The connected AP is returned by reference as a parameter.
  int retVal = esp8266.getAP(connectedSSID);
  if (retVal > 0)
  {
    SerialUSB.print(F("Connected to: "));
    SerialUSB.println(connectedSSID);
  }

  // esp8266.localIP returns an IPAddress variable with the
  // ESP8266's current local IP address.
  IPAddress myIP = esp8266.localIP();
  SerialUSB.print(F("My IP: ")); SerialUSB.println(myIP);

//  while(1){
//    int status = esp8266.status();
//    SerialUSB.print("status: ");
//    SerialUSB.println(status);
//    int retval = esp8266.ping("10.0.16.1");
//    SerialUSB.print("ping: ");
//    SerialUSB.println(retval);
//    delay(1000);
//  }
}

// errorLoop prints an error code, then loops forever.
void errorLoop(int error)
{
  SerialUSB.print(F("Error: ")); SerialUSB.println(error);
  SerialUSB.println(F("Looping forever."));
  for (;;)
    ;
}
