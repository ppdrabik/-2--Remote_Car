/*
 * LoRa.c
 *
 *  Created on: 6 lip 2023
 *      Author: ppdra
 */

#include "LoRa.h"


static void SPI_Write_8(uint8_t data);
static uint8_t SPI_Read_8(uint8_t data);


static void SPI_Write_8(uint8_t data)
{
	/* wait for empty buffer */
	while (!(LL_SPI_IsActiveFlag_TXE(SPI_HANDLER))){}
		LL_SPI_TransmitData8(SPI_HANDLER, data);
	/* wait for TX buffer to clear */
	while (LL_SPI_GetTxFIFOLevel(SPI_HANDLER) != LL_SPI_TX_FIFO_EMPTY){};
	/* check busy flag */
	while (LL_SPI_IsActiveFlag_BSY(SPI_HANDLER) != 0);
	/* wait for RX buffer to fill */
	while (LL_SPI_GetRxFIFOLevel(SPI_HANDLER) != LL_SPI_RX_FIFO_EMPTY)
	{
		/* clear received data */
		LL_SPI_ReceiveData8(SPI_HANDLER);
	}
}


static uint8_t SPI_Read_8(uint8_t reg_address)
{
	uint8_t temp = 0;
	/* write address of register to read */
	SPI_Write_8(reg_address);
	/* send dummy byte to generate clock signal and read data from register */
	while (!(LL_SPI_IsActiveFlag_TXE(SPI_HANDLER))){}
	LL_SPI_TransmitData8(SPI_HANDLER, 0xFF);
	while (LL_SPI_GetTxFIFOLevel(SPI_HANDLER) != LL_SPI_TX_FIFO_EMPTY){};
		/* check busy flag */
		while (LL_SPI_IsActiveFlag_BSY(SPI_HANDLER) != 0);
		/* wait for RX buffer to fill */
		while (LL_SPI_GetRxFIFOLevel(SPI_HANDLER) != LL_SPI_RX_FIFO_EMPTY)
		{
			/* clear received data */
			temp = LL_SPI_ReceiveData8(SPI_HANDLER);
		}
		return temp;
	}


void LoRa_Write(uint8_t reg_addr, uint8_t reg_value)
{
	NSS_PIN_LOW
	uint8_t write_addr = 0;
	write_addr = reg_addr | 0x80;
	SPI_Write_8(write_addr);
	SPI_Write_8(reg_value);
	NSS_PIN_HIGH
}


uint8_t LoRa_Read(uint8_t reg_addr)
{
	uint8_t addr = 0;
	uint8_t temp = 0;
	addr = reg_addr & 0x7F;
	NSS_PIN_LOW
	temp = SPI_Read_8(addr);
	NSS_PIN_HIGH
	return temp;
}


void LoRa_Mode(e_mode mode)
{

	/* mask 0xf8 */

	uint8_t read_value = 0;
	read_value = LoRa_Read(ADDR_RegOpMode);
	read_value = read_value & 0xf8; /* set bit that you want to change to 0 */
	read_value = read_value | mode; /* add bits you want */
	LoRa_Write(ADDR_RegOpMode, read_value);
}

void lora_set_bandwidth(e_bandwidth bandwidth)
{
	uint8_t read_value = 0;
	read_value = LoRa_Read(ADDR_RegModemConfig1);
	read_value = read_value & 0x0f;
	read_value = read_value | bandwidth<<4;
	LoRa_Write(ADDR_RegModemConfig1, read_value);
}

void lora_set_coding_rate(e_coding_rate coding_rate)
{
	uint8_t read_value = 0;
	read_value = LoRa_Read(ADDR_RegModemConfig1);
	read_value = read_value & 0xf1;
	read_value = read_value | coding_rate<<1;
	LoRa_Write(ADDR_RegModemConfig1, read_value);
}

void lora_set_spreading_factor(e_spreading_factor SF)
{
	uint8_t read_value = 0;
	read_value = LoRa_Read(ADDR_RegModemConfig2);
	read_value = read_value & 0x0f;
	read_value = read_value | SF<<4;
	LoRa_Write(ADDR_RegModemConfig2, read_value);
}

void lora_transmit_8(uint8_t data, uint8_t length)
{
	uint8_t txaddr = 0;
	LoRa_Mode(STDBY);
	LoRa_Write(ADDR_RegIrqFlags, 0xFF);
	txaddr = LoRa_Read(ADDR_RegFiFoTxBaseAddr);
	LoRa_Write(ADDR_RegFiFoAddPtr, txaddr);
	LoRa_Write(ADDR_RegPayloadLength, length);
	LoRa_Write(ADDR_REGFIFO, data);
	LoRa_Mode(TX);
	while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) == 0){};
}


uint8_t lora_recieve_8()
{
	uint8_t rxflag = 0;
	uint8_t recieve_data = 0;
	LoRa_Mode(STDBY);
	LoRa_Write(ADDR_RegFiFoAddPtr, 0x00);
	LoRa_Mode(RXSINGLE);
	rxflag = LoRa_Read(ADDR_RegIrqFlags);
	while((rxflag&0x40)>>6 == 0)
	{
		rxflag = LoRa_Read(ADDR_RegIrqFlags);
	}
	recieve_data = LoRa_Read(ADDR_REGFIFO);
	return recieve_data;
}


void LoRa_Init()
{
	/*
	 * Carrier Frequency = 434 MHz (default state)
	 * Bandwidth = 10.4 kHz
	 * Coding Rate = 4/5
	 * SF Rate = 12
	 * Header = Implicit
	 * Preamble length =
	 *
	 *
	 *
	 */
	LoRa_Mode(SLEEP);
	/* SET LORA MODE */
	LoRa_Write(ADDR_RegOpMode, 0x88);
	/* Interrupt mapping */
	LoRa_Write(ADDR_RegDioMapping1, 0x40);
	lora_set_bandwidth(khz_125);
	lora_set_spreading_factor(SF_12);
	/* SET CRC enable */
	LoRa_Write(ADDR_RegModemConfig2, 0xC4);
}



