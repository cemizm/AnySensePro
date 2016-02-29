/*
 * Board.h
 *
 *  Created on: 27.02.2016
 *      Author: cem
 */

#ifndef INCLUDE_BOARD_H_
#define INCLUDE_BOARD_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/syscfg.h>

#define FLASH_SPI_MOSI_RCC				RCC_GPIOB
#define FLASH_SPI_MOSI_PORT				GPIOB
#define FLASH_SPI_MOSI_PIN				GPIO5

#define FLASH_SPI_MISO_RCC				RCC_GPIOB
#define FLASH_SPI_MISO_PORT				GPIOB
#define FLASH_SPI_MISO_PIN				GPIO4

#define FLASH_SPI_SCK_RCC				RCC_GPIOB
#define FLASH_SPI_SCK_PORT				GPIOB
#define FLASH_SPI_SCK_PIN				GPIO3

#define FLASH_SPI_CSN_RCC				RCC_GPIOA
#define FLASH_SPI_CSN_PORT				GPIOA
#define FLASH_SPI_CSN_PIN				GPIO15

#define FLASH_SPI_PORT					SPI3
#define FLASH_SPI_RCC					RCC_SPI3
#define FLASH_SPI_AF					GPIO_AF6

#define LED_RED_PORT					GPIOC
#define LED_RED_RCC						RCC_GPIOC
#define LED_RED_PIN						GPIO2

#define LED_ORANGE_PORT					GPIOC
#define LED_ORANGE_RCC					RCC_GPIOC
#define LED_ORANGE_PIN					GPIO3

#define FLASH_TIMEOUT_COEF				3200

#define ROM_START_ADDR					0x08000000

void BootIndicate();
void BootActiveIndicate();
void InitClock();

#endif /* INCLUDE_BOARD_H_ */
