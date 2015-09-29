/*
 * Board.cpp
 *
 *  Created on: 11.09.2015
 *      Author: cem
 */

#include "Board.h"
#include <Interrupt.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/can.h>
#include <libopencm3/stm32/usb.h>
#include <libopencm3/stm32/syscfg.h>

namespace Board
{

HAL::Pin LedActivity(GPIOC, RCC_GPIOC, GPIO7);
HAL::Pin LedError(GPIOC, RCC_GPIOC, GPIO8);

HAL::Pin USB_DP(GPIOA, RCC_GPIOA, GPIO11);
HAL::Pin USB_DM(GPIOA, RCC_GPIOA, GPIO12);
HAL::Pin USB_Disconnect(GPIOA, RCC_GPIOA, GPIO10);
HAL::Pin USB_Sense(GPIOA, RCC_GPIOA, GPIO9, EXTI19, NVIC_EXTI9_5_IRQ);

HAL::USB USB(RCC_USB, rcc_usb_prescale_1_5, USB_DP, USB_DM, GPIO_AF14, USB_Sense, USB_Disconnect, &stm32f103_usb_driver,
NVIC_USB_LP_IRQ, NVIC_USB_HP_IRQ, NVIC_USB_WKUP_IRQ);

namespace MicroSD
{

HAL::Pin CD(GPIOC, RCC_GPIOC, GPIO6);

HAL::Pin MOSI(GPIOB, RCC_GPIOB, GPIO15);
HAL::Pin MISO(GPIOB, RCC_GPIOB, GPIO14);
HAL::Pin SCK(GPIOB, RCC_GPIOB, GPIO13);
HAL::Pin CSN(GPIOB, RCC_GPIOB, GPIO12);

HAL::DMA RX(DMA1, DMA_CHANNEL4, NVIC_DMA1_CHANNEL4_IRQ, rcc_periph_clken::RCC_DMA1);
HAL::DMA TX(DMA1, DMA_CHANNEL5, NVIC_DMA1_CHANNEL5_IRQ, rcc_periph_clken::RCC_DMA1);

HAL::SPI SPI(SPI2, rcc_periph_clken::RCC_SPI2, MOSI, MISO, SCK, GPIO_AF5, RX, TX);

}

namespace Ram
{

HAL::Pin MOSI(GPIOB, RCC_GPIOB, GPIO5);
HAL::Pin MISO(GPIOB, RCC_GPIOB, GPIO4);
HAL::Pin SCK(GPIOB, RCC_GPIOB, GPIO3);
HAL::Pin CSN(GPIOA, RCC_GPIOA, GPIO15);

HAL::DMA RX(DMA2, DMA_CHANNEL1, NVIC_DMA2_CHANNEL1_IRQ, rcc_periph_clken::RCC_DMA2);
HAL::DMA TX(DMA2, DMA_CHANNEL2, NVIC_DMA2_CHANNEL2_IRQ, rcc_periph_clken::RCC_DMA2);

HAL::SPI SPI(SPI3, rcc_periph_clken::RCC_SPI3, MOSI, MISO, SCK, GPIO_AF6, RX, TX);

}

namespace FC
{

HAL::Pin RX(GPIOB, RCC_GPIOB, GPIO8);
HAL::Pin TX(GPIOB, RCC_GPIOB, GPIO9);

HAL::CAN CAN(CAN1, RCC_CAN, TX, RX, GPIO_AF9, NVIC_USB_HP_CAN1_TX_IRQ, NVIC_USB_LP_CAN1_RX0_IRQ, NVIC_CAN1_RX1_IRQ);
}

void InitClock()
{
	/* Enable internal high-speed oscillator. */
	rcc_osc_on(HSI);
	rcc_wait_for_osc_ready(HSI);

	/* Select HSI as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_HSI);

	/* Enable external high-speed oscillator 8MHz. */
	rcc_osc_on(HSE);
	rcc_wait_for_osc_ready(HSE);

	rcc_set_hpre(RCC_CFGR_HPRE_DIV_NONE);
	rcc_set_ppre1(RCC_CFGR_PPRE1_DIV_2);
	rcc_set_ppre2(RCC_CFGR_PPRE2_DIV_NONE);

	rcc_set_main_pll_hsi(RCC_CFGR_PLLMUL_PLL_IN_CLK_X9);
	rcc_set_pll_source(RCC_CFGR_PLLSRC_HSE_PREDIV);

	/* Enable PLL oscillator and wait for it to stabilize. */
	rcc_osc_on(PLL);
	rcc_wait_for_osc_ready(PLL);

	/* Configure flash settings. */
	flash_set_ws(FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2WS);

	/* Select PLL as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_PLL);

	/* Wait for PLL clock to be selected. */
	rcc_wait_for_sysclk_status(PLL);

	/* Set the peripheral clock frequencies used. */
	rcc_apb1_frequency = 36000000;
	rcc_apb2_frequency = 72000000;

	/* Disable internal high-speed oscillator. */
	rcc_osc_off(HSI);
}

void SystemInit()
{

	InitClock();

	rcc_periph_clock_enable(rcc_periph_clken::RCC_SYSCFG);

	SYSCFG_MEMRM |= 1 << 5; //USB Remap
}

}

extern "C" void can1_rx1_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_CAN1_RX1_IRQ);
}

extern "C" void usb_hp_can1_tx_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_USB_HP_CAN1_TX_IRQ);
}

extern "C" void usb_lp_can1_rx0_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_USB_LP_CAN1_RX0_IRQ);
}

extern "C" void usb_hp_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_USB_HP_IRQ);
}

extern "C" void usb_lp_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_USB_LP_IRQ);
}

extern "C" void usb_wkup_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_USB_WKUP_IRQ);
}

extern "C" void exti4_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_EXTI4_IRQ);
}

extern "C" void dma1_channel2_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA1_CHANNEL2_IRQ);
}

extern "C" void dma1_channel3_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA1_CHANNEL3_IRQ);
}

extern "C" void dma1_channel4_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA1_CHANNEL4_IRQ);
}

extern "C" void dma1_channel5_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA1_CHANNEL5_IRQ);
}
