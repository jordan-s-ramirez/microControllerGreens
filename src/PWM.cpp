#include <Arduino.h>

static uint8_t CHANNEL = 0;
static int FREQUENCY = 5000;
static uint8_t RESOLUTION = 8;
static uint8_t GPIO_PIN = 26;

void setupPWM(uint8_t channel, int frequency, uint8_t resolution, uint8_t gpioPin) {
    /*
    channel (uint8_t): 0-15
    frequency (int): 5000 standard
    resolution (uint8_t): 1-16
    gpioPin (uint8_t): any
    */
    CHANNEL = channel;
    FREQUENCY = frequency;
    RESOLUTION = resolution;
    GPIO_PIN = gpioPin;
    pinMode(gpioPin, OUTPUT);
    ledcSetup(channel, frequency, resolution);
    ledcAttachPin(gpioPin, channel);
    ledcWrite(channel, 0);
}

void setPWMDutyCycle(uint8_t dutyCycle) {
    /*
    dutyCycle (uint8_t): 0-100%
    */
    uint8_t normalizedDutyCycle = (dutyCycle * ((1 << RESOLUTION) - 1)) / 100;
    ledcWrite(CHANNEL, normalizedDutyCycle);
}