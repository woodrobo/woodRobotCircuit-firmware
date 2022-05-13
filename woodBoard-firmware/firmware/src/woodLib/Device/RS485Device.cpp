#include "RS485Device.h"
#include "../../wood_rs485/wood_rs485.h"
#include "peripheral/coretimer/plib_coretimer.h"

namespace Device{

WoodRS485Manager rs485_manager;

RS485Device::RS485Device(uint8_t address) {
    this->address = address;
    this->is_valid = false;
}

uint8_t RS485Device::getAddress(){
    return this->address;
}

bool RS485Device::isValid(){
    return this->is_valid;
}

void RS485Device::send(WoodRS485Data send_data){
    send_data.receiver_address = this->address;
    
    uint8_t packet[WOODRS485_DATA_SIZE+4];
    int size;
    woodRS485MakeData(&rs485_manager, packet, &size, send_data);
    
    RS485_SW_Set();
    UART5_Write(packet, size);
    //wait until data send complete
    while(UART5_WriteCountGet() != 0 || !U5STAbits.TRMT);
    RS485_SW_Clear();
}

bool RS485Device::recv(WoodRS485Data *recv_data,int timeout_us){
    uint32_t start = CORETIMER_CounterGet();
    while((CORETIMER_CounterGet() - start) < (CORETIMER_FrequencyGet() / 1000000 * timeout_us)){//timeout
        if(UART5_ReadCountGet()){
            uint8_t byte;
            UART5_Read(&byte, 1);
            if(woodRS485AddByte(&rs485_manager, byte)){
                if(woodRS485GetData(&rs485_manager, recv_data)){
                    //success
                    return true;
                }
            }
        }
    }
    return false;
}

}
