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

Device::WoodCurrentMD2 motor1(12);
Device::WoodCurrentMD2 motor2(13);
Device::EyesController eyes(21);

Control::PID yaw_pid(50.0, 10.0, 0, 1000);
Control::PID pitch_pid(50.0, 10.0, 0, 1000);

const int cycle_time_us = 5000;   //5ms

float yawDeg(){
    return -0.428 * (motor1.getADCPort() - 2112);
}

float pitchDeg(){
    return 0.428 * (motor2.getADCPort() - 1716);
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
    static int pwm = 0;
    static int current = 0;
    static int current_index = 0;
    static int current_time = 0;
    
//    if(motor1.isValid()){
//        ////////////////////simple test////////////////////
//        printf("[motor1 sensor]current:%d adc_port:%u encoder:%d\r\n", motor1.getCurrentSensor(), motor1.getADCPort(), motor1.getEncoder());
//        printf("               current:%d[mA]\r\n", (int)((float)motor1.getCurrentSensor() * ADVALUE_TO_MILLIAMPARE)); 
//        motor1.setPWM(1000);
//        
//        ////////////////////current control test////////////////////
//        current_time++;
//        if(current_time >= 200){    //period:1s
//            current_time = 0;
//            //current_index = 0-9
//            if(current_index < 9){
//                current_index++;
//            }else{
//                current_index = 0;
//            }
//            
//            if(current_index >= 8){
//                current = 5 * 1000 / ADVALUE_TO_MILLIAMPARE;
//            }else{
//                current = 0;
//            }
//        }
//        motor1.setCurrent(current);
//        printf("%d,%d\r\n", (int)(current * ADVALUE_TO_MILLIAMPARE), (int)(motor1.getCurrentSensor() * ADVALUE_TO_MILLIAMPARE)); 
//        //printf("%d,%d\r\n", current, motor1.getCurrentSensor()); 
//    }else{
//        printf("error\r\n");
//    }
    
    //////////////////kanojo move arm////////////////////   
    static bool is_first = true;
    static float target_pitch = 90;
    static bool is_down = true;
    
    motor1.setPWM(1000);
        motor2.setPWM(-1000);
    if(motor1.isValid() && motor2.isValid()){
        printf("[motor1]current:%f[A] deg:%d\r\n", motor1.getCurrentSensor(), motor1.getADCPort());
        printf("[motor2]current:%f[A] deg:%d\r\n", motor2.getCurrentSensor(), motor2.getADCPort());
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
//        int pwm_yaw = (int)yaw_pid.calc(0, yawDeg(), (float)cycle_time_us * 0.000001);
//        int pwm_pitch = (int)pitch_pid.calc(target_pitch, pitchDeg(), (float)cycle_time_us * 0.000001);
//        printf("PWM yaw:%d pitch:%d\r\n", pwm_yaw, pwm_pitch);
//        yawPWM(pwm_yaw);
//        pitchPWM(pwm_pitch);
        
    }else{
        printf("error\r\n");
    }
}

void Framework::device_task(){
    motor1.communicate();
    motor2.communicate();
    //eyes.communicate();
}