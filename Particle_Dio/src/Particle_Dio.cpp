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
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

void setup();
void loop();
int ledToggle(String command);
String readTemp();
#line 13 "c:/Git/Particle_DIO/Particle_Dio/src/Particle_Dio.ino"
#define DHTPIN            A0         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 11 

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

int led1 = D2;
int led2 = D3;
int but1 = D11;
int but2 = D12;
//int Pin_DHT11_Data = A0;
  

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

   dht.begin();
    Serial.println("DHTxx Unified Sensor Example");
    // Print temperature sensor details.
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.println("Temperature");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
    Serial.println("------------------------------------");
    // Print humidity sensor details.
    dht.humidity().getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.println("Humidity");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
    Serial.println("------------------------------------");
    // Set delay between sensor readings based on sensor details.
    delayMS = sensor.min_delay / 100;

}

// loop() runs over and over again, as quickly as it can execute.
void loop() 
{
  // The core of your code will likely live here.
  digitalWrite(led1, HIGH);   // Turn ON the LED

  
   // Delay between measurements.
  //delay(delayMS);

  //Read the temperature from the DHT11
  String temp = readTemp();

  //String temp = String(random(-10, 60));
  Particle.publish("Publish_Chamber_Temperature", temp, PRIVATE);
  delay(30000);               // Wait for 30 seconds

  digitalWrite(led1, LOW);    // Turn OFF the LED
  delay(30000);  
  
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

String readTemp()
{
    float f_Temperature; 
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) 
    {
        String s_Temperature = String(random(-10, -5));
        Serial.println("Error reading temperature!");
        return s_Temperature;
    }
    else 
    {
        f_Temperature = event.temperature; 
        String s_Temperature = String(f_Temperature);
        Serial.print("Temperature: ");
        Serial.print(event.temperature);
        Serial.println(" *C");
        return s_Temperature;
    } 
}