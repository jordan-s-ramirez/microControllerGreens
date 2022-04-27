// Libraries
#include <Arduino.h>
// Our Files
#include "Hardware.h"
#include "connectWifi.h"
#include "DataStructures.h"

// Global variables
  // Sensor values - light, soil moisture, break beam
Measurements measurements;
  // User preferences - light and water levels
Preferences preferences;
  // Wifi timing stuff
unsigned long wifiTimer = 0;
#define WIFI_INTERVAL 30 // time delay between wifi sending and receiving data in seconds

void setup() {
  Serial.begin(9600);

  Serial.println("entering hardware setup");
  hardwareSetup();
  Serial.println("exiting hardware setup");

  Serial.println("entering wifi setup");
  wifiSetup();
  Serial.println("exiting wifi setup");
}

void loop() {
  measurements = hardwareLoop(preferences);
  if ((millis() - wifiTimer)/1000 > WIFI_INTERVAL) {
    Serial.println("Measurements");
    Serial.print("light: ");
    Serial.println(measurements.light);
    Serial.print("water: ");
    Serial.println(measurements.water);
    Serial.print("break beam: ");
    Serial.println(measurements.breakBeam);
    wifiTimer = millis();
    preferences = wifiLoop(measurements);
    Serial.println("Preferences");
    Serial.print("light: ");
    Serial.println(preferences.lightLevel);
    Serial.print("water: ");
    Serial.println(preferences.waterLevel);
  }
}
