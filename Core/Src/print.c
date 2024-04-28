/*
 * print.c
 *
 *  Created on: Apr 20, 2024
 *      Author: ppdra
 */
#include "print.h"


void Transmit_uart(uint8_t *data_buf, uint8_t data_size)
 {
	for (int var = 0; var < data_size; ++var)
	{
		 while (!LL_LPUART_IsActiveFlag_TXE(LPUART1)){};
		  LL_LPUART_TransmitData8(LPUART1, data_buf[var]);
	}
 }

int __io_putchar(int ch)
{
	Transmit_uart((uint8_t*)&ch, 1);
	return ch;
}
