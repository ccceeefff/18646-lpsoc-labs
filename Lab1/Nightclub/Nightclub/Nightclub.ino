/*
 * Nightclub - example to demonstrate thread definition, semphores and thread sleep.
 */

#include "FreeRTOS_ARM.h"
#include "basic_io_arm.h"

// The LED is attached to pin 13 on Arduino
const uint8_t LED_PIN = 13;

// Declare a semaphore handle.
SemaphoreHandle_t Bouncer;

//------------------------------------------------------------------------------------
/*
 * Thread Guest, It start in the line, gets in, dances, and then leaves.
 */
 // Declare the thread function for thread guest.
static void Guest(void *arg){
  while(1){

    // Wait to enter the nightclub
    vPrintStringAndNumber("Guest is waiting to enter nightclub: ", (long)arg);
    xSemaphoreTake(Bouncer, portMAX_DELAY);
   
    // enter and do some dancing
    vPrintStringAndNumber("Guest is doing some dancing: ", (long)arg);
    vTaskDelay((random(500)+10000 * configTICK_RATE_HZ) / 1000L);
    
    // leave the nightclub
    vPrintStringAndNumber("Guest is leaving the nightclub: ", (long)arg);
    xSemaphoreGive(Bouncer);

    // stay at home
    while(1);
  }
}

//------------------------------------------------------------------------------------

void setup() {  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  // SerialUSB -> if using the native USB port
  // Serial -> if using the programming port
  // Serial USB does not need a Baudrate
  // Serial needs a Baudrate set
  Serial.begin(115200);
  // Wait for Serial Monitor to be connected
  while(!Serial);

  digitalWrite(LED_PIN, HIGH);
  vPrintString("Serial USB connected\n");

  // Put a Bouncer at the door and only let 4 people enter at a time
  Bouncer = xSemaphoreCreateCounting(4, 4);

  randomSeed(analogRead(0));

  char names[][4] = {"12", "24", "36", "48", "60", "72", "84", "96", "108", "120", "132", "144", "156", "168", "180", "192", "204", "216", "228", "240"};
  // put 20 people in line for the night club
  for(int i=1; i <= 20; i++){
    // Let each guest enter on their own thread
    xTaskCreate(Guest, names[i-1], configMINIMAL_STACK_SIZE, (void *)i, 0, NULL);
  }
  
  // check for creation errors
  if( Bouncer == NULL ){
    vPrintString("Bouncer failed to show up for work today.\n");
    while(1);
  } else {
    vPrintString("Bouncer ready to boogie\n");
  }

  // start scheduler
  vTaskStartScheduler();

  // If something wrong
  vPrintString("Insufficient RAM\n");
  while(1);
}

void loop() {
  // not used in FreeRTOS
}
