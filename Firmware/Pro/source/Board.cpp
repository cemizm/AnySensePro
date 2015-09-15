/*
 * Board.cpp
 *
 *  Created on: 11.09.2015
 *      Author: cem
 */

#include "Board.h"
#include <Interrupt.h>
#include <BLEDevice.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>

namespace
{

const clock_scale_t sysclock = {
RCC_CFGR_PLLMUL_PLL_IN_CLK_X16,
RCC_CFGR_PLLSRC_HSI_DIV2,
FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2WS,
RCC_CFGR_HPRE_DIV_NONE,
RCC_CFGR_PPRE1_DIV_2,
RCC_CFGR_PPRE2_DIV_NONE, 1, 32000000, 64000000, };

}

namespace Board
{

HAL::Pin Led(GPIOA, RCC_GPIOA, GPIO5);

void SystemInit()
{
	rcc_clock_setup_hsi(&sysclock);
}

namespace BLE
{

HAL::Pin MISO(GPIOA, RCC_GPIOA, GPIO6);
HAL::Pin MOSI(GPIOA, RCC_GPIOA, GPIO7);
HAL::Pin SCK(GPIOB, RCC_GPIOB, GPIO3);

HAL::Pin IRQ(GPIOA, RCC_GPIOA, GPIO0, EXTI0, NVIC_EXTI0_IRQ);
HAL::Pin CSN(GPIOA, RCC_GPIOA, GPIO1);
HAL::Pin RSTN(GPIOA, RCC_GPIOA, GPIO8);

HAL::DMA DMA_RX(DMA1, DMA_CHANNEL2, NVIC_DMA1_CHANNEL2_IRQ, RCC_DMA1);
HAL::DMA DMA_TX(DMA1, DMA_CHANNEL3, NVIC_DMA1_CHANNEL3_IRQ, RCC_DMA1);

HAL::SPI SPI(SPI1, RCC_SPI1, MOSI, MISO, SCK, GPIO_AF5, DMA_RX, DMA_TX);

BlueNRG::BLEConfig Config = { { 0x14, 0x24, 0x56, 0xF2, 0x65, 0x34 }, "AnySense Pro", "AnySense" };

}

}

extern "C" void exti0_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_EXTI0_IRQ);
}

extern "C" void dma1_channel2_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA1_CHANNEL2_IRQ);
}

extern "C" void dma1_channel3_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA1_CHANNEL3_IRQ);
}
