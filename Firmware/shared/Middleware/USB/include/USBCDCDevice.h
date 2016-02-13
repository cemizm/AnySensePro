/*
 * USBCDCDevice.h
 *
 *  Created on: 12.02.2016
 *      Author: cem
 */

#ifndef MIDDLEWARE_USB_INCLUDE_USBCDCDEVICE_H_
#define MIDDLEWARE_USB_INCLUDE_USBCDCDEVICE_H_

#include <USB.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <Interrupt.h>

struct __attribute__((packed)) _cdcacm_functional_descriptors
{
	struct usb_cdc_header_descriptor header;
	struct usb_cdc_call_management_descriptor call_mgmt;
	struct usb_cdc_acm_descriptor acm;
	struct usb_cdc_union_descriptor cdc_union;
};

struct __attribute__((packed)) _dataEPs
{
	usb_endpoint_descriptor RX;
	usb_endpoint_descriptor TX;
};

namespace USB
{
class USBSerialHandler: public HAL::USBConnectedHandler
{
public:
	virtual void DataRX(uint8_t* data, uint8_t len)
	{
		(void) data;
		(void) len;
	}
	virtual ~USBSerialHandler()
	{
	}
};

class USBCDCDevice: HAL::InterruptHandler
{
private:
	HAL::USB& m_usb;

	usb_device_descriptor m_dev;
	usbd_device *m_usbd_dev;
	uint8_t m_usbd_control_buffer[255];
	usb_config_descriptor m_config;
	const char *m_usb_strings[3] = { "xeniC UG", "AnySense Pro", "1.0" };

	usb_interface_descriptor m_DataInterface;
	usb_interface_descriptor m_CommInterface;
	usb_config_descriptor::usb_interface m_interfaces[2];

	_cdcacm_functional_descriptors m_FunctionDescriptors;
	_dataEPs m_DataEPs;
	usb_endpoint_descriptor m_CommEP;

	USBSerialHandler* m_handler;

	void SetConfig(uint16_t wValue);
	int ControlRequest(usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete);
	void Poll();
	void DataRX();
public:
	USBCDCDevice(HAL::USB& usb);

	void Init();

	void SendData(uint8_t* data, uint8_t len);

	void RegisterHandler(USBSerialHandler* handler)
	{
		m_usb.SetConnectedHandler(handler);
		m_handler = handler;
	}

	uint8_t IsConnected()
	{
		return m_usb.IsConnected();
	}

	void ISR() override;

	static int usb_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
			usbd_control_complete_callback *complete);

	static void usb_set_config(usbd_device *usbd_dev, uint16_t wValue);

	static void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep);
};


}

extern USB::USBCDCDevice CDCDevice;

#endif /* MIDDLEWARE_USB_INCLUDE_USBCDCDEVICE_H_ */
