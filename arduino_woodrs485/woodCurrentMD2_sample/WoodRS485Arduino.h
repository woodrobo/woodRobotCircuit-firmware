#include "src/wood_rs485/wood_rs485.h"

#pragma once

class WoodRS485Arduino{
public:
  WoodRS485Arduino(uint8_t address, int de_pin);
      
  void sendCommand(WoodRS485Data data);
  bool recvCommand(WoodRS485Data *data, unsigned long timeout_us);

protected:
  int de_pin;
  WoodRS485Manager rs485_manager;
};
