#include "WoodCurrentMD2.h"

namespace Device{

WoodCurrentMD2::WoodCurrentMD2(uint8_t address, int max_pwm, int max_current) : RS485Device(address){
    if(max_pwm > 8800){
        max_pwm = 8800;
    }
    if(max_current > 10000){
        max_current = 10000;
    }
    this->mode = MODE_VOLTAGE;
    this->max_pwm = max_pwm;
    this->max_current = max_current;
    
    this->pwm = 0;
    this->current = 0;
    this->adc_port = 0;
    this->current_sensor = 0;
}

WoodCurrentMD2::WoodCurrentMD2(uint8_t address) : WoodCurrentMD2(address, 8800, 10000){
    //none
}

void WoodCurrentMD2::setPWM(int value){
    if(value > this->max_pwm){
        value = max_pwm;
    }else if(value < -this->max_pwm){
        value = -max_pwm;
    }
    this->mode = MODE_VOLTAGE;
    this->pwm = value;
}

int WoodCurrentMD2::getMaxPWM(){
    return this->max_pwm;
}

void WoodCurrentMD2::setCurrent(int value){
    if(value > this->max_current){
        value = max_current;
    }else if(value < -this->max_current){
        value = -max_current;
    }
    this->mode = MODE_CURRENT;
    this->current = value;
}

int WoodCurrentMD2::getMaxCurrent(){
    return this->max_current;
}

uint16_t WoodCurrentMD2::getADCPort(){
    return this->adc_port;
}

int16_t WoodCurrentMD2::getCurrentSensor(){
    return this->current_sensor;
}

int16_t WoodCurrentMD2::getEncoder(){
    return this->encoder;
}

void WoodCurrentMD2::communicate(){
    for(int i=0;i<3;i++){
        WoodRS485Data send_data, recv_data;
    
        switch(this->mode){
            case MODE_VOLTAGE:
                send_data.size = 3;
                send_data.data[0] = 0;
                send_data.data[1] = (pwm >> 8) & 0xff;
                send_data.data[2] = pwm & 0xff;
                break;
            case MODE_CURRENT:
                send_data.size = 3;
                send_data.data[0] = 1;
                send_data.data[1] = (current >> 8) & 0xff;
                send_data.data[2] = current & 0xff;
                break;
            default:
                send_data.size = 3;
                send_data.data[0] = 0;
                send_data.data[1] = 0;
                send_data.data[2] = 0;
                break;
        }

        RS485Device::send(send_data);

        this->is_valid = false;
        if(RS485Device::recv(&recv_data, 1000)){
            if(recv_data.sender_address == this->address && recv_data.size == 6){
                this->is_valid = true;
                this->current_sensor = ((recv_data.data[0] << 8) & 0xff00) | ((recv_data.data[1] << 0) & 0x00ff);
                this->adc_port = ((recv_data.data[2] << 8) & 0xff00) | ((recv_data.data[3] << 0) & 0x00ff);
                this->encoder = ((recv_data.data[4] << 8) & 0xff00) | ((recv_data.data[5] << 0) & 0x00ff);
            }
        }
        
        if(this->is_valid){
            break;
        }
    }
}

}