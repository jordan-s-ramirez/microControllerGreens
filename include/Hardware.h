#pragma once
#include "DataStructures.h"

// Sets up PWM (for lights), Light sensor, Break beam sensor, Soil moisture sensor, and Pump
void hardwareSetup();

// Takes measurements from sensors, changes lights or runs pump if necessary,
// and returns the measurements
Measurements hardwareLoop(Preferences preferences);