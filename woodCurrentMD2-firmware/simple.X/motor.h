/* 
 * File: wood_rs485.h
 * Author: wood
 * Comments: wood_rs485 protocol file
 * Revision history: ver1.00 2022/04/17
 */

#ifndef MOTOR_H
#define	MOTOR_H

#include "mcc_generated_files/pin_manager.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */
    
#define MAX_PWM 9000    //180MHz to 20kHz
    
//Motor Control
//A = PWM_GENERATOR_2
//B = PWM_GENERATOR_1

void motorOutput(bool on, signed int pwm){
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

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* MOTOR_H */