#include "WoodRS485Arduino.h"
#include "WoodCurrentMD2.h"

const int ADDRESS_THIS_BOARD = 8;
const int de_pin = 2;

WoodRS485Arduino rs485 = WoodRS485Arduino(ADDRESS_THIS_BOARD, de_pin);
WoodCurrentMD2 motor = WoodCurrentMD2(&rs485, 12);

void setup() {
  
}

void loop() {
  WoodCurrentMD2Sensor sensor;
  if(motor.commandPWM(1000, &sensor)){
    Serial.println("ok");
  }else{
    Serial.println("error");
  }
  delay(100);
}
