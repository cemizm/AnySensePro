/*
 * M25P16.c
 *
 *  Created on: 27.02.2016
 *      Author: cem
 */

#include "M25P16.h"
#include "Board.h"

void initHW()
{
	//CSN
	rcc_periph_clock_enable(FLASH_SPI_CSN_RCC);
	gpio_mode_setup(FLASH_SPI_CSN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, FLASH_SPI_CSN_PIN);
	gpio_set_output_options(FLASH_SPI_CSN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, FLASH_SPI_CSN_PIN);
	gpio_set(FLASH_SPI_CSN_PORT, FLASH_SPI_CSN_PIN);

	//MOSI
	rcc_periph_clock_enable(FLASH_SPI_MOSI_RCC);
	gpio_mode_setup(FLASH_SPI_MOSI_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, FLASH_SPI_MOSI_PIN);
	gpio_set_output_options(FLASH_SPI_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, FLASH_SPI_MOSI_PIN);
	gpio_set_af(FLASH_SPI_MOSI_PORT, FLASH_SPI_AF, FLASH_SPI_MOSI_PIN);

	//MISO
	rcc_periph_clock_enable(FLASH_SPI_MISO_RCC);
	gpio_mode_setup(FLASH_SPI_MISO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, FLASH_SPI_MISO_PIN);
	gpio_set_output_options(FLASH_SPI_MISO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, FLASH_SPI_MISO_PIN);
	gpio_set_af(FLASH_SPI_MISO_PORT, FLASH_SPI_AF, FLASH_SPI_MISO_PIN);

	//SCK
	rcc_periph_clock_enable(FLASH_SPI_SCK_RCC);
	gpio_mode_setup(FLASH_SPI_SCK_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, FLASH_SPI_SCK_PIN);
	gpio_set_output_options(FLASH_SPI_SCK_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, FLASH_SPI_SCK_PIN);
	gpio_set_af(FLASH_SPI_SCK_PORT, FLASH_SPI_AF, FLASH_SPI_SCK_PIN);

	//SPI
	rcc_periph_clock_enable(FLASH_SPI_RCC);
	spi_init_master(FLASH_SPI_PORT, SPI_CR1_BAUDRATE_FPCLK_DIV_2, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1,
	SPI_CR2_DS_8BIT, SPI_CR1_MSBFIRST);

	spi_set_unidirectional_mode(FLASH_SPI_PORT);
	spi_set_full_duplex_mode(FLASH_SPI_PORT);

	spi_enable_software_slave_management(FLASH_SPI_PORT);
	spi_disable_crc(FLASH_SPI_PORT);

	spi_fifo_reception_threshold_8bit(FLASH_SPI_PORT);

	spi_enable(FLASH_SPI_PORT);
}

inline void select()
{
	gpio_clear(FLASH_SPI_CSN_PORT, FLASH_SPI_CSN_PIN);
}

inline void deselect()
{
	gpio_set(FLASH_SPI_CSN_PORT, FLASH_SPI_CSN_PIN);
}

inline uint8_t xfer8(uint8_t data)
{
	while (!(SPI_SR(FLASH_SPI_PORT) & SPI_SR_TXE))
		;

	SPI_DR8(FLASH_SPI_PORT) = data;

	/* Wait for transfer finished. */
	while (!(SPI_SR(FLASH_SPI_PORT) & SPI_SR_RXNE))
		;

	return SPI_DR8(FLASH_SPI_PORT);
}

int32_t flash_init()
{
	initHW();
	select();

	xfer8(CMD_ReadId);

	xfer8(0xFF); //Manufacturer
	uint16_t deviceID = xfer8(0xFF) << 8; //Device ID
	deviceID |= xfer8(0xFF);

	deselect();

	return deviceID != 0x2015 ? FLASH_ERROR : FLASH_OK;
}

uint8_t waitReady(uint16_t timeout)
{
	select();

	uint8_t d = 0x00;
	uint64_t i = 0;
	uint64_t dTimeout = timeout * FLASH_TIMEOUT_COEF;

	xfer8(CMD_ReadStatus);

	do
	{
		d = xfer8(0xFF);
		i++;
	} while (((d & 0x01) == 0x01) && i < dTimeout);

	deselect();

	return (d & 0x01) == 0x00 ? 1 : 0;
}

uint8_t writePage(uint32_t addr, uint32_t size, const uint8_t* buff)
{
	select();

	xfer8(CMD_WriteEnable);

	deselect();
	select();

	xfer8(CMD_PageProgram);
	xfer8(addr >> 16);
	xfer8(addr >> 8);
	xfer8(addr);

	for (uint32_t i = 0; i < size; i++)
		xfer8(buff[i]);

	deselect();

	return 1;
}

int32_t flash_read(uint32_t addr, uint32_t size, uint8_t* data)
{
	select();

	xfer8(CMD_FastReadData);
	xfer8(addr >> 16); // Address
	xfer8(addr >> 8);
	xfer8(addr);

	xfer8(0xFF); // Dummy

	for (uint32_t i = 0; i < size; i++)
		data[i] = xfer8(0xFF);

	deselect();

	return FLASH_OK;
}

int32_t flash_write(uint32_t addr, uint32_t size, uint8_t* data)
{
	uint32_t buff_size = FLASH_PAGE_SIZE;
	do
	{
		buff_size = size > FLASH_PAGE_SIZE ? FLASH_PAGE_SIZE : size;

		if (!writePage(addr, buff_size, data))
			break;

		if (!waitReady(10))
			break;

		addr += buff_size;
		data += buff_size;
		size -= buff_size;

	} while (size > 0);

	return size > 0 ? FLASH_ERROR : FLASH_OK;

}

int32_t flash_erase(uint32_t addr, uint32_t size)
{
	(void) size;

	select();

	xfer8(CMD_WriteEnable);

	deselect();
	select();

	xfer8(CMD_SectorErase);
	xfer8(addr >> 16); // Address
	xfer8(addr >> 8);
	xfer8(addr);

	deselect();

	if (!waitReady(3100))
		return FLASH_ERROR;

	return FLASH_OK;
}
