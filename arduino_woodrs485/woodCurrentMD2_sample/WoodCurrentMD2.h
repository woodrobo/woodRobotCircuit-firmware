#include "WoodRS485Arduino.h"

#pragma once

const float WoodCurrentMD2_ADVALUE_TO_AMPERE = 3300.0 / 4096.0 / (66.0 * 10.0 / 14.7);    //ACS712 30A -> 66mV/A and voltage divider

typedef struct{
  int16_t current;
  uint16_t adc_port;
  int16_t encoder;
}WoodCurrentMD2Sensor;

class WoodCurrentMD2{
public:
  WoodCurrentMD2(WoodRS485Arduino *rs485, uint8_t address, int max_pwm, float max_current);
  WoodCurrentMD2(WoodRS485Arduino *rs485, uint8_t address);
    
  //motor
  int getMaxPWM();
  float getMaxCurrent();
  bool commandPWM(int16_t value, WoodCurrentMD2Sensor *sensor);
  bool commandCurrent(float value, WoodCurrentMD2Sensor *sensor);

protected:
  WoodRS485Arduino *rs485;
  int16_t max_pwm, max_current;
  uint8_t address;
};
