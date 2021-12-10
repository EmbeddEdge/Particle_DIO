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
#include "MQTT-TLS.h"

const int      MAXRETRY          = 4;
const uint32_t msSAMPLE_INTERVAL = 2500;
const uint32_t msMETRIC_PUBLISH  = 300000; // 5 minutes
const uint32_t msSAMPLE_INTERVAL2 = 250;

// Button results 
int function = 0;
int lastFunction = 0;

int led1 = D7;
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
char     temp3[64];
char     temp4[64];
double   celsius;
bool     doorSwitch = false;
double   Vbatt;
int      pubInterval;
String   timeStamp;
uint32_t msLastMetric;
uint32_t msLastSample;

const char awsRootCert[] = "-----BEGIN CERTIFICATE-----\r\nMIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\r\nADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\r\nb24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\r\nMAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\r\nb3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\r\nca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\r\n9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\r\nIFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\r\nVOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\r\n93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\r\njgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\r\nAYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\r\nA4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\r\nU5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\r\nN+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\r\no/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\r\n5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\r\nrqXRfboQnoZsG4q5WTP468SQvvG5\r\n-----END CERTIFICATE-----";
const char awsClientCert[] = "-----BEGIN CERTIFICATE-----\r\nMIIDWjCCAkKgAwIBAgIVANvz/9+KxwnXu8hUFrstz8HUKE/cMA0GCSqGSIb3DQEB\r\nCwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\r\nIEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMTEwMTExMDE1\r\nMTJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\r\ndGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDJvMuZyyCmWTZp5lw6\r\n6lei6ZPu69RpZ3uxXelEsFGaA2yR6F1U9HsfMW1Q1Bh3mlYBykJJCjMoKFv8VA8A\r\nNjcjsfmDHW74ez8kPD7t+YdjAxZmxD80C57g7SWsbioqJ5ZMtZU3s3/QzlZ+QJp/\r\nCxJnWpVi0GFrLjmqWPRcN4t9nZ0rAjvuK4LvR6RkNikZgHZtGd9D+gKTEiZTWn9Q\r\n6oTiuePFAamdBwvMAMdhbH7MFX02Si1ZDOSYD/xvyoFC36CWVofbA9ZpWFn+p+5J\r\n32bzr3wyeaMcUNt0dXGHrJb0MJLsC1IiYAmyfu+BIlACVA7+eBP9GJ2MTf93SYwG\r\ne5GvAgMBAAGjYDBeMB8GA1UdIwQYMBaAFJTBlmBCwsuOmEy7wmv+9y82yY74MB0G\r\nA1UdDgQWBBTi+zaKkM1JW6638aMIIa+vaJATrDAMBgNVHRMBAf8EAjAAMA4GA1Ud\r\nDwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAUoMmh8eqry1NcHTo9/FslRvx\r\nO2Ff7m+1iS/1pWqB9aZycWHhP0H05YHunTXDnjTjvOYtcINGSehpkACw7dDvskoH\r\nEOBPzDiPfvHfLnBqiiW8twa+mhFopRoPv1tvybI9MnVUf/JZXzViLogLkbq9qo5U\r\n+nlbFrJWzIyacUGqSD0vEU3zCMBWfaFIFqKN9vFuzvENEXXEaqaIK64/wzcz5cPO\r\ne3qF+WOFICe/dKu+KN+dL1sG7NS6bbMG7p/rWLvW5kNfNuagkSwEpFiS9mlMtn/Q\r\nhDFnUPC3qvbbCo99iNtktB49EyOzhrAg7Tt/wF3kIadvO0TbFG2VS62j5A3x6g==\r\n-----END CERTIFICATE-----";
const char awsClientKey[] = "-----BEGIN RSA PRIVATE KEY-----\r\nMIIEpgIBAAKCAQEAybzLmcsgplk2aeZcOupXoumT7uvUaWd7sV3pRLBRmgNskehd\r\nVPR7HzFtUNQYd5pWAcpCSQozKChb/FQPADY3I7H5gx1u+Hs/JDw+7fmHYwMWZsQ/\r\nNAue4O0lrG4qKieWTLWVN7N/0M5WfkCafwsSZ1qVYtBhay45qlj0XDeLfZ2dKwI7\r\n7iuC70ekZDYpGYB2bRnfQ/oCkxImU1p/UOqE4rnjxQGpnQcLzADHYWx+zBV9Nkot\r\nWQzkmA/8b8qBQt+gllaH2wPWaVhZ/qfuSd9m8698MnmjHFDbdHVxh6yW9DCS7AtS\r\nImAJsn7vgSJQAlQO/ngT/RidjE3/d0mMBnuRrwIDAQABAoIBAQDEmyhZo+sX843i\r\nO0zbZhMuhDFWAaOrbdzQiNOTy7pbPMcMk8aRywpK5iLBSVLnDRSQ/gsHn9cnZd3v\r\n1gQgkrW+Je8jZwYFSCfLKqOxzSOyzQmB8fQexsx6LOQwXdTd7GLAM59O/7KrTsxg\r\nkpl/As1WyKMaADdQ+2XbrIHJdaCOM9g+yRvIRVswNPY7WqLLt1fMM/laI1tFzvS8\r\nafQ+VtR/iSlPm9c4CXXvgChlq5OekqkC6cgKNoPXaG6+F4T3rSq/Dq/b0elVPHzh\r\nJZSxHDePIO2FansqxFtd6fbfPKXWWunIXr3ZoGljL+OzS4UyJxTQk8DtjBUgG/pS\r\nFuHRDTBxAoGBAOZtz/FPHbO6ubxYGtA6PUQARJt4JLmM+Qszu5bRWGBC6UyvMb8s\r\nJj+fEdnUJW4WgSW1gNMvDnJ6gI+Ja7qzoQfH5E/tEKJLgODWxZP4UIHdWHxsQF93\r\nBXJEdsF2Z/sgIaUWN6W+jI+L1nYef+0Zj/RDxMULWLjyPqHB6IQ5sajlAoGBAOAf\r\n5UCzGyqsPq4EIpnuTOw0TZtn3gRwmftHAID5kP5OEvpGjfh0MbKwGnb71Iwo0gdd\r\nsjKGBtKTYeqVyA6nOH6sJZI0837dIo2lmgnfVqp9qOYc4TP6E6IP2i2vkUWqUpHk\r\nMJ8OsS3vDVLOYmHkeejq4EH19D6ftxChtdO8BcsDAoGBAMHsFJ4uSP7i3EEHnLk0\r\n6i56U0LL34xQeZ7V0L9iOw/3/8NLhYAvtZjBVBKQGVgGlHWZxGr2JSBuvBcQPZuI\r\nZLLZpOkmx2LuS+3skaavyYEhOMCtkZdF4U2DFLZH4cZUCjyW8AoKzNxZgQkzLdHq\r\nO14EUmrJ28b6B3+2Yuo0a7GJAoGBANxCnFQMcTtr0aHFEJhE2LYd5NNArbGhU8Le\r\nEYGvQq6Z3g62vhmZX6ojZa7faRBhoZ95loIOs6/JBia8EsNL1bqvxbC9dzuLacTZ\r\nJdxj8FFXc2DKqwwgzaCsgZf6fgOAEuur3ZjDoJ7q2yYxDAD2wFSGV+SbKfFNl2Sx\r\n9/bhRrQvAoGBAKZlcvwpySLe9a4YvCDU4xChP4SH0hDB3vE55r7yokW424Hl1vG9\r\nipp+jiaSVMD3xHnVV4MtyHOCGFRm4wTLpd5GCCtfZZpQHQ/RFDRPQsbyJMVL1Imn\r\n+gibwkSbnmHuEoH82I6lSp+/yrn/TxcPZqmBtBVRy+grEMnenwrh1DXp\r\n-----END RSA PRIVATE KEY-----";
//const char vrapCert[] = "-----BEGIN CERTIFICATE-----\r\nMIIDzzCCAregAwIBAgIBATANBgkqhkiG9w0BAQsFADBaMSIwIAYKCZImiZPyLGQBGRYSdDAwMDAw\r\nMDAwMi12cmFwdG9yMRAwDgYDVQQKDAdJb1Qubnh0MSIwIAYDVQQLDBlWaXJ0dWFsIFJhcHRvciBT\r\naWduaW5nIENBMB4XDTIxMDYxNDIxNTA1N1oXDTIzMDYxNDIxNTA1N1owWDEiMCAGCgmSJomT8ixk\r\nARkWEnQwMDAwMDAwMDItdnJhcHRvcjEQMA4GA1UECgwHSW9ULm54dDEgMB4GA1UEAwwXbXF0dC50\r\nMDAwMDAwMDAyLXZyYXB0b3IwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC8YHPmRND2\r\nRE2vIedyxqMZv8mc9m053YZZLHQzdpeF9Qjdi6SwqEpsa1luKZLtKqkPLIW77xRNDnDPBwyAKsen\r\nFXHQvkxS73zsZEk95XoWksGOF6GBivvngLzwlBE6E0g4S3L8y0Ga0RtMTuEwIjg7gnIu18MEilky\r\nDSKUi6+rlkEalAvIffEuIrlnjnW+kpbm2R3uOekDqt7Q7tRQ/JhZLwNujamNNVAW0RhAV+vvpn7H\r\n4JfaWI/BMpYmSl2kz0bctoniLlPDeaiEzEKnvBYv9InD0GhRfUL+JcSssiHx6F0/pFWKxSCLM9Fl\r\nUvNW8MHHBLHK9FPbOmCLINMrO/7ZAgMBAAGjgaEwgZ4wDgYDVR0PAQH/BAQDAgWgMAkGA1UdEwQC\r\nMAAwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMB0GA1UdDgQWBBSwWZXf5aVTAU6WExlU\r\nV26tspQlHjAfBgNVHSMEGDAWgBToIqHp8NBBI4QGTrMOVWvC9swr3jAiBgNVHREEGzAZghdtcXR0\r\nLnQwMDAwMDAwMDItdnJhcHRvcjANBgkqhkiG9w0BAQsFAAOCAQEAti50LvxRD8xmUPHnltTSJj2/\r\nFnYykFdLD1N7u8ExJ1t/qL/z6/F4dk1ddHOTi7tBeuBCQJ0iyOuhbBveBuDhtMadKxuyKI7d5XUK\r\nDa5gLUnXUN9vvRaPbuYoJEf5AVnVdGMjzXlG360uBkR0vjb6P+Bb2SvODxaCsT7/pqFq54BVDV5U\r\n8hiTXa0QjPEQuuZdAqd1oG156pF7qBCbmtW7f632nuHvmRqTKAbq81exWd9I16KfXnAb7XERu0jB\r\nnx5aVILP8bzb/bgEh5q3IVdRF+PYtuiJ13oEIj9bbTonQq0xiv4C30yuhfgVD+b/yShKjRrxvkG8\r\nyCKHKMAGY3XG5w==\r\n-----END CERTIFICATE-----";

void callback(char* topic, byte* payload, unsigned int length);

MQTT client("a2pmxes33f0p7y-ats.iot.us-west-2.amazonaws.com",8883, callback);
//MQTT client("iotnxt-mqttbroker.iotnxt.io",8883, callback);

void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);
}


// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
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

  client.enableTls(awsRootCert, sizeof(awsRootCert),awsClientCert, sizeof(awsClientCert), awsClientKey, sizeof(awsClientKey));
  //client.enableTls(vrapCert, sizeof(vrapCert));
  client.connect("Particle_2","rac_mothership","iA4gM9qO1zB1oR9e");
  client.publish("rac_mothership/nightjar/nightjarSub/environment", "temp");


}

// loop() runs over and over again, as quickly as it can execute.
void loop() {

  // Update button state
  button1.Update();

  //Update LED based on Button input
  //updateLED();

  //Update the switch state to publish
  updateSwitchState();
  
  // Sample the temperature sensor on a timer interval
  sampleTemp();

  // Sample the temperature sensor on a timer interval
  sampleVbat();

  sampleDateTime();

  pubInterval = minuteConversion(msMETRIC_PUBLISH);

  // Publish the data on a timer interval
  updateTelemetry();
  if (client.isConnected()){
    client.loop();
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
    //Serial.println(celsius);
  }
  else {
    Serial.println("Invalid reading");
  }
  msLastSample = millis();
}

void sampleTemp(){
  if (millis() - msLastSample >= msSAMPLE_INTERVAL){
    getTemp();
  }
}

void sampleVbat(){
  if (millis() - msLastSample >= msSAMPLE_INTERVAL){
    double voltage = analogRead(BATT) * 0.0011224;
    Serial.print("Battery voltage: ");
    Serial.println(voltage);
    Vbatt = voltage;
  }
}

void sampleDateTime(){
  if (millis() - msLastSample >= msSAMPLE_INTERVAL){
    Time.zone(+2);
    Serial.print("Timestamp: ");
    Serial.println(Time.timeStr());
    timeStamp = Time.timeStr();
  }
}

int minuteConversion(uint32_t p_MS){
  int minutes;
  minutes = p_MS/60000; //milliseconds divided by 60 000 give minute value
  return minutes;
}

void publishData(){
  int str_len = timeStamp.length() + 1;
  char tempTS[str_len];
  sprintf(temp, "%2.2f", celsius);
  sprintf(temp2, "%d", doorSwitch);
  sprintf(temp3, "%1.2f", Vbatt);
  sprintf(temp4, "%d", pubInterval);
  timeStamp.toCharArray(tempTS, str_len);
  Particle.publish("Publish_TimeStamp", tempTS, PRIVATE);
  Particle.publish("Publish_Chamber_Temperature", temp, PRIVATE);
  Particle.publish("Publish_Chamber_BoolState", temp2, PRIVATE);
  Particle.publish("Publish_Chamber_DeviceBattery", temp3, PRIVATE);
  Particle.publish("Publish_Interval", temp4, PRIVATE);
  delay(30000);
  msLastMetric = millis();
}

void updateTelemetry(){
  if (millis() - msLastMetric >= msMETRIC_PUBLISH){
    Serial.println("Publishing now.");
    publishData();
    if(client.isConnected()){
      Serial.println("Client connected");
      client.loop();
      client.publish("rac_mothership/nightjar/nightjarSub/environment", "80");
      //Particle.publish("connection", "connected", PRIVATE);
    }
    else{
      /*
      int ret;
        if((ret = client.enableTls(vrapCert, sizeof(vrapCert)))<0){
                       Serial.printlnf("client.enableTls failed with code: %d", ret);
                       };

      Serial.println("tls enable");

      // connect to the server
      if ( (ret >= 0) && ((ret = client.connect("Particle_2","rac_mothership","iA4gM9qO1zB1oR9e")) < 0) ) {    // if client enable tls succeeded, attempt connect.  Can block for up to 60 seconds
                  Serial.printlnf("client.connect failed with code: %d", ret);
              }
      */
      Serial.println("Client not connected");
      Particle.publish("connection", "not connected", PRIVATE);
      client.connect("Particle_2","rac_mothership","iA4gM9qO1zB1oR9e");
    }
  }

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