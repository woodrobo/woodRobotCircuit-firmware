#include "motorController.h"

#include <stdbool.h>
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/pwm.h"

void motorVoltageController(bool on, signed int pwm){
    if(!on){
        PWM_DutyCycleSet(PWM_GENERATOR_1, 0);
        PWM_DutyCycleSet(PWM_GENERATOR_2, 0);
        SD_PIN_SetLow();
    }else{
        if(pwm == 0){
            PWM_DutyCycleSet(PWM_GENERATOR_1, 0);
            PWM_DutyCycleSet(PWM_GENERATOR_2, 0);
        }else if(pwm > 0){
            PWM_DutyCycleSet(PWM_GENERATOR_1, 0);
            PWM_DutyCycleSet(PWM_GENERATOR_2, (unsigned int)pwm);
        }else if(pwm < 0){
            PWM_DutyCycleSet(PWM_GENERATOR_1, (unsigned int)(-pwm));
            PWM_DutyCycleSet(PWM_GENERATOR_2, 0);
        }
        SD_PIN_SetHigh();
    }
}

void motorCurrentController(signed int current){
    //TODO
    //書き換える
    motorVoltageController(true, current);
}
