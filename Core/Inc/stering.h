/*
 * stering.h
 *
 *  Created on: Apr 10, 2024
 *      Author: ppdra
 */

#ifndef INC_STERING_H_
#define INC_STERING_H_

#include <stdint.h>
#include <main.h>


typedef enum
{
	IDLE,
	UP,
	LEFT,
	RIGHT,
	DOWN
}state_e;


typedef struct
{
	int8_t x;
	int8_t y;
	int8_t recieved_data[2];
}axis_s;


typedef struct
{
	state_e up_down;
	state_e left_right;
}motor_state_s;


typedef struct
{
	uint8_t up_down;
	uint8_t left_right;
}duty_cycle_s;


void loop(motor_state_s *state, duty_cycle_s *duty_cycle);
void start();
void move_up();
void move_down();
void move_right();
void move_left();
void no_move();
void set_motor_speed(axis_s *axis, motor_state_s *state, duty_cycle_s *duty_cycle);
void set_motor_direction(axis_s *axis, motor_state_s *state);
void get_axis_position(axis_s *axis);


#endif /* INC_STERING_H_ */
