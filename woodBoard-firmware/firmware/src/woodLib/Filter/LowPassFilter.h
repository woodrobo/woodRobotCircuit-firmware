/* 
 * File:   LowPassFilter.h
 * Author: wood
 *
 * Created on 2022/09/05, 22:05
 */

#ifndef LOWPASSFILTER_H
#define	LOWPASSFILTER_H

namespace Filter{

class LowPassFilter{
public:
    LowPassFilter(float k);
    float get(float value);
private:
    float k;
    float lpf_value;
};

}

#endif	/* LOWPASSFILTER_H */

