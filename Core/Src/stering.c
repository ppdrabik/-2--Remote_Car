
#include "LoRa.h"
#include "stering.h"
#include "print.h"


axis_s axis;
motor_state_s state;
duty_cycle_s duty_cycle;


void start()
{
	  get_axis_position(&axis);
	  set_motor_direction(&axis, &state);
	  loop(&state, &duty_cycle);
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

void set_motor_speed(axis_s *axis, motor_state_s *state, duty_cycle_s *duty_cycle)
{
	if(state->up_down == UP)
	{
		duty_cycle->up_down = 70;
		LL_TIM_OC_SetCompareCH1(TIM2, duty_cycle->up_down);
	}
	else if (state->up_down == DOWN)
	{
		duty_cycle->up_down = 70;
		LL_TIM_OC_SetCompareCH1(TIM2, duty_cycle->up_down);
;
	}
	else if (state->up_down == IDLE)
	{
		duty_cycle->up_down = 0;
		LL_TIM_OC_SetCompareCH1(TIM2, 0);
	}

	if (state->left_right == RIGHT)
	{
		duty_cycle->left_right = 50;
		LL_TIM_OC_SetCompareCH2(TIM2, duty_cycle->left_right);
	}
	else if (state->left_right == LEFT)
	{
		duty_cycle->left_right = 50;
		LL_TIM_OC_SetCompareCH2(TIM2, duty_cycle->left_right);
	}
	else if (state->left_right == IDLE)
	{
		duty_cycle->left_right = 0; // RIGHT
		LL_TIM_OC_SetCompareCH2(TIM2, 0);
	}
}


void set_motor_direction(axis_s *axis, motor_state_s *state)
{
	if(axis->y == 1)
	{
		state->up_down = UP;
	}
	else if (axis->y == -1)
	{
		state->up_down = DOWN;
	}
	else
	{
		state->up_down = IDLE;
	}

	if(axis->x == 1)
	{
		state->left_right = RIGHT;;
	}
	else if (axis->x == -1)
	{
		state->left_right = LEFT;
	}
	else
	{
		state->left_right = IDLE;
	}

	//printf("Motor state up: %d\n\r", state->up_down);
	//printf("Motor state left: %d\n\r", state->left_right);
}


void get_axis_position(axis_s *axis)
{
	lora_recieve_8(axis->recieved_data);
	axis->x = axis->recieved_data[0];
	axis->y = axis->recieved_data[1];

	//printf("Axis x: %d\n\r", axis->x);
	//printf("Axis y: %d\n\r", axis->y);
}


void loop(motor_state_s *state, duty_cycle_s *duty_cycle)
{
	switch (state->up_down)
	{
	case IDLE:
		no_move();
		printf("IDLE:%u \n\r", duty_cycle->up_down);
		break;

	case UP:
		move_forward();
		printf("UP:%u \n\r", duty_cycle->up_down);
		break;

	case DOWN:
		move_revers();
		printf("DOWN:%u \n\r", duty_cycle->up_down);
		break;

	default: break;
	}

	switch (state->left_right)
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
