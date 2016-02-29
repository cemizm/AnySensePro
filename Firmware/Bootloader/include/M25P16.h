/*
 * M25P16.h
 *
 *  Created on: 27.02.2016
 *      Author: cem
 */

#ifndef INCLUDE_M25P16_H_
#define INCLUDE_M25P16_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#define FLASH_PAGE_SIZE        			256UL
#define FLASH_PAGES_PER_SECTORS   		256UL
#define FLASH_SECTORS          			32UL
#define FLASH_SECTOR_SIZE				(FLASH_PAGE_SIZE * FLASH_PAGES_PER_SECTORS)
#define FLASH_SIZE 		            	(FLASH_SECTOR_SIZE * FLASH_SECTORS)
#define FLASH_START						0UL

enum cmd_t
{
	CMD_WriteEnable = 0x06,
	CMD_WriteDisable = 0x04,
	CMD_ReadId = 0x9F,
	CMD_ReadStatus = 0x05,
	CMD_WriteStatus = 0x01,
	CMD_ReadData = 0x03,
	CMD_FastReadData = 0x0B,
	CMD_PageProgram = 0x02,
	CMD_SectorErase = 0xD8,
	CMD_BlukErase = 0xC7,
	CMD_DeepPowerDown = 0xB9,
	CMD_ReleasePowerDown = 0xAB,
};

#define FLASH_OK						0
#define FLASH_ERROR						-10050

void select();
void deselect();
uint8_t xfer8(uint8_t);

int32_t flash_read(uint32_t addr, uint32_t size, uint8_t* data);
int32_t flash_write(uint32_t addr, uint32_t size, uint8_t* data);
int32_t flash_erase(uint32_t addr, uint32_t size);

int32_t flash_init();
uint8_t waitReady(uint16_t timeout);
uint8_t writePage(uint32_t addr, uint32_t size, const uint8_t* buff);


#endif /* INCLUDE_M25P16_H_ */
