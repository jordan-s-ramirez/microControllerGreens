#pragma once
#include <Arduino.h>

void setupPWM(uint8_t channel, int frequency, uint8_t resolution, uint8_t gpioPin);
void setPWMDutyCycle(uint8_t dutyCycle);