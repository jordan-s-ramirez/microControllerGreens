#include <Arduino.h>
#include "soc/sens_reg.h"
// Our Files
#include "PWM.h"
#include "LightSensor.h"
#include "connectWifi.h"

uint8_t PWMCounter = 50;
int PWMDir = 1;
uint64_t reg_a;
uint64_t reg_b;
uint64_t reg_c;

void resetWifi() {
  WRITE_PERI_REG(SENS_SAR_START_FORCE_REG, reg_a);  // fix ADC registers
  WRITE_PERI_REG(SENS_SAR_READ_CTRL2_REG, reg_b);
  WRITE_PERI_REG(SENS_SAR_MEAS_START2_REG, reg_c);
}

int hardwareLoop(int preferences) {
  if(PWMCounter >= 100) {
    PWMDir = -1;
  } else if(PWMCounter <= 0) {
    PWMDir = 1;
  }
  PWMCounter += PWMDir;
  Serial.print("PWMCounter: ");
  Serial.println(PWMCounter);
  setPWMDutyCycle(PWMCounter);
  //////////////////////////////////////////////
  Serial.print("Light: ");
  Serial.println(getLightValue());
  //////////////////////////////////////////////
  int reading = digitalRead(4);
  if (reading == HIGH) {
    digitalWrite(BUILTIN_LED, HIGH);
  } else {
    digitalWrite(BUILTIN_LED, LOW);
  }
  Serial.print("Break Beam: ");
  Serial.println(reading);
  //////////////////////////////////////////////
  reading = analogRead(15);
  Serial.print("Soil Moisture: ");
  Serial.println(reading);
  ////////////////////////////////////////////
  if(PWMCounter % 8 < 4) {
    digitalWrite(27, HIGH);
  } else {
    digitalWrite(27, LOW);
  }
  return 0;
}

void setup() {
  Serial.begin(9600);
  // Save Wifi register values
  reg_a = READ_PERI_REG(SENS_SAR_START_FORCE_REG);
  reg_b = READ_PERI_REG(SENS_SAR_READ_CTRL2_REG);
  reg_c = READ_PERI_REG(SENS_SAR_MEAS_START2_REG);
  // Set up PWM
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

  //////////////////////////////////////////////////////
  createSAP();
  createWebServer();
  wifiSetupLoop();
  ///////////////////////////////////////////////
}

void loop() {
  int sensorVals;
  int preferences;
  ///////////////////////////////////////
  // sensorVals = hardwareLoop(preferences);
  ///////////////////////////////////////
  wifiLoop();
  ///////////////////////////////////////////////////
  delay(500);
}
