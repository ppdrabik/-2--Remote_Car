/*
 * LoRa.c
 *
 *  Created on: 6 lip 2023
 *      Author: ppdra
 */

#include "LoRa.h"

static void spi_write_8(uint8_t data);
static uint8_t spi_read_8(uint8_t data);

static void spi_write_8(uint8_t data)
{
	while (!(LL_SPI_IsActiveFlag_TXE(SPI_HANDLER))){} 	/* wait for tx buffer to clear */
		LL_SPI_TransmitData8(SPI_HANDLER, data);		/* write data */
	while (LL_SPI_GetRxFIFOLevel(SPI_HANDLER) != LL_SPI_RX_FIFO_EMPTY)
	{
		/* clear received data */
		LL_SPI_ReceiveData8(SPI_HANDLER);
	}
}

static uint8_t spi_read_8(uint8_t reg_address)
{
	uint8_t temp = 0;
	/* write address of register to read */
	spi_write_8(reg_address);
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


void lora_write(uint8_t reg_addr, uint8_t reg_value)
{
	NSS_PIN_LOW
	uint8_t write_addr = 0;
	write_addr = reg_addr | 0x80;
	spi_write_8(write_addr);
	spi_write_8(reg_value);
	NSS_PIN_HIGH
}


uint8_t lora_read(uint8_t reg_addr)
{
	uint8_t addr = 0;
	uint8_t temp = 0;
	addr = reg_addr & 0x7F;
	NSS_PIN_LOW
	temp = spi_read_8(addr);
	NSS_PIN_HIGH
	return temp;
}


void lora_mode(e_mode mode)
{
	/* mask 0xf8 */
	uint8_t read_value = 0;
	read_value = lora_read(ADDR_RegOpMode);
	read_value = read_value & 0xf8; /* set bit that you want to change to 0 */
	read_value = read_value | mode; /* add bits you want */
	lora_write(ADDR_RegOpMode, read_value);
}

void lora_set_bandwidth(e_bandwidth bandwidth)
{
	uint8_t read_value = 0;
	read_value = lora_read(ADDR_RegModemConfig1);
	read_value = read_value & 0x0f;
	read_value = read_value | bandwidth<<4;
	lora_write(ADDR_RegModemConfig1, read_value);
}

void lora_set_coding_rate(e_coding_rate coding_rate)
{
	uint8_t read_value = 0;
	read_value = lora_read(ADDR_RegModemConfig1);
	read_value = read_value & 0xf1;
	read_value = read_value | coding_rate<<1;
	lora_write(ADDR_RegModemConfig1, read_value);
}

void lora_set_spreading_factor(e_spreading_factor SF)
{
	uint8_t read_value = 0;
	read_value = lora_read(ADDR_RegModemConfig2);
	read_value = read_value & 0x0f;
	read_value = read_value | SF<<4;
	lora_write(ADDR_RegModemConfig2, read_value);
}

void lora_set_crc(e_bool set_rest)
{
  uint8_t read_value = 0;
  read_value = lora_read(ADDR_RegModemConfig2);
  if(set_rest == SETT)
  {
    read_value = read_value | 0x04;
  }
  else
  {
    read_value = read_value & 0xfb;
  }
  lora_write(ADDR_RegModemConfig2, read_value);
}


/*
 * REG: RegPaConfig(0x09)
 * BIT: 7
 * PA_BOOST: SET - Max Output Power limited to +20dBm
 * PA_BOOST: RESET - Max Output Power limited to +14dBm
 */
void lora_set_pa_boost(e_bool set_rest)
{
  uint8_t read_value = 0;
  read_value = lora_read(ADDR_RegPaConfig);
  if(set_rest == SETT)
  {
    read_value = read_value | (1<<7);
  }
  else
  {
    read_value = read_value & ~(1<<7);
  }
  lora_write(ADDR_RegPaConfig, read_value);
}


/*
 * REG: RegPaConfig(0x09)
 * BIT: 6-4
 * Set Max Output Power: 0 to 7
 */
void lora_set_max_output_power(uint8_t set_rest)
{
	if(set_rest <= 7)
	{
		  uint8_t read_value = 0;
		  read_value = lora_read(ADDR_RegPaConfig);
		  read_value = read_value | (set_rest)<<4;
		  lora_write(ADDR_RegPaConfig, read_value);
	}
}

/*
 * REG: RegPaConfig(0x09)
 * BIT: 3-0
 * Set Max Output Power: 0 to 15
 */
void lora_set_output_power(uint8_t set_rest)
{
	if(set_rest <= 15)
	{
		uint8_t read_value = 0;
		read_value = lora_read(ADDR_RegPaConfig);
		read_value = read_value | (set_rest);
		lora_write(ADDR_RegPaConfig, read_value);
	}

}

/*
 * REG: RegLna(0x0C)
 * BIT: 7-5
 * Set Max Output Power: 0 to 7
 */
void lora_set_lna_gain(uint8_t set_rest)
{
	if(set_rest <= 7)
	{
		uint8_t read_value = 0;
		read_value = lora_read(ADDR_RegLna);
		read_value = read_value | (set_rest)<<5;
		lora_write(ADDR_RegLna, read_value);
	}

}

void lora_get_rssi(uint8_t *data)
{
	int8_t read_value = 0;
	read_value = lora_read(ADDR_RegRssiValue);
	*data = read_value;
}


void lora_init_transmit()
{
    lora_write(ADDR_RegDioMapping1, 0x40);  /* Map Pin DIO0 for interrupt when tx done */
    lora_write(ADDR_RegIrqFlags, 0xFF);     /* Clear interrupts flags */
    lora_set_crc(SET); /* SET CRC enable */
}

void lora_transmit_8(uint8_t *data, uint8_t length)
{
		lora_init_transmit();
	    uint8_t fifo_current_address = 0;
	    lora_mode(STDBY);
	    fifo_current_address = lora_read(ADDR_RegFiFoTxBaseAddr);   /* Read pointer from where data to Tx will be stored */
	    lora_write(ADDR_RegFiFoAddPtr, fifo_current_address);       /* Write address of pointer */
	    lora_write(ADDR_RegPayloadLength, length);                  /* Write data length */
	    for (uint8_t i = 0; i < length; i++)
	    {
	        lora_write(ADDR_REGFIFO, data[i]);                      /* Write data to FIFO, Pointer increment itself*/
	    }
	    lora_mode(TX);
	    while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) == 0){};                     /* Check if TX is done */
}

void lora_init_receive()
{
    lora_write(ADDR_RegDioMapping1, 0x00);  /* Map Pin DIO0 for interrupt when rx done */
    lora_write(ADDR_RegIrqFlags, 0xFF);     /* Clear interrupts flags */
	lora_set_crc(RESETT);
}



void lora_recieve_8(int8_t *recieved_data)
	{
		lora_init_receive();
	  uint8_t fifo_current_address = 0;
	  uint8_t bytes_received = 0;

	  lora_mode(STDBY);
	  fifo_current_address = lora_read(ADDR_RegFiFoRxBaseAddr);
	  lora_write(ADDR_RegFiFoAddPtr, fifo_current_address);
	  lora_mode(RXCONTINUOUS);


	  while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) == 0){};

	  bytes_received = lora_read(ADDR_RegRxNbBytes);
	  fifo_current_address = lora_read(ADDR_RegFiFoRxBaseAddr);
	  lora_write(ADDR_RegFiFoAddPtr, fifo_current_address);
	  for (uint8_t i = 0; i < bytes_received; i++)
	    {
	        recieved_data[i] = lora_read(ADDR_REGFIFO);                    /* Write data to FIFO, Pointer increment itself*/
	    }
	  lora_write(ADDR_RegIrqFlags, 0xFF);
	}



void LoRa_Init()
{
	/*
	 * Carrier Frequency = 434 MHz (default state)
	 * Bandwidth = 10.4 kHz
	 * Coding Rate = 4/5
	 * SF Rate = 12
	 * Header = Explicit
	 * Preamble length = 8
	 *
	 */
	lora_mode(SLEEP);
	lora_write(ADDR_RegOpMode, 0x88); 			/* SET LORA MODE */
	lora_set_bandwidth(khz_250);
	lora_set_spreading_factor(SF_10);
	lora_set_pa_boost(SETT);
	lora_set_max_output_power(3);
	lora_set_output_power(14);
	lora_set_lna_gain(6);
}



