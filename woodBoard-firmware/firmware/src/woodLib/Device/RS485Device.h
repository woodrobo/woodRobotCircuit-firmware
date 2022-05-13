/* 
 * File:   RS485Device.h
 * Author: wood
 *
 * Created on 2022/05/02, 2:15
 */

#ifndef RS485DEVICE_H
#define	RS485DEVICE_H

#include <vector>
#include <memory>
#include "wood_rs485/wood_rs485.h"
#include "config/default/peripheral/gpio/plib_gpio.h"
#include "config/default/peripheral/uart/plib_uart5.h"

namespace Device{

extern WoodRS485Manager rs485_manager;

class RS485Device{
public:
    RS485Device(uint8_t address);
    uint8_t getAddress();
    bool isValid();
    virtual void communicate() = 0;
protected:
    void send(WoodRS485Data send_data);
    bool recv(WoodRS485Data *recv_data,int timeout_us);
    uint8_t address;
    bool is_valid;
};

}

#endif	/* RS485DEVICE_H */

