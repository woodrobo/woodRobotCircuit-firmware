#include "WoodCurrentMD2.h"

WoodCurrentMD2::WoodCurrentMD2(WoodRS485Arduino *rs485, uint8_t address, int max_pwm, float max_current){
  if(max_pwm > 10000){
    max_pwm = 10000;
  }
  if(max_current > 10.0){
    max_current = 10.0;
  }
  this->max_pwm = max_pwm;
  this->max_current = max_current;

  this->rs485 = rs485;
  this->address = address;
}

WoodCurrentMD2::WoodCurrentMD2(WoodRS485Arduino *rs485, uint8_t address) : WoodCurrentMD2(rs485, address, 10000, 10.0){
  //none
}
    
int16_t WoodCurrentMD2::getMaxPWM(){
  return this->max_pwm;
}

float WoodCurrentMD2::getMaxCurrent(){
  return this->max_current;
}

bool WoodCurrentMD2::commandPWM(int16_t pwm, WoodCurrentMD2Sensor *sensor){
  for(int i=0;i<3;i++){
    //send
    WoodRS485Data command;
    command.receiver_address = this->address;
    command.data[0] = 0;
    command.data[1] = (pwm >> 8);
    command.data[2] = (pwm >> 0);
    command.size = 3;
    rs485->sendCommand(command);

    //recv
    WoodRS485Data recv_data;
    if(rs485->recvCommand(&recv_data, 1000)){
      if(recv_data.sender_address == this->address && recv_data.size == 6){
        int16_t current_raw = ((recv_data.data[0] << 8) & 0xff00) | ((recv_data.data[1] << 0) & 0x00ff);
        sensor->current = current_raw * WoodCurrentMD2_ADVALUE_TO_AMPERE;
        sensor->adc_port = ((recv_data.data[2] << 8) & 0xff00) | ((recv_data.data[3] << 0) & 0x00ff);
        sensor->encoder = ((recv_data.data[4] << 8) & 0xff00) | ((recv_data.data[5] << 0) & 0x00ff);
  
        return true;
      }
    }
  }
  return false;
}
  
bool WoodCurrentMD2::commandCurrent(float ampere, WoodCurrentMD2Sensor *sensor){
    for(int i=0;i<3;i++){
    //data transform
    int16_t current_sensor_value = ampere / WoodCurrentMD2_ADVALUE_TO_AMPERE;
    
    //send
    WoodRS485Data command;
    command.receiver_address = this->address;
    command.data[0] = 1;
    command.data[1] = (current_sensor_value >> 8);
    command.data[2] = (current_sensor_value >> 0);
    command.size = 3;
    rs485->sendCommand(command);

    //recv
    WoodRS485Data recv_data;
    if(rs485->recvCommand(&recv_data, 1000)){
      if(recv_data.sender_address == this->address && recv_data.size == 6){
        int16_t current_raw = ((recv_data.data[0] << 8) & 0xff00) | ((recv_data.data[1] << 0) & 0x00ff);
        sensor->current = current_raw * WoodCurrentMD2_ADVALUE_TO_AMPERE;
        sensor->adc_port = ((recv_data.data[2] << 8) & 0xff00) | ((recv_data.data[3] << 0) & 0x00ff);
        sensor->encoder = ((recv_data.data[4] << 8) & 0xff00) | ((recv_data.data[5] << 0) & 0x00ff);
  
        return true;
      }
    }
  }
  return false;
}
