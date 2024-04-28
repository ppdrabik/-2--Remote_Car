/*
 * stering.c
 *
 *  Created on: Apr 10, 2024
 *      Author: ppdra
 */

#include "LoRa.h"
#include "stering.h"
#include "print.h"
/*
 *
 *	IDLE motor
 *	Start motor / Stop motor
 *	Speed of motor
 *	Move up / Move down
 *	Move left / Move right
 *
 *	Data: X-axis, Y-axis
 *
 *
 */

struct axis_s axis;
struct motor_state_s state;
struct duty_cycle_s duty_cycle;

void start()
{
	  get_axis_position(&axis);
	  set_motor_direction(&axis, &state);
	  loop(&state, &duty_cycle);
}

void taillamps_turn_on()
{
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_13);
	LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_14);
}

void taillamps_turn_off()
{
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_13);
	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_14);
}

void move_forward()
{
	LL_GPIO_SetOutputPin(GPIOB, EN1_Pin);
	LL_GPIO_ResetOutputPin(GPIOB, EN2_Pin);
	set_motor_speed(&axis, &state, &duty_cycle);

}

void move_revers()
{
	LL_GPIO_ResetOutputPin(GPIOB, EN1_Pin);
	LL_GPIO_SetOutputPin(GPIOB, EN2_Pin);
	set_motor_speed(&axis, &state, &duty_cycle);
	//taillamps_turn_on();
}

void move_right()
{
	LL_GPIO_SetOutputPin(GPIOB, EN3_Pin);
	LL_GPIO_ResetOutputPin(GPIOB, EN4_Pin);
	set_motor_speed(&axis, &state, &duty_cycle);
}

void move_left()
{
	LL_GPIO_ResetOutputPin(GPIOB, EN3_Pin);
	LL_GPIO_SetOutputPin(GPIOB, EN4_Pin);
	set_motor_speed(&axis, &state, &duty_cycle);
}

void no_move()
{
	set_motor_speed(&axis, &state, &duty_cycle);
}

void set_motor_speed(struct axis_s *axis, struct motor_state_s *state, struct duty_cycle_s *duty_cycle)
{
	if(state->state_up_down == FORWARD)
	{
		duty_cycle->up_down = 70;
		LL_TIM_OC_SetCompareCH1(TIM2,duty_cycle->up_down);
	}
	else if (state->state_up_down == REVERS)
	{
		duty_cycle->up_down = 70;
		LL_TIM_OC_SetCompareCH1(TIM2,duty_cycle->up_down);
;
	}
	else if (state->state_up_down == IDLE)
	{
		duty_cycle->up_down = 0;
		LL_TIM_OC_SetCompareCH1(TIM2,0);
	}

	if (state->state_left_right == RIGHT)
	{
		duty_cycle->left_right = 50;
		LL_TIM_OC_SetCompareCH2(TIM2,duty_cycle->left_right);
	}
	else if (state->state_left_right == LEFT)
	{
		duty_cycle->left_right = 50;
		LL_TIM_OC_SetCompareCH2(TIM2,duty_cycle->left_right);
	}
	else if (state->state_left_right == IDLE)
	{
		duty_cycle->left_right = 0; // RIGHT
		LL_TIM_OC_SetCompareCH2(TIM2,0);
	}
}


void set_motor_direction(struct axis_s *axis, struct motor_state_s *state)
{

	if(axis->y == 1)
	{
		state->state_up_down = FORWARD;
	}
	else if (axis->y == -1)
	{
		state->state_up_down = REVERS;
	}
	else
	{
		state->state_up_down = IDLE;
	}


	if(axis->x == 1)
	{
		state->state_left_right = RIGHT;;
	}
	else if (axis->x == -1)
	{
		state->state_left_right = LEFT;
	}
	else{
		state->state_left_right = IDLE;
	}
	//printf("Motor state up: %d\n\r", state->state_up_down);
	//printf("Motor state left: %d\n\r", state->state_left_right);
}


void get_axis_position(struct axis_s *axis)
{

	lora_recieve_8(axis->recieved_data);
	axis->x = axis->recieved_data[0];
	axis->y = axis->recieved_data[1];
	//lora_transmit_8(&ack, 1);
	printf("Axis x: %d\n\r", axis->x);
	printf("Axis y: %d\n\r", axis->y);
}


void loop(struct motor_state_s *state, struct duty_cycle_s *duty_cycle)
{
	switch (state->state_up_down)
	{
	case IDLE:
		no_move();
		printf("IDLE:%u \n\r", duty_cycle->up_down);
		break;

	case FORWARD:
		move_forward();
		printf("FORWARD:%u \n\r", duty_cycle->up_down);
		break;

	case REVERS:
		move_revers();
		printf("REVERS:%u \n\r", duty_cycle->up_down);
		break;

	default: break;
	}

	switch (state->state_left_right)
	{
	case IDLE:
		no_move();
		printf("IDLE:%u \n\r", duty_cycle->left_right);
		break;

	case RIGHT:
		move_right();
		printf("RIGHT:%u \n\r", duty_cycle->left_right);
		break;

	case LEFT:
		move_left();
		printf("LEFT:%u \n\r", duty_cycle->left_right);
		break;

	default: break;
	}
}
