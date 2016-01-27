/*
 * Nightclub - example to demonstrate thread definition, semphores and thread sleep.
 */

#include "FreeRTOS_ARM.h"

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

    Serial.print("Guest ");
    Serial.print((int)arg);
    Serial.println(" is waiting to enter nightclub.");
    // Wait to enter the nightclub
    xSemaphoreTake(Bouncer, portMAX_DELAY);
   
    // enter and do some dancing
    Serial.print("Guest ");
    Serial.print((int)arg);
    Serial.println(" is doing some dancing.");
    vTaskDelay((random(500)+10000 * configTICK_RATE_HZ) / 1000L);
    
    // leave the nightclub
    Serial.print("Guest ");
    Serial.print((int)arg);
    Serial.println(" is leaving the nightclub");
    xSemaphoreGive(Bouncer);

    // stay at home
    while(1);
  }
}

//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//  Guest Variations
//------------------------------------------------------------------------------------
/*
 * These threads are guests with different levels of dancing effort to simulate
 * the power variations between each guest
 */

/*
 * Slow Int Dancer - dances then rests for a bit
 */
static void SlowIntDancerGuest(void *arg){
  while(1){
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is waiting to enter nightclub.");
    // Wait to enter the nightclub
    xSemaphoreTake(Bouncer, portMAX_DELAY);
   
    // enter and do some dancing
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is doing some dancing.");

    int x = 1;
    for(int i=0; i < 100000; i++){
      x += random(20);
    }
    
    // leave the nightclub
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is leaving the nightclub");
    xSemaphoreGive(Bouncer);
    
    // wait at home for some time
//    vTaskDelay((random(500) * configTICK_RATE_HZ)/1000L);
  }
}

/*
 * Fast Int Dancer - dances continually
 */
static void FastIntDancerGuest(void *arg){
  while(1){
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is waiting to enter nightclub.");
    // Wait to enter the nightclub
    xSemaphoreTake(Bouncer, portMAX_DELAY);
   
    // enter and do some dancing
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is doing some dancing.");

    int x = 1;
    for(int i=0; i < 100000; i++){
      x = (x + random(20)) / (random(4) + 1);
    }
    
    // leave the nightclub
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is leaving the nightclub");
    xSemaphoreGive(Bouncer);
    
    // wait at home for some time
//    vTaskDelay((random(500) * configTICK_RATE_HZ)/1000L);
  }
}

/*
 * Slow Float Dancer - dances then rests for a bit
 */
static void SlowFloatDancerGuest(void *arg){
  while(1){
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is waiting to enter nightclub.");
    // Wait to enter the nightclub
    xSemaphoreTake(Bouncer, portMAX_DELAY);
   
    // enter and do some dancing
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is doing some dancing.");

    float x = 1.0;
    for(int i=0; i < 100000; i++){
      x += random(20);
    }
    
    // leave the nightclub
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is leaving the nightclub");
    xSemaphoreGive(Bouncer);
    
    // wait at home for some time
//    vTaskDelay((random(500) * configTICK_RATE_HZ)/1000L);
  }
}

/*
 * Fast Float Dancer - dances continually
 */
static void FastFloatDancerGuest(void *arg){
  while(1){
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is waiting to enter nightclub.");
    // Wait to enter the nightclub
    xSemaphoreTake(Bouncer, portMAX_DELAY);
   
    // enter and do some dancing
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is doing some dancing.");

    float x = 1.0;
    for(int i=0; i < 100000; i++){
      x = (x + random(20)) / (random(4) + 1.0);
    }
    
    // leave the nightclub
    Serial.print("Slow Dancer ");
    Serial.print((int)arg);
    Serial.println(" is leaving the nightclub");
    xSemaphoreGive(Bouncer);
    
    // wait at home for some time
//    vTaskDelay((random(500) * configTICK_RATE_HZ)/1000L);
  }
}

//------------------------------------------------------------------------------------

void setup() {
  // put your setup code here, to run once:

  analogWriteResolution(8);
  
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
  Serial.println("Serial USB connected");

  // Put a Bouncer at the door and only let 4 people enter at a time
  Bouncer = xSemaphoreCreateCounting(4, 4);

  randomSeed(analogRead(0));

//  // put 20 people in line for the night club
//  for(int i=1; i <= 20; i++){
//    // Let each guest enter on their own thread
//    xTaskCreate(Guest, (char *)(i*12), configMINIMAL_STACK_SIZE, (void *)i, 0, NULL);
//  }

  // create 4 dancers of different levels
  xTaskCreate(SlowIntDancerGuest, "96", configMINIMAL_STACK_SIZE, (void *)1, 1, NULL);
  xTaskCreate(FastIntDancerGuest, "128", configMINIMAL_STACK_SIZE, (void *)2, 1, NULL);
  xTaskCreate(SlowFloatDancerGuest, "160", configMINIMAL_STACK_SIZE, (void *)3, 1, NULL);
  xTaskCreate(FastFloatDancerGuest, "192", configMINIMAL_STACK_SIZE, (void *)4, 1, NULL);

  // check for creation errors
  if( Bouncer == NULL ){
    Serial.println("Bouncer failed to show up for work today.");
    while(1);
  } else {
    Serial.println("Bouncer ready to boogie");
  }

  // start scheduler
  vTaskStartScheduler();

  // If something wrong
  Serial.println("Insufficient RAM");
  while(1);
}

void loop() {
  // not used in FreeRTOS
}
