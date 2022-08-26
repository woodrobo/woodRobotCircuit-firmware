#ifndef MOTOR_CONTROLLER_H
#define	MOTOR_CONTROLLER_H

#include <stdbool.h>

extern float POWER_VCC;
extern float MAX_CURRENT_IGAIN_VOLTAGE;
extern float MOTOR_RESISTANCE;
extern float CURRENT_PGAIN;
extern float CURRENT_IGAIN;

extern float current_deviation_integral;

void motorVoltageController(bool on, signed int pwm);
void motorCurrentController(signed int target_current, signed int now_current);
void motorCurrentControllerReset();

#endif	/* MOTOR_CONTROLLER_H */

