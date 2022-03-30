#include <Arduino.h>
#include <EEPROM.h>
// Our Files
#include "PWM.h"
#include "LightSensor.h"
#include "connectWifi.h"

uint8_t PWMCounter = 50;
int PWMDir = 1;

void setup() {
  Serial.begin(9600);
  // // Set up PWM
  // setupPWM(0, 5000, 8, 26);
  // ////////////////////////////////////////////////
  // // Set up Light Sensor
  // setupLightSensor(18, 22, 21);
  // ////////////////////////////////////////
  // // Set up break beam sensor
  // pinMode(4, INPUT);
  // pinMode(BUILTIN_LED, OUTPUT);
  // ////////////////////////////////////////
  // // Set up soil moisture sensor
  // pinMode(15, INPUT);
  // ////////////////////////////////////////////
  // // Set up motor
  // pinMode(27, OUTPUT);

  ////////////////////////////////////////////////////////

  // Allocate Storage For WiFi
  Settings user_wifi = getWifiInfo();
  EEPROM.begin(sizeof(user_wifi));
  // EEPROM.put(0, user_wifi); // DELETES PREVIOSU SAVED WIFI SETTINGS
  EEPROM.get(0, user_wifi);
  
  // Try WiFi Connection, will create SAP if fails
  connectToWifi();

  // If Not connected to WiFi
  testWifiConnection();
  ///////////////////////////////////////////////
}

void loop() {
  // if(PWMCounter >= 100) {
  //   PWMDir = -1;
  // } else if(PWMCounter <= 0) {
  //   PWMDir = 1;
  // }
  // PWMCounter += PWMDir;
  // Serial.print("PWMCounter: ");
  // Serial.println(PWMCounter);
  // setPWMDutyCycle(PWMCounter);
  // //////////////////////////////////////////////
  // Serial.print("Light: ");
  // Serial.println(getLightValue());
  // //////////////////////////////////////////////
  // int reading = digitalRead(4);
  // if (reading == HIGH) {
  //   digitalWrite(BUILTIN_LED, HIGH);
  // } else {
  //   digitalWrite(BUILTIN_LED, LOW);
  // }
  // Serial.print("Break Beam: ");
  // Serial.println(reading);
  // //////////////////////////////////////////////
  // reading = analogRead(15);
  // Serial.print("Soil Moisture: ");
  // Serial.println(reading);
  // ////////////////////////////////////////////
  // if(PWMCounter % 8 < 4) {
  //   digitalWrite(27, HIGH);
  // } else {
  //   digitalWrite(27, LOW);
  // }
  /////////////////////////////////////////
  wifiLoop();
  ///////////////////////////////////////////////////
  delay(500);
}
