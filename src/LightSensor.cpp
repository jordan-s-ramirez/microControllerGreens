#include "DFRobot_B_LUX_V30B.h"

static DFRobot_B_LUX_V30B myLux(18, 22, 21);

void setupLightSensor(uint8_t cEN, uint8_t scl, uint8_t sda) {
    myLux = DFRobot_B_LUX_V30B(cEN, scl, sda);
    myLux.begin();
}

float getLightValue() {
    return myLux.lightStrengthLux();
}