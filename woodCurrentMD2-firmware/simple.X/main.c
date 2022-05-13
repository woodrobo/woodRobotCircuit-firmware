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
        Device            :  dsPIC33CH64MP202
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
#include "mcc_generated_files/pwm.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/delay.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/adc1.h"
#include "motor.h"
#include "wood_rs485/wood_rs485.h"
#include "mcc_generated_files/tmr1.h"

/*
                         Main application
 */

//parameter
//PWM clock:176MHz frequency:20kHz MAX_VALUE:8800
//RS485 2MHz
//debug UART 1MHz

#define ADDRESS_MAINBOARD   8
#define ADDRESS_THIS_BOARD  12

#define MODE_VOLTAGE        0
#define MODE_CURRENT        1

WoodRS485Manager rs485_manager;

void rs485_physical_layer_write(uint8_t packet[], int size){
    RS485_SW_PIN_SetHigh();
    UART1_WriteBuffer(packet, size);
    //wait until data send complete
    while(!UART1_IsTxDone());
    RS485_SW_PIN_SetLow();
}

//TMR1 callback function 1000ms
void timeover_callback(){
    //if rs485 time over, motor drive shutdown
    motorOutput(false, 0);
    printf("time over\r\n");
}

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    //timer
    TMR1_SetInterruptHandler(&timeover_callback);
    TMR1_Start();
    //RS485
    woodRS485Init(&rs485_manager, ADDRESS_THIS_BOARD);
    RS485_SW_PIN_SetLow();
    
    while (1)
    {
        // Add your application code
        if(UART1_IsRxReady()){
            if(woodRS485AddByte(&rs485_manager, UART1_Read())){
                WoodRS485Data receive_data;
                if(woodRS485GetData(&rs485_manager, &receive_data)){
                    //get data
                    if(receive_data.sender_address == ADDRESS_MAINBOARD && receive_data.size == 3){
                        //reset timer
                        TMR1_Counter16BitSet(0);
                        
                        //motor drive
                        int16_t value = ((receive_data.data[1] << 8) & 0xff00) | (receive_data.data[2] & 0x00ff);
                        switch(receive_data.data[0]){
                            case MODE_VOLTAGE:
                                motorOutput(true, value);
                                break;
                            case MODE_CURRENT:
                                //none
                                break;
                            default:
                                //none
                                break;
                        }
                        
                        //get sensor data
                        ADC1_SoftwareTriggerEnable();
                        while(ADC1_IsConversionComplete(channel_AN1));
                        while(ADC1_IsConversionComplete(channel_AN2));
                        uint16_t adc_port_vol = ADC1_ConversionResultGet(channel_AN1);
                        uint16_t current_sensor_vol = ADC1_ConversionResultGet(channel_AN2);
                        
                        //make sensor data
                        WoodRS485Data send_data;
                        uint8_t send_buf[10];
                        int send_size;
                        send_data.receiver_address = receive_data.sender_address;
                        send_data.size = 3;
                        send_data.data[0] = (current_sensor_vol >> 4) & 0xff;
                        send_data.data[1] = ((current_sensor_vol << 4) & 0xf0) | ((adc_port_vol >> 8) & 0x0f);
                        send_data.data[2] = adc_port_vol & 0xff;
                        woodRS485MakeData(&rs485_manager, send_buf, &send_size, send_data);
                        rs485_physical_layer_write(send_buf, send_size);
                        
                        //debug
                        printf("cur:%u, adc:%u\r\n", current_sensor_vol, adc_port_vol);
                    }
                }
            }
        }
    }
    return 1; 
}
/**
 End of File
*/

