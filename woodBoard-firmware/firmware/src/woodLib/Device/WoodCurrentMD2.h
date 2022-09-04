#ifndef WOODCURRENTMD2_H
#define	WOODCURRENTMD2_H

#include "RS485Device.h"

namespace Device{
    
const float WoodCurrentMD2_ADVALUE_TO_AMPERE = 3300.0 / 4096.0 / (66.0 * 10.0 / 14.7);    //ACS712 30A -> 66mV/A and voltage divider

class WoodCurrentMD2 : public RS485Device{
public:
    WoodCurrentMD2(uint8_t address, bool is_output_inverse, int max_pwm, float max_current);
    WoodCurrentMD2(uint8_t address);
    
    //motor
    void setPWM(int value);
    int getMaxPWM();
    void setCurrent(float ampere);
    float getMaxCurrent();
    
    //sensor
    uint16_t getADCPort();
    float getCurrentSensor();
    int16_t getEncoder();
    
    void communicate() override;
protected:
    typedef enum{
        MODE_VOLTAGE,
        MODE_CURRENT
    }MODE;
    
    bool is_output_inverse;
    int pwm, max_pwm;
    float current, max_current;
    uint16_t adc_port;
    int16_t current_sensor, encoder;
    MODE mode;
};

}

#endif	/* WOODCURRENTMD2_H */

