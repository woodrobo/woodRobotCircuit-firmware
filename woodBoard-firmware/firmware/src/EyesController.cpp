#include "EyesController.h"

namespace Device{

EyesController::EyesController(uint8_t address) : RS485Device(address){
    this->is_send = false;
    this->mode = 0;
    this->x = 0;
    this->y = 0;
}

void EyesController::setEyes(uint8_t mode, uint8_t x, uint8_t y){
    this->mode = mode;
    this->x = x;
    this->y = y;
    this->is_send = true;
}

void EyesController::communicate(){
    if(this->is_send){
        this->is_send = false;
        for(int i=0;i<3;i++){
            WoodRS485Data send_data, recv_data;

            send_data.size = 3;
            send_data.data[0] = this->mode;
            send_data.data[1] = this->x;
            send_data.data[2] = this->y;

            RS485Device::send(send_data);

            this->is_valid = false;
            if(RS485Device::recv(&recv_data, 1000)){
                if(recv_data.sender_address == this->address && recv_data.size == 1){
                    this->is_valid = true;
                    //recv_data is dummy data
                    uint8_t dummy = recv_data.data[0];
                }
            }

            if(this->is_valid){
                break;
            }
        }
    }
}

}