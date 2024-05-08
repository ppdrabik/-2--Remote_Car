

#include <stdint.h>
#include <main.h>

#ifndef SRC_LORA_H_
#define SRC_LORA_H_

#define NSS_PIN_HIGH 	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_8);
#define NSS_PIN_LOW		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);

#define SPI_HANDLER	SPI3

/*
 * IMPORTANT REGISTER ADDRESSES
 */

#define ADDR_REGFIFO					0x00
#define ADDR_REGOPMODE					0x01
#define ADDR_REGFRMSB					0x06

#define ADDR_REGFRMID					0x07
#define ADDR_REGFRLSB					0x08
#define ADDR_REGPACONFIG				0x09
#define ADDR_REGPARAMP					0x0A
#define ADDR_REGOCP						0x0B
#define ADDR_REGLNA						0x0C
#define ADDR_REGFIFOADDPTR				0x0D
#define ADDR_REGFIFOTXBASEADDR			0x0E
#define ADDR_REGFIFORXBASEADDR			0x0F
#define ADDR_REGFIFORXCURRENTADDR		0x10
#define ADDR_REGIRQFLAGS				0x12
#define ADDR_REGRXNBBYTES				0x13
#define ADDR_REGPKTRSSIVALUE			0x1A
#define	ADDR_REGMODEMCONFIG1			0x1D
#define ADDR_REGMODEMCONFIG2			0x1E
#define ADDR_REGMODEMSTAT				0x18
#define ADDR_REGSYMBTIMEOUTL			0x1F
#define ADDR_REGPREAMBLEMSB				0x20
#define ADDR_REGPREAMBLELSB				0x21
#define ADDR_REGPAYLOADLENGTH			0x22
#define ADDR_REGDIOMAPPING1				0x40
#define ADDR_REGDIOMAPPING2				0x41
#define ADDR_REGVERSION					0x42
#define ADDR_REGRSSIVALUE				0x1B


typedef enum
{
	RESET_BIT,
	SET_BIT,
}bool_e;


typedef enum
{
	SLEEP = 0x00,
	STDBY = 0x01,
	FSTX = 0x02,
	TX = 0x03,
	FSRX = 0x04,
	RXCONTINUOUS = 0x05,
	RXSINGLE = 0x06,
	CAD = 0x07
}mode_e;


typedef enum {
	khz_7_8,
	khz_10_4,
	khz_15_7,
	khz_20_8,
	khz_31_25,
	khz_41_7,
	khz_62_5,
	khz_125,
	khz_250,
	khz_500
}bandwidth_e;


typedef enum {
	CR_4_5 = 0x01,
	CR_4_6,
	CR_4_7,
	CR_4_8,
}coding_rate_e;


typedef enum {
	SF_6 = 0x06,
	SF_7,
	SF_8,
	SF_9,
	SF_10,
	SF_11,
	SF_12,
}spreading_factor_e;


typedef struct
{
	bandwidth_e bandwidth;
	spreading_factor_e sf;
	bool_e pa_boost;
	uint8_t max_power;
	uint8_t output_power;
	uint8_t lna_gain;
}lora_s;



uint8_t lora_read(uint8_t reg_addr);
void lora_write(uint8_t reg_addr, uint8_t reg_value);
void lora_set_spreading_factor(spreading_factor_e sf);
void lora_set_coding_rate(coding_rate_e coding_rate);
void lora_set_bandwidth(bandwidth_e bandwidth);
void lora_set_crc(bool_e set_rest);
void lora_init_transmit();
void lora_init_receive();
void lora_transmit_8(uint8_t *data, uint8_t length);
void lora_recieve_8(int8_t *recieved_data);
void lora_set_pa_boost(bool_e set_rest);
void lora_set_max_output_power(uint8_t set_rest);
void lora_set_output_power(uint8_t set_rest);
void lora_set_lna_gain(uint8_t set_rest);
void lora_init(lora_s *lora);
void start();

#endif /* SRC_LORA_H_ */
