#include "framework.h"
#include "definitions.h"
#include "peripheral/coretimer/plib_coretimer.h"
#include <stdio.h>

namespace Framework{
    void run(const unsigned int cycle_time_us){
        while(true){
            //record start time
            unsigned int start = CORETIMER_CounterGet();
            
            /* Maintain state machines of all polled MPLAB Harmony modules. */
            SYS_Tasks ( );

            //task
            device_task();
            main_task();

            //debug
            unsigned int elapsed_time_us = (CORETIMER_CounterGet() - start) / (CORETIMER_FrequencyGet() / 1000000);
            //printf("[framework]elapsed_time:%u[us]\r\n", elapsed_time_us);
            
            //wait
            unsigned int cycle_time_counter = CORETIMER_FrequencyGet() / 1000000 * cycle_time_us;
            while((CORETIMER_CounterGet() - start) < cycle_time_counter);
        }
    }
    
    void __attribute__((weak)) main_task(){
        //none
    }
    
    void __attribute__((weak)) device_task(){
        //none
    }
}
