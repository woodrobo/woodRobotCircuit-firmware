#include "WoodCurrentMD2.h"

namespace Device{
    
float WoodCurrentMD2_ADVALUE_TO_AMPERE = 3300.0 / 4096.0 / (66.0 * 10.0 / 14.7);    //ACS712 30A -> 66mV/A and voltage divider

WoodCurrentMD2::WoodCurrentMD2(uint8_t address, bool is_output_inverse, int max_pwm, float max_current) : RS485Device(address){
    if(max_pwm > 10000){
        max_pwm = 10000;
    }
    if(max_current > 20.0){
        max_current = 20.0;
    }
    this->mode = MODE_VOLTAGE;
    this->is_output_inverse = is_output_inverse;
    this->max_pwm = max_pwm;
    this->max_current = max_current;
    
    this->pwm = 0;
    this->current = 0;
    this->adc_port = 0;
    this->current_sensor = 0;
    this->encoder = 0;
}

WoodCurrentMD2::WoodCurrentMD2(uint8_t address) : WoodCurrentMD2(address, false, 10000, 20.0){
    //none
}

void WoodCurrentMD2::setPWM(int value){
    if(this->is_output_inverse){
        value = -value;
    }
    
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

void WoodCurrentMD2::setCurrent(float value){
    if(this->is_output_inverse){
        value = -value;
    }
        
    if(value > this->max_current){
        value = max_current;
    }else if(value < -this->max_current){
        value = -max_current;
    }
    this->mode = MODE_CURRENT;
    this->current = value;
}

float WoodCurrentMD2::getMaxCurrent(){
    return this->max_current;
}

uint16_t WoodCurrentMD2::getADCPort(){
    return this->adc_port;
}

float WoodCurrentMD2::getCurrentSensor(){
    if(is_output_inverse){
        return -this->current_sensor * WoodCurrentMD2_ADVALUE_TO_AMPERE;
    }
    return this->current_sensor * WoodCurrentMD2_ADVALUE_TO_AMPERE;
}

int16_t WoodCurrentMD2::getEncoder(){
    return this->encoder;
}

void WoodCurrentMD2::communicate(){
    int16_t current_value;
    
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
                current_value = this->current / WoodCurrentMD2_ADVALUE_TO_AMPERE;
                send_data.size = 3;
                send_data.data[0] = 1;
                send_data.data[1] = (current_value >> 8) & 0xff;
                send_data.data[2] = current_value & 0xff;
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