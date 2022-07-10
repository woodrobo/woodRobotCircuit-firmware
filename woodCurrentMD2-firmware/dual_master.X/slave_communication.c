#include "slave_communication.h"
#include "mcc_generated_files/slave1.h"

void motorOutput(SLAVE_MODE mode, signed int power){
    ProtocolOrder_DATA motor_data;
    motor_data.ProtocolA[0] = mode;
    motor_data.ProtocolA[1] = power;
 
    //Mailbox write
    SLAVE1_ProtocolOrderWrite((ProtocolOrder_DATA*)&motor_data); 
}
