


#include "LoRa.h"

static void spi_write_8(uint8_t data);
static uint8_t spi_read_8(uint8_t data);


static void spi_write_8(uint8_t data)
{
	while (!(LL_SPI_IsActiveFlag_TXE(SPI_HANDLER))){} 	/* Wait for Tx buffer to empty */
		LL_SPI_TransmitData8(SPI_HANDLER, data);
	while (LL_SPI_GetRxFIFOLevel(SPI_HANDLER) != LL_SPI_RX_FIFO_EMPTY)
	{
		LL_SPI_ReceiveData8(SPI_HANDLER); 				/* Clear received data */
	}
}


static uint8_t spi_read_8(uint8_t reg_address)
{
	uint8_t temp = 0;
	spi_write_8(reg_address);
	while (!(LL_SPI_IsActiveFlag_TXE(SPI_HANDLER))){}
	LL_SPI_TransmitData8(SPI_HANDLER, 0xFF); 	/* send dummy byte to
											 	 generate clock signal and read data from register */

	while (LL_SPI_GetTxFIFOLevel(SPI_HANDLER) != LL_SPI_TX_FIFO_EMPTY){}; /* Wait for  */

	while (LL_SPI_IsActiveFlag_BSY(SPI_HANDLER) != 0);
	while (LL_SPI_GetRxFIFOLevel(SPI_HANDLER) != LL_SPI_RX_FIFO_EMPTY) /* check busy flag */
		{
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


void lora_mode(mode_e mode)
{
	/* mask 0xf8 */
	uint8_t read_value = 0;
	read_value = lora_read(ADDR_REGOPMODE);
	read_value = read_value & 0xf8; /* set bit that you want to change to 0 */
	read_value = read_value | mode; /* add bits you want */
	lora_write(ADDR_REGOPMODE, read_value);
}

void lora_set_bandwidth(bandwidth_e bandwidth)
{
	uint8_t read_value = 0;
	read_value = lora_read(ADDR_REGMODEMCONFIG1);
	read_value = read_value & 0x0f;
	read_value = read_value | bandwidth<<4;
	lora_write(ADDR_REGMODEMCONFIG1, read_value);
}

void lora_set_coding_rate(coding_rate_e coding_rate)
{
	uint8_t read_value = 0;
	read_value = lora_read(ADDR_REGMODEMCONFIG1);
	read_value = read_value & 0xf1;
	read_value = read_value | coding_rate<<1;
	lora_write(ADDR_REGMODEMCONFIG1, read_value);
}

void lora_set_spreading_factor(spreading_factor_e SF)
{
	uint8_t read_value = 0;
	read_value = lora_read(ADDR_REGMODEMCONFIG2);
	read_value = read_value & 0x0f;
	read_value = read_value | SF<<4;
	lora_write(ADDR_REGMODEMCONFIG2, read_value);
}

void lora_set_crc(bool_e set_rest)
{
  uint8_t read_value = 0;
  read_value = lora_read(ADDR_REGMODEMCONFIG2);
  if(set_rest == SET_BIT)
  {
    read_value = read_value | 0x04;
  }
  else
  {
    read_value = read_value & 0xfb;
  }
  lora_write(ADDR_REGMODEMCONFIG2, read_value);
}


/*
 * REG: RegPaConfig(0x09)
 * BIT: 7
 * PA_BOOST: SET - Max Output Power limited to +20dBm
 * PA_BOOST: RESET - Max Output Power limited to +14dBm
 */
void lora_set_pa_boost(bool_e set_rest)
{
  uint8_t read_value = 0;
  read_value = lora_read(ADDR_REGPACONFIG);
  if(set_rest == SET_BIT)
  {
    read_value = read_value | (1<<7);
  }
  else
  {
    read_value = read_value & ~(1<<7);
  }
  lora_write(ADDR_REGPACONFIG, read_value);
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
		  read_value = lora_read(ADDR_REGPACONFIG);
		  read_value = read_value | (set_rest)<<4;
		  lora_write(ADDR_REGPACONFIG, read_value);
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
		read_value = lora_read(ADDR_REGPACONFIG);
		read_value = read_value | (set_rest);
		lora_write(ADDR_REGPACONFIG, read_value);
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
		read_value = lora_read(ADDR_REGLNA);
		read_value = read_value | (set_rest)<<5;
		lora_write(ADDR_REGLNA, read_value);
	}
}


void lora_init_transmit()
{
    lora_write(ADDR_REGDIOMAPPING1, 0x40);  /* Map Pin DIO0 for interrupt when TX done */
    lora_write(ADDR_REGIRQFLAGS, 0xFF);     /* Clear interrupts flags */
    lora_set_crc(SET_BIT); 					/* SET CRC enable */
}

void lora_transmit_8(uint8_t *data, uint8_t length)
{
	uint8_t fifo_current_address = 0;
	lora_mode(STDBY);
	fifo_current_address = lora_read(ADDR_REGFIFOTXBASEADDR);   /* Read pointer from where data to Tx will be stored */
	lora_write(ADDR_REGFIFOADDPTR, fifo_current_address);       /* Write address of pointer */
	lora_write(ADDR_REGPAYLOADLENGTH, length);                  /* Write data length */
	for (uint8_t i = 0; i < length; i++)
		{
			lora_write(ADDR_REGFIFO, data[i]);                      /* Write data to FIFO, Pointer increment itself*/
	    }
	lora_mode(TX);
	while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) == 0){};                     /* Check if TX is done */
}

void lora_init_receive()
{
    lora_write(ADDR_REGDIOMAPPING1, 0x00);  /* Map Pin DIO0 for interrupt when RX done */
    lora_write(ADDR_REGIRQFLAGS, 0xFF);     /* Clear interrupts flags */
	lora_set_crc(RESET_BIT);				/* CRC only on TX side in Explicit mode */
}


void lora_recieve_8(int8_t *recieved_data)
{

    uint8_t fifo_current_address = 0;
	uint8_t bytes_received = 0;

	lora_mode(STDBY);
	fifo_current_address = lora_read(ADDR_REGFIFORXBASEADDR);
	lora_write(ADDR_REGFIFOADDPTR, fifo_current_address);
	lora_mode(RXCONTINUOUS);

	while(LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6) == 0){}; 	/* Wait for RX done interrupt */

	bytes_received = lora_read(ADDR_REGRXNBBYTES);
	fifo_current_address = lora_read(ADDR_REGFIFORXBASEADDR);
	lora_write(ADDR_REGFIFOADDPTR, fifo_current_address);
	for (uint8_t i = 0; i < bytes_received; i++)
	{
		recieved_data[i] = lora_read(ADDR_REGFIFO);     		/* Write data to FIFO, Pointer to next data
																   increment itself		*/
	}
	lora_write(ADDR_REGIRQFLAGS, 0xFF);
}


void lora_init(lora_s *lora)
{
	/*
	 * Header = Explicit
	 * Preamble length = 8
	 *
	 */

	lora_mode(SLEEP);
	lora_write(ADDR_REGOPMODE, 0x88); 		/* Set LoRa Mode */
	lora_set_bandwidth(lora->bandwidth);
	lora_set_spreading_factor(lora->sf);
	lora_set_pa_boost(lora->pa_boost);
	lora_set_max_output_power(lora->max_power);
	lora_set_output_power(lora->output_power);
	lora_set_lna_gain(lora->lna_gain);
}
