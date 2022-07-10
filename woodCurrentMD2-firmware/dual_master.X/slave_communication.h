#ifndef SLAVE_COMMUNICATION_H
#define	SLAVE_COMMUNICATION_H

typedef enum{
    SLAVE_MODE_VOLTAGE = 0,
    SLAVE_MODE_CURRENT = 1,
    SLAVE_MODE_FREE = 2
}SLAVE_MODE;

void motorOutput(SLAVE_MODE mode, signed int power);

#endif	/* SLAVE_COMMUNICATION_H */

