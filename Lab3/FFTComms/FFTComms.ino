#include <arduinoFFT.h>
#include <FreeRTOS_ARM.h>
#include <WiFiEsp.h>

#include "FFTAudioSampler.h"
#include "FFTConfig.h"

#if NET_SUBMIT
WiFiEspClient client;
#else
#define client SerialUSB
#endif

QueueHandle_t xAudioBufferQueue;
QueueHandle_t xSenderQueue;

arduinoFFT FFT = arduinoFFT();

char wifi_ssid[] = "CMU-SV";            // your network SSID (name)
char wifi_pass[] = "";        // your network password

char server_host[] = "10.0.15.226";
int server_port = 80;

#if LOCAL_FFT
char server_endpoint[] = "/CefRamirez/fft";
#else
char server_endpoint[] = "/CefRamirez/signal";
#endif

double vReal[NUM_SAMPLES];
double vImag[NUM_SAMPLES];

void buffer_ready(double *audioSample, int bufferSize){
  BaseType_t xHigherPriorityTaskWoken;
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
      SerialUSB.println("Sending packet....");
#if NET_SUBMIT
        if(client.connect(server_host, server_port)){
          // send post request headers
          SerialUSB.println("Connected to server...");
          client.print("POST ");
          client.print(server_endpoint);
          client.println(" HTTP/1.1");
          client.println("Content-Type: application/json");
          client.println("Connection: close");
          client.println();
#endif         
          // send json encoded data
          client.print("{\"data\":[");
          for(int i=0; i < NUM_SAMPLES; i++){
            client.print("[");
            client.print(i);
            client.print(",");
            client.print(vReal[i]);
            client.print("]");
            if(i < NUM_SAMPLES-1){
              client.print(",");
            }
          }
          client.print("]");
#if !LOCAL_FFT
          client.print(", \"sampleRate\": ");
          client.print(SAMPLING_RATE);
#endif
          client.println("}");
          client.println();
          
#if NET_SUBMIT
          // flush response
          while(client.available()){
            char c = client.read();
            SerialUSB.write(c);
          }

          if(!client.connected()){
            SerialUSB.println();
            SerialUSB.println("Disconnecting from server...");
            client.stop();
          }
        } // close client connection
        else {
          SerialUSB.println("Failed to connect to server.");
        }
#endif
      
    }
  }
}

void setup() {
  SerialUSB.begin(115200);
  while(!SerialUSB);
  SerialUSB.println("Ready!");

#if NET_SUBMIT
  initializeWiFi();
#else
  client = SerialUSB;  
#endif

  AudioSampler.setBufferReadyCallback(buffer_ready);

  xAudioBufferQueue = xQueueCreate(AUDIO_BUFFER_QUEUE_SIZE, sizeof(double *));
  xSenderQueue = xQueueCreate(SENDER_QUEUE_SIZE, sizeof(double *));

  xTaskCreate(Task_Sender, NULL, configMINIMAL_STACK_SIZE, NULL, 0, NULL);
  xTaskCreate(Task_FFT, NULL, configMINIMAL_STACK_SIZE, NULL, 0, NULL);

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
  PDWiFiSerial.begin(PDWiFiSerialBaudRate);
  WiFi.init(&PDWiFiSerial);

  if (WiFi.status() == WL_NO_SHIELD){
    SerialUSB.println("WiFi shield not present");
    while(true);
  }

  int status = WL_IDLE_STATUS;
  
  while(status != WL_CONNECTED){
    SerialUSB.print("Attempting to connect to WPA SSID: ");
    SerialUSB.println(wifi_ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(wifi_ssid, wifi_pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  printWifiStatus();
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  SerialUSB.print("SSID: ");
  SerialUSB.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  SerialUSB.print("IP Address: ");
  SerialUSB.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  SerialUSB.print("Signal strength (RSSI):");
  SerialUSB.print(rssi);
  SerialUSB.println(" dBm");
}
