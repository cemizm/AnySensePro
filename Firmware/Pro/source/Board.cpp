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
#include <libopencm3/stm32/st_usbfs.h>
#include <libopencm3/stm32/syscfg.h>


namespace Board
{

HAL::Pin LedActivity(GPIOC, RCC_GPIOC, GPIO3);
HAL::Pin LedError(GPIOC, RCC_GPIOC, GPIO2);

HAL::Pin USB_DM(GPIOA, RCC_GPIOA, GPIO11);
HAL::Pin USB_DP(GPIOA, RCC_GPIOA, GPIO12);
HAL::Pin USB_Disconnect(GPIOA, RCC_GPIOA, GPIO8);
HAL::Pin USB_Sense(GPIOC, RCC_GPIOC, GPIO0, EXTI0, NVIC_EXTI0_IRQ);

HAL::USB USB(RCC_USB, rcc_usb_prescale_1_5, USB_DP, USB_DM, GPIO_AF14, USB_Sense, USB_Disconnect, &st_usbfs_v2_usb_driver,
NVIC_USB_LP_IRQ, NVIC_USB_HP_IRQ, NVIC_USB_WKUP_IRQ);

namespace MicroSD
{

HAL::Pin CD(GPIOC, RCC_GPIOC, GPIO4, EXTI4, NVIC_EXTI4_IRQ);

HAL::Pin MOSI(GPIOA, RCC_GPIOA, GPIO7);
HAL::Pin MISO(GPIOA, RCC_GPIOA, GPIO6);
HAL::Pin SCK(GPIOA, RCC_GPIOA, GPIO5);
HAL::Pin CSN(GPIOA, RCC_GPIOA, GPIO4);

HAL::DMA RX(DMA1, DMA_CHANNEL2, NVIC_DMA1_CHANNEL2_IRQ, rcc_periph_clken::RCC_DMA1);
HAL::DMA TX(DMA1, DMA_CHANNEL3, NVIC_DMA1_CHANNEL3_IRQ, rcc_periph_clken::RCC_DMA1);

HAL::SPI SPI(SPI1, rcc_periph_clken::RCC_SPI1, MOSI, MISO, SCK, GPIO_AF5, RX, TX);

}

namespace Flash
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

HAL::Pin USART_RX(GPIOA, RCC_GPIOA, GPIO3);
HAL::Pin USART_TX(GPIOA, RCC_GPIOA, GPIO2);

HAL::DMA RX_DMA(DMA1, DMA_CHANNEL6, NVIC_DMA1_CHANNEL6_IRQ, rcc_periph_clken::RCC_DMA1);
HAL::DMA TX_DMA(DMA1, DMA_CHANNEL7, NVIC_DMA1_CHANNEL7_IRQ, rcc_periph_clken::RCC_DMA1);

HAL::USART USART(USART2, RCC_USART2, RX, TX, GPIO_AF7, NVIC_USART2_EXTI26_IRQ, RX_DMA, TX_DMA);
}

namespace Telemetry
{

HAL::Pin RX(GPIOA, RCC_GPIOA, GPIO10);
HAL::Pin TX(GPIOA, RCC_GPIOA, GPIO9);

HAL::DMA TX_DMA(DMA1, DMA_CHANNEL4, NVIC_DMA1_CHANNEL4_IRQ, rcc_periph_clken::RCC_DMA1);
HAL::DMA RX_DMA(DMA1, DMA_CHANNEL5, NVIC_DMA1_CHANNEL5_IRQ, rcc_periph_clken::RCC_DMA1);

HAL::USART USART(USART1, RCC_USART1, RX, TX, GPIO_AF7, NVIC_USART1_EXTI25_IRQ, RX_DMA, TX_DMA);

}

namespace Sensor
{
HAL::Pin RX(GPIOB, RCC_GPIOB, GPIO11);
HAL::Pin TX(GPIOB, RCC_GPIOB, GPIO10);

HAL::DMA RX_DMA(DMA1, DMA_CHANNEL3, NVIC_DMA1_CHANNEL3_IRQ, rcc_periph_clken::RCC_DMA1);
HAL::DMA TX_DMA(DMA1, DMA_CHANNEL2, NVIC_DMA1_CHANNEL2_IRQ, rcc_periph_clken::RCC_DMA1);

HAL::USART USART(USART3, RCC_USART3, RX, TX, GPIO_AF7, NVIC_USART3_EXTI28_IRQ, RX_DMA, TX_DMA);
}

namespace OSD
{

HAL::Pin RX(GPIOC, RCC_GPIOC, GPIO11);
HAL::Pin TX(GPIOC, RCC_GPIOC, GPIO10);

HAL::DMA RX_DMA(DMA2, DMA_CHANNEL3, NVIC_DMA2_CHANNEL3_IRQ, rcc_periph_clken::RCC_DMA2);
HAL::DMA TX_DMA(DMA2, DMA_CHANNEL5, NVIC_DMA2_CHANNEL5_IRQ, rcc_periph_clken::RCC_DMA2);

HAL::USART USART(UART4, RCC_UART4, RX, TX, GPIO_AF5, NVIC_UART4_EXTI34_IRQ, RX_DMA, TX_DMA);
}

void InitClock()
{

	/* Enable internal high-speed oscillator. */
	rcc_osc_on(RCC_HSI);
	rcc_wait_for_osc_ready(RCC_HSI);

	/* Select HSI as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_HSI);

	/* Enable external high-speed oscillator 8MHz. */
	rcc_osc_on(RCC_HSE);
	rcc_wait_for_osc_ready(RCC_HSE);

	rcc_set_hpre(RCC_CFGR_HPRE_DIV_NONE);
	rcc_set_ppre1(RCC_CFGR_PPRE1_DIV_2);
	rcc_set_ppre2(RCC_CFGR_PPRE2_DIV_NONE);

	rcc_set_pll_source(RCC_CFGR_PLLSRC_HSE_PREDIV);
	rcc_set_pll_multiplier(RCC_CFGR_PLLMUL_PLL_IN_CLK_X9);

	/* Enable PLL oscillator and wait for it to stabilize. */
	rcc_osc_on(RCC_PLL);
	rcc_wait_for_osc_ready(RCC_PLL);

	/* Configure flash settings. */
	flash_set_ws(FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2WS);

	/* Select PLL as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_PLL);

	/* Wait for PLL clock to be selected. */
	rcc_wait_for_sysclk_status(RCC_PLL);

	/* Set the peripheral clock frequencies used. */
	rcc_apb1_frequency = 36000000;
	rcc_apb2_frequency = 72000000;

	/* Disable internal high-speed oscillator. */
	rcc_osc_off(RCC_HSI);
}

void SystemInit()
{

	InitClock();

	rcc_periph_clock_enable(rcc_periph_clken::RCC_SYSCFG);

	SYSCFG_MEMRM |= 1 << 5; //USB Remap
}

}

#ifndef INTERRUPT_FORWARDING

extern "C" void usart1_exti25_isr(void)
{
	HAL::InterruptRegistry.HandleISR(NVIC_USART1_EXTI25_IRQ);
}

extern "C" void usart2_exti26_isr(void)
{
	HAL::InterruptRegistry.HandleISR(NVIC_USART2_EXTI26_IRQ);
}

extern "C" void usart3_exti28_isr(void)
{
	HAL::InterruptRegistry.HandleISR(NVIC_USART3_EXTI28_IRQ);
}

extern "C" void uart4_exti34_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_UART4_EXTI34_IRQ);
}

extern "C" void spi2_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_SPI2_IRQ);
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

extern "C" void exti0_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_EXTI0_IRQ);
}

extern "C" void exti4_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_EXTI4_IRQ);
}

extern "C" void exti9_5_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_EXTI9_5_IRQ);
}

extern "C" void dma1_channel1_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA1_CHANNEL1_IRQ);
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

extern "C" void dma1_channel6_isr(void)
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA1_CHANNEL6_IRQ);
}

extern "C" void dma1_channel7_isr(void)
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA1_CHANNEL7_IRQ);
}

extern "C" void dma2_channel1_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA2_CHANNEL1_IRQ);
}

extern "C" void dma2_channel2_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA2_CHANNEL2_IRQ);
}

extern "C" void dma2_channel3_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA2_CHANNEL3_IRQ);
}

extern "C" void dma2_channel5_isr()
{
	HAL::InterruptRegistry.HandleISR(NVIC_DMA2_CHANNEL5_IRQ);
}

#endif
