// Includes
#include "Hardware.h"
#include "PWM.h"
#include "LightSensor.h"
#include "DataStructures.h"
// Defines
  // PWM
#define PWM_CHANNEL 0
#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8
#define PWM_PIN 26
  // Light sensor
#define LIGHT_CEN 18
#define LIGHT_SCL 22
#define LIGHT_SDA 21
  // Break beam sensor
#define BREAKBEAM_PIN 4
  // Soil moisture sensor
#define SOILMOISTURE_PIN 15
  // Pump
#define PUMP_PIN 27

// Code
// Sets up PWM (for lights), Light sensor, Break beam sensor, Soil moisture sensor, and Pump
void hardwareSetup() {
    ////////////////////////////////////////////////
    // Set up Light Sensor
    setupLightSensor(LIGHT_CEN, LIGHT_SCL, LIGHT_SDA);
    ////////////////////////////////////////
    // Set up soil moisture sensor
    pinMode(SOILMOISTURE_PIN, INPUT);
    ////////////////////////////////////////////
    // Set up break beam sensor
    pinMode(BREAKBEAM_PIN, INPUT);
    ////////////////////////////////////////
    // Set up PWM
    setupPWM(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION, PWM_PIN);
    ////////////////////////////////////////////
    // Set up pump
    pinMode(PUMP_PIN, OUTPUT);
}

// Takes measurements from sensors, changes lights or runs pump if necessary,
// and returns the measurements
Measurements hardwareLoop(Preferences preferences) {
    Measurements measurements = {0};
    // Take measurements
    measurements.light = getLightValue();
    measurements.water = analogRead(SOILMOISTURE_PIN);
    measurements.breakBeam = (bool) digitalRead(BREAKBEAM_PIN);
    // Do stuff based on preferences and measurements
    // TODO (PWM(lights) and Pump)

    return measurements;
}