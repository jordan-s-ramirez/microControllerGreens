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
    // calibration stuff
#define LIGHT_TOLERANCE 0.05 // percentage in decimal form
static unsigned int lux_values[5] = {0, 13, 155, 300, 1000000}; // coordinates with lightLevel enum values
#define LIGHT_COOLDOWN (15/100) // numerator is seconds to change from 0-100, denominator used to convert
  // Break beam sensor
#define BREAKBEAM_PIN 2
  // Soil moisture sensor
#define SOILMOISTURE_PIN 34
#define SOIL_MOISTURE_TRIGGER_COUNT 1000 // large numbers are more stable for pump logic but depend on low to no delay between measurements
  // Pump
#define PUMP_PIN 27
    // seconds pump is turned on when soil moisture is low
#define PUMP_ON_TIME 5
    // seconds after pump is turned on that the pump can be turned on again
    // (gives time for soil to absorb water to avoid overwatering)
#define PUMP_COOLDOWN 30
    // calibration values
#define DRY_SOIL_MEASUREMENT 2600
#define WET_SOIL_MEASUREMENT 1400


// Variables
static unsigned int PWMDutyCycle = 0;
static unsigned long lastPWMChangeTime = millis();
static unsigned long soilMoistureCounter = 0; // soil moisture counter (tracks number of recent moisture measurements that are drier than threshold, triggers pump at SOIL_MOISTURE_TRIGGER_COUNT)
static unsigned long lastPumpOnTime = millis();

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
  // Take measurements (get PWM value later)
  measurements.light = getLightValue();
  measurements.water = analogRead(SOILMOISTURE_PIN);
  measurements.breakBeam = !((bool) digitalRead(BREAKBEAM_PIN));
  // Do stuff based on preferences and measurements
  // Pump
  // update counter
  if (measurements.water > (DRY_SOIL_MEASUREMENT - ((int)preferences.waterLevel * ((DRY_SOIL_MEASUREMENT - WET_SOIL_MEASUREMENT)/5)))) {
    soilMoistureCounter++;
  } else if (soilMoistureCounter > 0) {
    soilMoistureCounter--;
  }
  unsigned long now = millis();
  // if past the cooldown time and soil moisture is above(dryer) than preference indicates, turn pump on and reset times
  if ((now - lastPumpOnTime)/1000 > PUMP_COOLDOWN && soilMoistureCounter >= SOIL_MOISTURE_TRIGGER_COUNT) {
    soilMoistureCounter=0;
    lastPumpOnTime = now;
    digitalWrite(PUMP_PIN, HIGH);
  }
  // if past pump on time, turn it off
  if ((now - lastPumpOnTime)/1000 > PUMP_ON_TIME) {
    digitalWrite(PUMP_PIN, LOW);
  }
  // Lights
  if ((now - lastPWMChangeTime)/1000 > LIGHT_COOLDOWN) {
    lastPWMChangeTime = now;
    if(measurements.light < (1-LIGHT_TOLERANCE)*lux_values[preferences.lightLevel] && PWMDutyCycle < 100) {
      PWMDutyCycle++;
    } else if (measurements.light > (1+LIGHT_TOLERANCE)*lux_values[preferences.lightLevel] && PWMDutyCycle > 0) {
      PWMDutyCycle--;
    }
    setPWMDutyCycle(PWMDutyCycle);
  }
  measurements.PWMDutyCycle = PWMDutyCycle;
  // return
  return measurements;
}