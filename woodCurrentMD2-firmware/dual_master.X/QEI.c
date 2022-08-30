#include <xc.h>
#include "QEI.h"

void QEI_setup(QEI_COUNT_POLARITY polarity){
    QEI1CON = 0x0000 | (polarity << 3);
    QEI1IOC = 0x0000;//QEI1IOCL
    QEI1IOCH = 0x0000;
    QEI1STAT = 0x0000;
    
    QEI_write(0);
    
    QEI1CONbits.QEIEN = 1;
}

void QEI_write(uint32_t count){
    POS1HLD = (count >> 16) & 0xffff;//POS1HLDH
    POS1CNTL = count & 0xffff;
}

uint32_t QEI_read(){
    uint32_t count;
    count = POS1CNTL;
    count |= ((uint32_t)POS1HLD << 16) & 0xffff0000;//POS1HLDH
    return count;
}
