#ifndef MOTOR_CONTROLLER_H
#define	MOTOR_CONTROLLER_H

#include <stdbool.h>

void motorVoltageController(bool on, signed int pwm);
void motorCurrentController(signed int current);

#endif	/* MOTOR_CONTROLLER_H */

