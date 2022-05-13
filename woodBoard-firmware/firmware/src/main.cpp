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

Device::WoodCurrentMD2 motor1(11);

int main(int argc, char** argv) {
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    /* user code */
    woodRS485Init(&Device::rs485_manager, 8);
    printf("woodBoard start\r\n");
    
    //framework loop 1s
    Framework::run(1000000);

    /* Execution should not come here during normal operation */
    return ( EXIT_FAILURE );
}

void Framework::main_task(){
    static int count = 0;
    if(count == 0){
        motor1.setPWM(1000);
        count = 1;
    }else{
        motor1.setPWM(-1000);
        count = 0;
    }
    
    if(motor1.isValid()){
        const float ADVALUE_TO_MILLIAMPARE = 3300.0 / 4096.0 / 66.0 * 1000.0;    //ACS712 30A -> 66mV/A
        printf("[motor1 sensor]current:%u adc_port:%u\r\n", motor1.getCurrentSensor(), motor1.getADCPort());
        printf("               current:%d[mA]\r\n", (int)((float)(motor1.getCurrentSensor() - 2111) * ADVALUE_TO_MILLIAMPARE)); 
    }
}

void Framework::device_task(){
    motor1.communicate();
}