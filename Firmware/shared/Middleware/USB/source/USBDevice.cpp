/*
 * USBDevice.cpp
 *
 *  Created on: 22.09.2015
 *      Author: cem
 */

#include "USBDevice.h"
#include <stdlib.h>

namespace
{

struct USBInstanceMap
{
	usbd_device* usbd_dev;
	USB::USBDevice* device;
};

USBInstanceMap devices[3];

void usb_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	for (uint8_t i = 0; i < 3; i++)
	{
		if (devices[i].usbd_dev == usbd_dev)
			devices[i].device->SetConfig(wValue);
	}
}

int usb_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
		usbd_control_complete_callback *complete)
{
	for (uint8_t i = 0; i < 3; i++)
	{
		if (devices[i].usbd_dev == usbd_dev)
			return devices[i].device->ControlRequest(req, buf, len, complete);
	}

	return USBD_REQ_NOTSUPP;
}

}

namespace USB
{

USBDevice::USBDevice(HAL::USB& usb, USBInterface** interfaces, uint8_t num_interfaces) :
		m_usb(usb), m_interfaces(interfaces), m_num_interfaces(num_interfaces), m_dev(), m_usbd_dev(NULL), m_config(), m_interfaceStorage(
				num_interfaces > 1)
{

	for (uint8_t num = 0; num < num_interfaces; num++)
	{
		m_interfaces[num]->FillInterfaces(m_interfaceStorage);
	}

	m_dev.bLength = USB_DT_DEVICE_SIZE;
	m_dev.bDescriptorType = USB_DT_DEVICE;
	m_dev.bcdUSB = 0x0200;
	m_dev.bDeviceClass = m_interfaceStorage.GetDeviceClass();
	m_dev.bDeviceSubClass = 0;
	m_dev.bDeviceProtocol = 0;
	m_dev.bMaxPacketSize0 = 64;
	m_dev.idVendor = 0x01CB;
	m_dev.idProduct = 0xAE10;
	m_dev.bcdDevice = 0x0200;
	m_dev.iManufacturer = 1;
	m_dev.iProduct = 2;
	m_dev.iSerialNumber = 3;
	m_dev.bNumConfigurations = 1;

	m_config.bLength = USB_DT_CONFIGURATION_SIZE;
	m_config.bDescriptorType = USB_DT_CONFIGURATION;
	m_config.wTotalLength = 0;
	m_config.bNumInterfaces = m_interfaceStorage.GetInterfaceCount();
	m_config.bConfigurationValue = 1;
	m_config.iConfiguration = 0;
	m_config.bmAttributes = 0x80;
	m_config.bMaxPower = 0x64;
	m_config.interface = m_interfaceStorage.GetInterfaces();

}

void USBDevice::Init()
{
	HAL::InterruptRegistry.Enable(m_usb.IRQN_USB_LP, 15, this);

	m_usb.Init();
	m_usbd_dev = usbd_init(m_usb.Driver, &m_dev, &m_config, m_usb_strings, 3, m_usbd_control_buffer,
			sizeof(m_usbd_control_buffer));

	for (uint8_t i = 0; i < 3; i++)
	{
		if (devices[i].usbd_dev == NULL)
		{
			devices[i].device = this;
			devices[i].usbd_dev = m_usbd_dev;
			break;
		}
	}

	usbd_register_set_config_callback(m_usbd_dev, usb_set_config);

	for (uint8_t i = 0; i < m_num_interfaces; i++)
		m_interfaces[i]->SetUSBDDevice(m_usbd_dev);

}

void USBDevice::SetConfig(uint16_t wValue)
{
	(void) wValue;

	for (uint8_t i = 0; i < m_num_interfaces; i++)
	{
		m_interfaces[i]->EPSetup();
	}

	usbd_register_control_callback(m_usbd_dev, USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
	USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT, (usbd_control_callback) usb_control_request);
}

int USBDevice::ControlRequest(usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete)
{
	int ret = USBD_REQ_NOTSUPP;

	for (uint8_t i = 0; i < m_num_interfaces; i++)
	{
		ret = m_interfaces[i]->ControlRequest(req, buf, len, complete);
		if (ret != USBD_REQ_NOTSUPP)
			return ret;
	}

	return ret;
}

void USBDevice::Poll()
{
	usbd_poll(m_usbd_dev);
}

void USBDevice::ISR()
{
	Poll();
}

} /* namespace LIBS */
