/*
 * joystick.h
 *
 * Created: 23/10/2022 10:23:54 PM
 *  Author: LiamM
 */ 


#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#define CENTRE_X 512
#define CENTRE_Y 512

#define DEADZONE_X	200
#define DEADZONE_Y	200

#define SENSITIVITY_INIT 600
#define SENSITIVITY_MOVE 70

#include <stdint.h>

void init_joystick(void);

void joystick_adc();

void set_axis_hold(uint8_t hold_flag);

void get_joystick_axis(uint16_t *x, uint16_t *y);

uint8_t handle_joystick_move(int8_t *dx, int8_t *dy);

#endif /* JOYSTICK_H_ */