/*
 * Project Particle_Dio
 * Description: A tester to connect digital inputs to an online dashboard
 * Author: Turyn Lim Banda
 * Date: 15/03/2021
 * 
 * 
 */
#include <DS18B20.h>
#include <clickButton.h>
#include <math.h>

const int      MAXRETRY          = 4;
const uint32_t msSAMPLE_INTERVAL = 2500;
const uint32_t msMETRIC_PUBLISH  = 240000;
const uint32_t msSAMPLE_INTERVAL2 = 250;

// Button results 
int function = 0;
int lastFunction = 0;

int led1 = D8;
int led2 = D8;
//int but1 = D2;
int but2 = D12;
const int16_t dsData = A1;
volatile int buttonState = LOW;
// the Button
const int but1 = D2;
ClickButton button1(but1, LOW, CLICKBTN_PULLUP);



// Sets Pin A0 as data pin and the only sensor on bus
DS18B20  ds18b20(dsData, true); 

char     temp[64];
char     temp2[64];
double   celsius;
bool     doorSwitch = false;
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
  //attachInterrupt(but1, buttonUpdate, RISING);

  // We are also going to declare a Particle.function so that we can turn the LED on and off from the cloud.
  Particle.function("led",ledToggle);
  // This is saying that when we ask the cloud for the function "led", it will employ the function ledToggle() from this app.

  // For good measure, let's also make sure both LEDs are off when we start:
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);

  // Setup button timers (all in milliseconds / ms)
  // (These are default if not set, but changeable for convenience)
  button1.debounceTime   = 20;   // Debounce timer in ms
  button1.multiclickTime = 250;  // Time limit for multi clicks
  button1.longClickTime  = 1000; // time until "held-down clicks" register

    delay(1000);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() 
{

  // Update button state
  button1.Update();

  //Update LED based on Button input
  //updateLED();

  //Update the switch state to publish
  updateSwitchState();
  
  // Sample the temperature sensor on a timer interval
  if (millis() - msLastSample >= msSAMPLE_INTERVAL){
    getTemp();
  }

  // Publish the data on a timer interval
  if (millis() - msLastMetric >= msMETRIC_PUBLISH){
    Serial.println("Publishing now.");
    publishData();
  }
  
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

void getTemp()
{
  float _temp;
  int   i = 0;

  do {
    _temp = ds18b20.getTemperature();
  } while (!ds18b20.crcCheck() && MAXRETRY > i++);

  if (i < MAXRETRY) 
  {
    celsius = _temp;
    Serial.println(celsius);
  }
  else {
    Serial.println("Invalid reading");
  }
  msLastSample = millis();
}

void publishData()
{
  sprintf(temp, "%2.2f", celsius);
  sprintf(temp2, "%d", doorSwitch);
  Particle.publish("Publish_Chamber_Temperature", temp, PRIVATE);
  Particle.publish("Publish_Chamber_BoolState", temp2, PRIVATE);
  msLastMetric = millis();
}

void buttonUpdate()
{
  buttonState = !buttonState;
}

void updateLED()
{
  if(buttonState==HIGH)
  {
    digitalWrite(led2, HIGH);
  }
  else
  {
    digitalWrite(led2, LOW);
  }
}

void buttonStatePrint()
{
  // Save click codes in LEDfunction, as click codes are reset at next Update()
  if(button1.clicks != 0) function = button1.clicks;
  if (function != 0){
    Serial.print("Button Press Type: ");
    Serial.println(function);
    function = 0;
  }
}

void updateSwitchState()
{
  // Save click codes in LEDfunction, as click codes are reset at next Update()
  if(button1.clicks != 0) function = button1.clicks;
  if (function == 1){
    doorSwitch = !doorSwitch;
    Serial.print("doorSwitch State: ");
    Serial.println(doorSwitch);
  }
  function = 0;
}