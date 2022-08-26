#include "motorController.h"

#include <stdbool.h>
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/pwm.h"

float POWER_VCC = 12.0;                 //[V]
float MAX_CURRENT_IGAIN_VOLTAGE = 10.0;  //[V]
float MOTOR_RESISTANCE = 0.5;           //[ohm]
float CURRENT_PGAIN = 0.7;              //[V/I]
float CURRENT_IGAIN = 14.0;              //[V/(I*s)]
//float CURRENT_PGAIN = 1;              //[V/I]
//float CURRENT_IGAIN = 20.0;              //[V/(I*s)]
//float CURRENT_IGAIN = 0.01 * 0.8 * 20000;              //[V/(I*s)]

float current_deviation_integral = 0.0;

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

void motorCurrentController(signed int target_current, signed int now_current){
    //単位変換
    const float ADVALUE_TO_AMPARE = 3300.0 / 4096.0 / (66.0 * 10.0 / 14.7);    //ACS712 30A -> 66mV/A and voltage divider
    float target = target_current * ADVALUE_TO_AMPARE;
    float now = now_current * ADVALUE_TO_AMPARE;
    
    //偏差計算
    float deviation = target - now;                                     //[A]
    current_deviation_integral += deviation * 0.00005;                  //[A*s]
    if(MAX_CURRENT_IGAIN_VOLTAGE < CURRENT_IGAIN * current_deviation_integral){
        current_deviation_integral = MAX_CURRENT_IGAIN_VOLTAGE / CURRENT_IGAIN;
    }else if(-MAX_CURRENT_IGAIN_VOLTAGE > CURRENT_IGAIN * current_deviation_integral){
        current_deviation_integral = -MAX_CURRENT_IGAIN_VOLTAGE / CURRENT_IGAIN;
    }
    
    //PI制御 + モーターの抵抗値モデル
    float vol = MOTOR_RESISTANCE * target;                                              //[V]
    vol += CURRENT_PGAIN * deviation + CURRENT_IGAIN * current_deviation_integral;      //[V]
    
    //電圧出力
    signed int pwm;
    if(vol > POWER_VCC){
        pwm = 10000;
    }else if(vol < -POWER_VCC){
        pwm = -10000;
    }else{
        pwm = vol / POWER_VCC * 10000;
    }
    motorVoltageController(true, pwm);
}

void motorCurrentControllerReset(){
    current_deviation_integral = 0.0;
}