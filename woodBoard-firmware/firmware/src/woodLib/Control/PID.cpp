#include "PID.h"
#include <cmath>

namespace Control{

PID::PID(float p_gain, float i_gain, float d_gain, float i_max){
    this->p_gain = p_gain;
    this->i_gain = i_gain;
    this->d_gain = d_gain;
    this->i_max = i_max;
    this->integral_deviation = 0;
    this->before_deviation = 0;
}

void PID::reset(){
    this->integral_deviation = 0;
    this->before_deviation = 0;
}

float PID::calc(float target, float now, float time_s){
    float deviation = target - now;
    this->integral_deviation += deviation * time_s;
    if(std::abs(this->i_gain) > 0.001){
        if(this->i_gain * this->integral_deviation > this->i_max){
            this->integral_deviation =this->i_max /  this->i_gain;
        }else if(this->i_gain * this->integral_deviation < -this->i_max){
            this->integral_deviation = -this->i_max /  this->i_gain;
        }
    }
    
    float p = this->p_gain * deviation;
    float i = this->i_gain * this->integral_deviation;
    float d = this->d_gain * (this->before_deviation - deviation);
    
    this->before_deviation = deviation;
    
    return p + i + d;
}

}
