/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  dsPIC33CH64MP202S1
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB 	          :  MPLAB X v5.45
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/master.h"
#include "mcc_generated_files/adc1.h"
#include "mcc_generated_files/delay.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/pwm.h"

#include "motorController.h"
#include "master_communication.h"
#include "buffer.h"

/*
                         Main application
 */

ProtocolOrder_DATA order;
bool current_control = false;
signed int target_current = 0;
int16_t current_sensor_vol;

//calibration
int16_t current_sensor_offset = 0;

//average current
RingBuffer current_buffer;

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    PWM_GeneratorEnable(PWM_GENERATOR_1);
    PWM_GeneratorEnable(PWM_GENERATOR_2);
    
    //電流バッファの初期化
    bufferInit(&current_buffer);
    
    //電流センサのキャリブレーション
    bool calibration_flag = true;
    int calibration_count = 0;
    uint32_t offset_32bit = 0;
    while(calibration_flag){
        //PWMと同期して電流値を取得
        while(!ADC1_IsConversionComplete(channel_S1AN16));
        offset_32bit += ADC1_ConversionResultGet(channel_S1AN16);
        calibration_count++;
        if(calibration_count >= 32){
            current_sensor_offset = (offset_32bit >> 5);
            calibration_flag = false;
        }
    }
    
    while (1)
    {
        //MASTERとのメールボックス処理
        if(MASTER_ProtocolOrderRead((ProtocolOrder_DATA*) &order)){
            if(order.ProtocolA[0] == SLAVE_MODE_VOLTAGE){
                motorVoltageController(true, order.ProtocolA[1]);
                current_control = false;
            }else if(order.ProtocolA[0] == SLAVE_MODE_CURRENT){
                target_current = order.ProtocolA[1];
                current_control = true;
            }else{
                motorVoltageController(false, 0);
                current_control = false;
            }
            
            ProtocolMonitor_DATA monitor;
            //monitor.ProtocolB[0] = current_sensor_vol;
            monitor.ProtocolB[0] = bufferGetAverage(&current_buffer);
            MASTER_ProtocolMonitorWrite((ProtocolMonitor_DATA*) &monitor);
        }
        
        //PWMと同期して電流値を取得
        while(!ADC1_IsConversionComplete(channel_S1AN16));
        IO_RB5_SetHigh();
        //DELAY_microseconds(2);
        current_sensor_vol = ADC1_ConversionResultGet(channel_S1AN16) - current_sensor_offset;
        bufferPush(&current_buffer, current_sensor_vol);
        if(current_control){
            motorCurrentController(target_current, current_sensor_vol);
        }
        IO_RB5_SetLow();
    }
    return 1; 
}
/**
 End of File
*/

