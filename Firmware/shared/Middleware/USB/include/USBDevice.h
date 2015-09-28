/*
 * USBDevice.h
 *
 *  Created on: 22.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_USB_INCLUDE_USBDEVICE_H_
#define MIDDLEWARE_USB_INCLUDE_USBDEVICE_H_

#include <USB.h>
#include "USBInterfaceStorage.h"
#include "USBInterface.h"
#include <libopencm3/usb/usbd.h>
#include <Interrupt.h>

namespace USB
{

class USBDevice: HAL::InterruptHandler
{
private:
	HAL::USB& m_usb;
	USBInterface** m_interfaces;
	uint8_t m_num_interfaces;
	usb_device_descriptor m_dev;
	usbd_device *m_usbd_dev;
	uint8_t m_usbd_control_buffer[255];
	usb_config_descriptor m_config;
	const char *m_usb_strings[3] = { "xeniC UG", "AnySense Pro", "1.0" };
	USBInterfaceStorage m_interfaceStorage;
public:

	USBDevice(HAL::USB& usb, USBInterface** interfaces, uint8_t num_interfaces);

	void Init();
	void SetConfig(uint16_t wValue);
	int ControlRequest(usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete);
	void Poll();

	void ISR() override;
};

} /* namespace LIBS */

#endif /* MIDDLEWARE_USB_INCLUDE_USBDEVICE_H_ */
