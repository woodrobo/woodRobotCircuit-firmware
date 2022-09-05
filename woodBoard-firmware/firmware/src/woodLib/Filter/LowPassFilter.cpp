#include "LowPassFilter.h"

namespace Filter{

LowPassFilter::LowPassFilter(float k){
    this->k = k;
    this->lpf_value = 0;
}

float LowPassFilter::get(float value){
    this->lpf_value += k * (value - lpf_value);
    return this->lpf_value;
}

}