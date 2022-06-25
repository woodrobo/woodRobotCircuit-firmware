/* 
 * File:   PWM.h
 * Author: wood
 *
 * Created on 2022/05/15, 14:13
 */

#ifndef PID_H
#define	PID_H

namespace Control{

class PID{
public:
    PID(float p_gain, float i_gain, float d_gain, float i_max);
    void reset();
    float calc(float target, float now, float time_s);
protected:
    float before_deviation;
    float integral_deviation;
    float p_gain, i_gain, d_gain;
    float i_max;
};

}

#endif	/* PID_H */

