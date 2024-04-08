/*
 * LoRa.h
 *
 *  Created on: 6 lip 2023
 *      Author: ppdra
 */
#include <stdint.h>
#include <main.h>

#ifndef SRC_LORA_H_
#define SRC_LORA_H_

#define NSS_PIN_HIGH 	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_8);
#define NSS_PIN_LOW		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);
//#define RST_PIN_HIGH	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_6);
//#define RST_PIN_LOW		LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_6);



#define SPI_HANDLER	SPI3

typedef enum {
	STATUS_OK = 0,
	STATUS_ERROR = 1
}e_lora_status;

/*
 * RegOpMode Mode Bits
 */

typedef enum {
	SLEEP = 0x00,
	STDBY = 0x01,
	FSTX = 0x02,
	TX = 0x03,
	FSRX = 0x04,
	RXCONTINUOUS = 0x05,
	RXSINGLE = 0x06,
	CAD = 0x07
}e_mode;

/*
 * 	RegModemConfig1 Bandwitdh bits
 */

typedef enum {
	khz_7_8 = 0x00,
	khz_10_4,
	khz_15_7,
	khz_20_8,
	khz_31_25,
	khz_41_7,
	khz_62_5,
	khz_125
}e_bandwidth;

/*
 * 	RegModemConfig1 Error Coding Rate bits
 */

typedef enum {
	CR_4_5 = 0x01,
	CR_4_6,
	CR_4_7,
	CR_4_8,
}e_coding_rate;

/*
 * 	RegModemConfig2 Spreading Factor
 */

typedef enum {
	SF_6 = 0x06,
	SF_7,
	SF_8,
	SF_9,
	SF_10,
	SF_11,
	SF_12,
}e_spreading_factor;












/* Register for Common settngs */
/* RegOpMode Bits */
/* Reset value 0x01 */




#define LOW_FREQ_MODE_ON			(1<<3)
#define LOW_FREQ_MODE_OFF 			~(1<<3)
#define ACCESS_SHARED_REG_FSK		(1<<4)
#define ACCESS_SHARED_REG_LORA		~(1<<4)
#define LONG_RANGE_MODE_LORA		(1<<7)
#define LONG_RANGE_MODE_FSK			~(1<<7)



/* Registers for RF Block */
/* RegPaConfig Bits */
/* Reset value */


/* RegPaRamp Bits */
/* Reset value */











/**** Important Register Addresses ****/
#define ADDR_REGFIFO					0x00
#define ADDR_RegOpMode					0x01
#define ADDR_RegFrMsb					0x06
#define ADDR_RegFrMid					0x07
#define ADDR_RegFrLsb					0x08
#define ADDR_RegPaConfig				0x09
#define ADDR_RegPaRamp					0x0A
#define ADDR_RegOcp						0x0B
#define ADDR_RegLna						0x0C
#define ADDR_RegFiFoAddPtr				0x0D
#define ADDR_RegFiFoTxBaseAddr			0x0E
#define ADDR_RegFiFoRxBaseAddr			0x0F
#define ADDR_RegFiFoRxCurrentAddr		0x10
#define ADDR_RegIrqFlags				0x12
#define ADDR_RegRxNbBytes				0x13
#define ADDR_RegPktRssiValue			0x1A
#define	ADDR_RegModemConfig1			0x1D
#define ADDR_RegModemConfig2			0x1E
#define ADDR_RegSymbTimeoutL			0x1F
#define ADDR_RegPreambleMsb				0x20
#define ADDR_RegPreambleLsb				0x21
#define ADDR_RegPayloadLength			0x22
#define ADDR_RegDioMapping1				0x40
#define ADDR_RegDioMapping2				0x41
#define ADDR_RegVersion					0x42

/**** RegOpMode Bits ****/
#define LONGRANGEMOE_MASK 		1<<7
#define LORAMODE 				LONGRANGEMOE_MASK


void LoRa_Write(uint8_t reg_addr, uint8_t reg_value);
void LoRa_Init();
uint8_t LoRa_Read(uint8_t reg_addr);
void lora_set_spreading_factor(e_spreading_factor SF);
void lora_set_coding_rate(e_coding_rate coding_rate);
void lora_set_bandwidth(e_bandwidth bandwidth);
void lora_transmit_8(uint8_t data, uint8_t length);
uint8_t lora_recieve_8();

#endif /* SRC_LORA_H_ */
