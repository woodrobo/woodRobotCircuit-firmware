//main.c
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

//add
#include <string.h>
#include <cstdlib>
#include <cmath>
#include <lega-c/stdint.h>
#include <proc/p32mz2048efh064.h>
#include "woodLib/Framework/framework.h"
#include "woodLib/Device/RS485Device.h"
#include "woodLib/Device/WoodCurrentMD2.h"
#include "woodLib/Control/PID.h"
#include "woodLib/Filter/LowPassFilter.h"
#include "EyesController.h"

Device::WoodCurrentMD2 motor1(12, false, 10000, 8.0);
Device::WoodCurrentMD2 motor2(13, true, 10000, 8.0);
Device::EyesController eyes(21);

Filter::LowPassFilter yawOmegaLPF(0.1);
Filter::LowPassFilter pitchOmegaLPF(0.1);

const int cycle_time_us = 5000;   //5ms

//control parameter
Control::PID yaw_pid(0.05, 0.0, 0, 2.0);
Control::PID pitch_pid(0.05, 0.0, 0, 2.0);
const float YAW_FRICTION = 2.0;
const float PITCH_FRICTION = 2.0;
const float YAW_GRAVITY = 0.0;
const float PITCH_GRAVITY = 2.5;
const float MOVE_OMEGA_THRESHOLD = 0.5 / (cycle_time_us * 0.000001);
const float CURRENT_FLICTION_THRESHOLD = 0.2;

float yawDeg(){
    return -0.45 * (motor1.getADCPort() - 1470);
}

float pitchDeg(){
    return 0.45 * (motor2.getADCPort() - 2940);
}

void pwmServoSetDeg(float deg){
    float pulse_us = 700 + (1750-700)/90.0 * deg;
    OCMP5_CompareSecondaryValueSet(pulse_us * 3.125);   //3.125MHz
}

typedef enum{
    CONTROL_MODE_NONE = 0,
    CONTROL_MODE_CURRENT = 1,
    CONTROL_MODE_POSITION = 2        
}CONTROL_MODE;

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
    //common variable
    static CONTROL_MODE control_mode = CONTROL_MODE_NONE;
    static float current_value = 0;
    static float target_yaw = 0, target_pitch = 0;
    static bool is_enable_pid = false;
    
    //////////////////command////////////////////
    const int command_buffer_size = 20;
    static char command_buffer[command_buffer_size];
    static int command_buffer_index = 0;
    
    if(UART1_ReceiverIsReady()){
        command_buffer[command_buffer_index] = UART1_ReadByte();
        command_buffer_index++;
        if(command_buffer[command_buffer_index - 1] == '\n'){
            command_buffer[command_buffer_index] = '\0';
            char command[command_buffer_size]; 
            int i;
            for(i=0;i<command_buffer_index-1;i++){
                if(command_buffer[i] == ' '){
                    break;
                }
                command[i] = command_buffer[i];
            }
            command[i] = '\0';
            
            printf("command:%s\r\n", command);
            
            //command response
            if(strcmp(command, "start") == 0){
                printf("[position control]only gravity and friction\r\n");
                control_mode = CONTROL_MODE_POSITION;
                is_enable_pid = false;
            }else if(strcmp(command, "current") == 0){
                if(command_buffer_index > i+1){
                    current_value = (float)atoi(&command_buffer[i+1]) / 1000.0;
                    control_mode = CONTROL_MODE_CURRENT;
                    printf("current %d[mA]\r\n", (int)(current_value * 1000));
                }
            }else if(command[0] == '\0'){       //only enter
                if(control_mode == CONTROL_MODE_POSITION && !is_enable_pid){
                    printf("[position control]pid enable\r\n");
                    target_pitch = pitchDeg();
                    target_yaw = yawDeg();
                    is_enable_pid = true; 
                }else{
                    printf("stop\r\n");
                    control_mode = CONTROL_MODE_NONE;
                }
            }
            
            command_buffer_index = 0;
        }
    }
    
    //////////////////kanojo move arm////////////////////  
    static float before_yawDeg = 0, before_pitchDeg = 0;
    
    //calc velocity
    float yawOmegaRaw = (yawDeg() - before_yawDeg) / ((float)cycle_time_us * 0.000001);
    float pitchOmegaRaw = (pitchDeg() - before_pitchDeg) / ((float)cycle_time_us * 0.000001);
    float yawOmega = yawOmegaLPF.get(yawOmegaRaw);
    float pitchOmega = pitchOmegaLPF.get(pitchOmegaRaw);
    
    //printf("%d, %d, %d, %d\r\n", (int)yawOmegaRaw, (int)pitchOmegaRaw, (int)yawOmega, (int)pitchOmega);
    
    pwmServoSetDeg(80);
    
    if(motor1.isValid() && motor2.isValid()){
        if(control_mode == CONTROL_MODE_POSITION){
            float current_yaw = 0, current_pitch = 0;
            //pid control
            float pid_yaw = yaw_pid.calc(target_yaw, yawDeg(), (float)cycle_time_us * 0.000001);
            float pid_pitch = pitch_pid.calc(target_pitch, pitchDeg(), (float)cycle_time_us * 0.000001);
            if(is_enable_pid){
                current_yaw += pid_yaw;
                current_pitch += pid_pitch;
            }
            
            //friction
            if(yawOmega > MOVE_OMEGA_THRESHOLD){
                current_yaw += YAW_FRICTION;
            }else if(yawOmega < -MOVE_OMEGA_THRESHOLD){
                current_yaw -= YAW_FRICTION;
            }else if(current_yaw > CURRENT_FLICTION_THRESHOLD){
                current_yaw += YAW_FRICTION;
            }else if(current_yaw < -CURRENT_FLICTION_THRESHOLD){
                current_yaw -= YAW_FRICTION;
            }
            if(pitchOmega > MOVE_OMEGA_THRESHOLD){
                current_pitch += PITCH_FRICTION;
            }else if(pitchOmega < -MOVE_OMEGA_THRESHOLD){
                current_pitch -= PITCH_FRICTION;
            }else if(current_pitch > CURRENT_FLICTION_THRESHOLD){
                current_pitch += PITCH_FRICTION;
            }else if(current_pitch < -CURRENT_FLICTION_THRESHOLD){
                current_pitch -= PITCH_FRICTION;
            }
            
            //gravity
            current_yaw += YAW_GRAVITY * 0;
            current_pitch += PITCH_GRAVITY * std::cos(pitchDeg() * 3.14 / 180.0);
            
            printf("[angle] yaw:%d[deg] pitch:%d[deg] [target current] yaw:%d[mA] pitch:%d[mA]\r\n", (int)yawDeg(), (int)pitchDeg(), (int)(current_yaw*1000), (int)(current_pitch*1000));
            //printf("adc1:%u, adc2:%u\r\n", motor1.getADCPort(), motor2.getADCPort());
            motor1.setCurrent(current_yaw);
            motor2.setCurrent(current_pitch);
        }else if(control_mode == CONTROL_MODE_CURRENT){
//            motor1.setCurrent(current_value);
//            motor2.setPWM(0);
            motor1.setPWM(0);
            motor2.setCurrent(current_value);
        }else{
            motor1.setPWM(0);
            motor2.setPWM(0);
        }
    }else{
        printf("error\r\n");
    }
    
    //save deg
    before_yawDeg = yawDeg();
    before_pitchDeg = pitchDeg();
}

void Framework::device_task(){
    motor1.communicate();
    motor2.communicate();
    //eyes.communicate();
}