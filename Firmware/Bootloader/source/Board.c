/*
 * Board.c
 *
 *  Created on: 27.02.2016
 *      Author: cem
 */
#include "Board.h"

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
	//rcc_osc_off(RCC_HSI);
}

void BootIndicate()
{
	//ORANGE
	rcc_periph_clock_enable(LED_ORANGE_RCC);
	gpio_mode_setup(LED_ORANGE_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_ORANGE_PIN);
	gpio_set_output_options(LED_ORANGE_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, LED_ORANGE_PIN);
	gpio_set(LED_ORANGE_PORT, LED_ORANGE_PIN);

	//RED
	rcc_periph_clock_enable(LED_RED_RCC);
	gpio_mode_setup(LED_RED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_RED_PIN);
	gpio_set_output_options(LED_RED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, LED_RED_PIN);
	gpio_clear(LED_RED_PORT, LED_RED_PIN);

}

void BootActiveIndicate()
{
	gpio_toggle(LED_RED_PORT, LED_ORANGE_PIN);
	gpio_toggle(LED_RED_PORT, LED_RED_PIN);
}
