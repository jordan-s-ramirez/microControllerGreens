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

void setup() {
  Serial.begin(9600);

  // hardwareSetup();

  createSAP();
  createWebServer();
  wifiSetupLoop();
}

void loop() {
  // measurements = hardwareLoop(preferences);

  // preferences = wifiLoop();
  wifiLoop();

}
