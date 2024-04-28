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
	FORWARD,
	LEFT,
	RIGHT,
	REVERS
}state_e;

struct axis_s
{
	int8_t x;
	int8_t y;
	int8_t recieved_data[2];
};

struct motor_state_s
{
	state_e state_up_down;
	state_e state_left_right;
	state_e current_state;
};

struct duty_cycle_s
{
	uint8_t up_down;
	uint8_t left_right;
};

void taillamps_turn_on();
void taillamps_turn_off();
void move_forward();
void move_revers();
void move_right();
void move_left();
void no_move();
void set_motor_speed(struct axis_s *axis, struct motor_state_s *state, struct duty_cycle_s *duty_cycle);
void set_motor_direction(struct axis_s *axis, struct motor_state_s *state);
void get_axis_position(struct axis_s *axis);
void loop(struct motor_state_s *state, struct duty_cycle_s *duty_cycle);
void start();

#endif /* INC_STERING_H_ */
