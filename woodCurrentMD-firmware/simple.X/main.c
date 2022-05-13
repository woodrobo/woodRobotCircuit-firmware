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
//#include "mcc_generated_files/system.h"
//#include "mcc_generated_files/pin_manager.h"
//#include "mcc_generated_files/pwm.h"
//#include "mcc_generated_files/uart1.h"
//#include "mcc_generated_files/uart2.h"
//#include "mcc_generated_files/delay.h"
#include "mcc_generated_files/mcc.h"

/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    //PWM_GeneratorEnable(PWM_GENERATOR_4);
    uint16_t max = PG4PER;
    uint16_t now = 0;
    
    while (1)
    {
        // Add your application code
//        if(UART1_IsRxReady()){
//            uint8_t hoge = UART1_Read();
//        }
        IO_PWML_SetHigh();
        IO_PHASE_SetLow();
        IO_SR_SetLow();
        //PWM_DutyCycleSet(PWM_GENERATOR_4, 2000);
        
        IO_PWMH_SetHigh();
        int i;
        for(i=0;i<50;i++){
            if(i >= 10){
                IO_PWMH_SetLow();
            }
            DELAY_microseconds(1);
        }
        
        now += max / 100;
        if(now > max){
            now = 0;
        }
        
        //DELAY_milliseconds(10);
    }
}
/**
 End of File
*/

