//main.c
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

//add
#include <cstdlib>
#include <lega-c/stdint.h>
#include <proc/p32mz2048efh064.h>
#include "woodLib/Framework/framework.h"
#include "woodLib/Device/RS485Device.h"
#include "woodLib/Device/WoodCurrentMD2.h"
#include "woodLib/Control/PID.h"
#include "EyesController.h"

Device::WoodCurrentMD2 motor1(11);
Device::WoodCurrentMD2 motor2(12);
Device::EyesController eyes(21);

Control::PID yaw_pid(50.0, 10.0, 0, 1000);
Control::PID pitch_pid(50.0, 10.0, 0, 1000);

const int cycle_time_us = 10000;   //10ms

float yawDeg(){
    return -0.428 * (motor1.getADCPort() - 2112);
}

float pitchDeg(){
    return 0.428 * (motor2.getADCPort() - 1716);
}

float yawCurrent(){
    const float ADVALUE_TO_MILLIAMPARE = 3300.0 / 4096.0 / 66.0 * 1000.0;    //ACS712 30A -> 66mV/A
    return (motor1.getCurrentSensor() - 2111) * ADVALUE_TO_MILLIAMPARE;
}

float pitchCurrent(){
    const float ADVALUE_TO_MILLIAMPARE = 3300.0 / 4096.0 / 66.0 * 1000.0;    //ACS712 30A -> 66mV/A
    return (motor2.getCurrentSensor() - 2111) * ADVALUE_TO_MILLIAMPARE;
}

void yawPWM(int16_t value){
    motor1.setPWM(value);
}

void pitchPWM(int16_t value){
    motor2.setPWM(-value);
}

void pwmServoSetDeg(float deg){
    float pulse_us = 700 + (1750-700)/90.0 * deg;
    OCMP5_CompareSecondaryValueSet(pulse_us * 3.125);   //3.125MHz
}

int main(int argc, char** argv) {
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    /* peripherals */
    TMR2_Start();
    OCMP1_Enable();
    OCMP4_Enable();
    OCMP5_Enable();
    
    /* user code */
    woodRS485Init(&Device::rs485_manager, 8);
    printf("woodBoard start\r\n");
    
    //framework loop
    Framework::run(cycle_time_us);

    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE );
}

void Framework::main_task(){  
//    printf("[motor1 sensor]current:%u adc_port:%u\r\n", motor1.getCurrentSensor(), motor1.getADCPort());
//    printf("               current:%d[mA]\r\n", (int)((float)(motor1.getCurrentSensor() - 2111) * ADVALUE_TO_MILLIAMPARE)); 
    
    static bool is_first = true;
    static float target_pitch = 90;
    static bool is_down = true;
    
    if(motor1.isValid() && motor2.isValid()){
        printf("[motor1]current:%d[mA] deg:%d\r\n", (int)yawCurrent(), (int)yawDeg());
        printf("[motor2]current:%d[mA] deg:%d\r\n", (int)pitchCurrent(), (int)pitchDeg());
        pwmServoSetDeg(0);
        //eyes.setEyes(0, 0, 0);
        
        //route
        const float pitch_speed = 30.0;
        const float pitch_min = 45.0;
        const float pitch_max = 90.0;
        if(is_down){
            target_pitch -= pitch_speed * (float)cycle_time_us * 0.000001;
            if(target_pitch < pitch_min){
                target_pitch = pitch_min;
                is_down = false;
            }
        }else{
            target_pitch += pitch_speed * (float)cycle_time_us * 0.000001;
            if(target_pitch > pitch_max){
                target_pitch = pitch_max;
                is_down = true;
            }
        }
        
        //pid control
        int pwm_yaw = (int)yaw_pid.calc(0, yawDeg(), (float)cycle_time_us * 0.000001);
        int pwm_pitch = (int)pitch_pid.calc(target_pitch, pitchDeg(), (float)cycle_time_us * 0.000001);
        printf("PWM yaw:%d pitch:%d\r\n", pwm_yaw, pwm_pitch);
        yawPWM(pwm_yaw);
        pitchPWM(pwm_pitch);
    }else{
        printf("error\r\n");
    }
}

void Framework::device_task(){
    motor1.communicate();
    motor2.communicate();
    eyes.communicate();
}