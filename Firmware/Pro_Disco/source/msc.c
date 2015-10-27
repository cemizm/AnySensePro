/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2013 Weston Schmidt <weston_schmidt@alumni.purdue.edu>
 * Copyright (C) 2013 Pavol Rusnak <stick@gk2.sk>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/msc.h>

#include "ramdisk.h"

static const struct usb_device_descriptor dev_descr = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0110,
	.bDeviceClass = 0,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x0483,
	.idProduct = 0x5741,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};

static const struct usb_endpoint_descriptor msc_endp[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x01,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 0,
}, {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x82,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 0,
}};

static const struct usb_interface_descriptor msc_iface[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = USB_CLASS_MSC,
	.bInterfaceSubClass = USB_MSC_SUBCLASS_SCSI,
	.bInterfaceProtocol = USB_MSC_PROTOCOL_BBB,
	.iInterface = 0,
	.endpoint = msc_endp,
	.extra = NULL,
	.extralen = 0
}};

static const struct usb_interface ifaces[] = {{
	.num_altsetting = 1,
	.altsetting = msc_iface,
}};

static const struct usb_config_descriptor config_descr = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = 1,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0x80,
	.bMaxPower = 0x32,
	.interface = ifaces,
};

static const char *usb_strings[] = {
	"Black Sphere Technologies",
	"MSC Demo",
	"DEMO",
};

static usbd_device *msc_dev;
/* Buffer to be used for control requests. */
static uint8_t usbd_control_buffer[128];

void SystemInit()
{
	/* Enable internal high-speed oscillator. */
	rcc_osc_on(HSI);
	rcc_wait_for_osc_ready(HSI);

	/* Select HSI as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_HSI);

	rcc_osc_bypass_enable(HSE);

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


int main(void)
{
	SystemInit();

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USB);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
			GPIO9 | GPIO11 | GPIO12);
	gpio_set_af(GPIOA, GPIO_AF14, GPIO9 | GPIO11 | GPIO12);

	msc_dev = usbd_init(&stm32f103_usb_driver, &dev_descr, &config_descr,
			    usb_strings, 3,
			    usbd_control_buffer, sizeof(usbd_control_buffer));

	usb_msc_init(msc_dev, 0x82, 64, 0x01, 64, "VendorID", "ProductID",
		"0.00", ramdisk_blocks(), ramdisk_read, ramdisk_write);

	for (;;) {
		usbd_poll(msc_dev);
	}
}
