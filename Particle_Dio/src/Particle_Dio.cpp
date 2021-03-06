/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Git/Particle_DIO/Particle_Dio/src/Particle_Dio.ino"
/*
 * Project Particle_Dio
 * Description: A tester to connect digital inputs to an online dashboard
 * Author: Turyn Lim Banda
 * Date: 15/03/2021
 * 
 * 
 */
#include <DS18B20.h>
#include <math.h>

void setup();
void loop();
int ledToggle(String command);
void getTemp();
void publishData();
#line 12 "c:/Git/Particle_DIO/Particle_Dio/src/Particle_Dio.ino"
const int      MAXRETRY          = 4;
const uint32_t msSAMPLE_INTERVAL = 5000;
const uint32_t msMETRIC_PUBLISH  = 30000;

int led1 = D2;
int led2 = D3;
int but1 = D11;
int but2 = D12;
const int16_t dsData = A0;

// Sets Pin A0 as data pin and the only sensor on bus
DS18B20  ds18b20(dsData, true); 

char     temp[64];
double   celsius;
uint32_t msLastMetric;
uint32_t msLastSample;

// setup() runs once, when the device is first turned on.
void setup() 
{
    Serial.begin(115200);
    // Put initialization like pinMode and begin functions here.
    // Here's the pin configuration, same as last time
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(but1, INPUT_PULLUP);
    pinMode(but2, INPUT_PULLUP);

    // We are also going to declare a Particle.function so that we can turn the LED on and off from the cloud.
    Particle.function("led",ledToggle);
    // This is saying that when we ask the cloud for the function "led", it will employ the function ledToggle() from this app.

    // For good measure, let's also make sure both LEDs are off when we start:
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);

    delay(1000);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() 
{
  // The core of your code will likely live here.
  digitalWrite(led1, HIGH);   // Turn ON the LED

  if (millis() - msLastSample >= msSAMPLE_INTERVAL){
    getTemp();
  }

  if (millis() - msLastMetric >= msMETRIC_PUBLISH){
    Serial.println("Publishing now.");
    publishData();
  }

   // Delay between measurements.
  //delay(delayMS);

  //Read the temperature from the DHT11
  //String temp = readTemp();
  //String temp = String(random(-5, 50));

  //String temp = String(random(-10, 60));
  //Particle.publish("Publish_Chamber_Temperature", temp, PRIVATE);

  //delay(30000);               // Wait for 30 seconds

  //digitalWrite(led1, LOW);    // Turn OFF the LED
  //delay(30000);  
  
}

// We're going to have a super cool function now that gets called when a matching API request is sent
// This is the ledToggle function we registered to the "led" Particle.function earlier.

int ledToggle(String command) {
    /* Particle.functions always take a string as an argument and return an integer.
    Since we can pass a string, it means that we can give the program commands on how the function should be used.
    In this case, telling the function "on" will turn the LED on and telling it "off" will turn the LED off.
    Then, the function returns a value to us to let us know what happened.
    In this case, it will return 1 for the LEDs turning on, 0 for the LEDs turning off,
    and -1 if we received a totally bogus command that didn't do anything to the LEDs.
    */

    if (command=="on") {
        //digitalWrite(led1,HIGH);
        digitalWrite(led2,HIGH);
        return 1;
    }
    else if (command=="off") {
        //digitalWrite(led1,LOW);
        digitalWrite(led2,LOW);
        return 0;
    }
    else {
        return -1;
    }
}

void getTemp(){
  float _temp;
  int   i = 0;

  do {
    _temp = ds18b20.getTemperature();
  } while (!ds18b20.crcCheck() && MAXRETRY > i++);

  if (i < MAXRETRY) {
    celsius = _temp;
    Serial.println(celsius);
  }
  else {
    Serial.println("Invalid reading");
  }
  msLastSample = millis();
}

void publishData(){
  sprintf(temp, "%2.2f", celsius);
  Particle.publish("Publish_Chamber_Temperature", temp, PRIVATE);
  msLastMetric = millis();
}