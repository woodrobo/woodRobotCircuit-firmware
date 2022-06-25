#ifndef EYESCONTROLLER_H
#define	EYESCONTROLLER_H

#include "woodLib/Device/RS485Device.h"

namespace Device{

class EyesController : public RS485Device{
public:
    EyesController(uint8_t address);
    
    //motor
    void setEyes(uint8_t mode, uint8_t x, uint8_t y);
    
    void communicate() override;
protected:
    bool is_send;
    uint8_t mode, x, y;
};

}

#endif	/* EYESCONTROLLER_H */

