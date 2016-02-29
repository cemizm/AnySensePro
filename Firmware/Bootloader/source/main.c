/*
 * main.c
 *
 *  Created on: 26.02.2016
 *      Author: cem
 */

#include "Board.h"
#include "M25P16.h"
#include "spiffs.h"
#include "libopencm3/cm3/scb.h"
#include "libopencm3/stm32/flash.h"

int8_t mount();

u8_t work_buf[FLASH_PAGE_SIZE * 2];
u8_t fds[32 * 4];

#define UPDATE_BUFFER_SIZE			2048

uint8_t update_buffer[UPDATE_BUFFER_SIZE];
uint16_t* range = (uint16_t*) (update_buffer + UPDATE_BUFFER_SIZE);

spiffs fs;

void doCheckUpdate();
int8_t mount();

int main()
{
	InitClock();
	BootIndicate();

	doCheckUpdate();

	scb_reset_system();

}

void doCheckUpdate()
{
	if (flash_init() != SPIFFS_OK)
		return;

	if (mount() != SPIFFS_OK)
		return;

	spiffs_file fd = SPIFFS_open(&fs, "fw", SPIFFS_RDONLY, 0);

	if (fd < 0)
		return;

	spiffs_stat st;

	if (SPIFFS_fstat(&fs, fd, &st) < 0)
	{
		SPIFFS_close(&fs, fd);
		return;
	}

	uint16_t* arr;
	uint16_t size = UPDATE_BUFFER_SIZE;
	uint32_t addr = ROM_START_ADDR;

	flash_unlock();

	while (st.size > 0)
	{
		if (st.size < size)
			size = st.size;

		memset(update_buffer, 0x00, UPDATE_BUFFER_SIZE);

		SPIFFS_read(&fs, fd, update_buffer, size);

		flash_erase_page(addr);
		for (arr = (uint16_t*) update_buffer; arr < range; arr++)
		{
			flash_program_half_word(addr, *arr);
			addr += 2;
		}

		st.size -= size;

		BootActiveIndicate();
	}

	flash_lock();

	SPIFFS_close(&fs, fd);
}

int8_t mount()
{
	spiffs_config cfg;
	cfg.hal_read_f = flash_read;
	cfg.hal_write_f = flash_write;
	cfg.hal_erase_f = flash_erase;

	return SPIFFS_mount(&fs, &cfg, work_buf, fds, sizeof(fds), 0, 0, 0);
}
