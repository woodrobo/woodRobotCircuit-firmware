#ifndef WOODCURRENTMD2_H
#define	WOODCURRENTMD2_H

#include "RS485Device.h"

namespace Device{

class WoodCurrentMD2 : public RS485Device{
public:
    WoodCurrentMD2(uint8_t address, int max_pwm, int max_current);
    WoodCurrentMD2(uint8_t address);
    
    //motor
    void setPWM(int value);
    int getMaxPWM();
    void setCurrent(int value);
    int getMaxCurrent();
    
    //sensor
    uint16_t getADCPort();
    uint16_t getCurrentSensor();
    
    void communicate() override;
protected:
    typedef enum{
        MODE_VOLTAGE,
        MODE_CURRENT
    }MODE;
    
    int pwm, max_pwm;
    int current, max_current;
    uint16_t adc_port, current_sensor;
    MODE mode;
};

}

#endif	/* WOODCURRENTMD2_H */

