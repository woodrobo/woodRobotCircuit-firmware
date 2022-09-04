#include "WoodRS485Arduino.h"
#include <Arduino.h>

WoodRS485Arduino::WoodRS485Arduino(uint8_t address, int de_pin){
  woodRS485Init(&this->rs485_manager, address);
  this->de_pin = de_pin;
  digitalWrite(this->de_pin, LOW);
}
      
void WoodRS485Arduino::sendCommand(WoodRS485Data data){
  uint8_t data_arr[WOODRS485_DATA_SIZE+4];
  int data_size;
  woodRS485MakeData(&this->rs485_manager, data_arr, &data_size, data);

  digitalWrite(this->de_pin, HIGH);
  Serial.write(data_arr, data_size);
  Serial.flush();
  digitalWrite(this->de_pin, LOW);
}

bool WoodRS485Arduino::recvCommand(WoodRS485Data *data, unsigned long timeout_us){
  unsigned long start_time = micros();
    while((micros() - start_time) < timeout_us){//timeout
        if(Serial.available()){
            uint8_t rs485_byte = Serial.read();
            if(woodRS485AddByte(&rs485_manager, rs485_byte)){
                if(woodRS485GetData(&rs485_manager, data)){
                    //success
                    return true;
                }
            }
        }
    }
    return false;
}
