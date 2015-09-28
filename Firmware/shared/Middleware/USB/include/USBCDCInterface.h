/*
 * USBCDCInterface.h
 *
 *  Created on: 22.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_USB_INCLUDE_USBCDCINTERFACE_H_
#define MIDDLEWARE_USB_INCLUDE_USBCDCINTERFACE_H_

#include "USBInterface.h"

#include <stdint.h>

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

namespace USB

{

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

class USBSerialHandler
{
public:
	virtual void Connected()
	{

	}
	virtual void Disconnected()
	{

	}
	virtual void DataRX(uint8_t* data, uint8_t len)
	{
		(void) data;
		(void) len;
	}
	virtual ~USBSerialHandler()
	{
	}
};

class USBCDCInterface: public USBInterface
{
private:
	usb_interface_descriptor m_DataInterface;
	usb_interface_descriptor m_CommInterface;
	_cdcacm_functional_descriptors m_FunctionDescriptors;
	_dataEPs m_DataEPs;
	usb_endpoint_descriptor m_CommEP;
	usbd_device* m_usbd_dev;
	uint8_t m_connected;
	USBSerialHandler* m_handler;

	void setConnected(uint8_t connected);

public:
	USBCDCInterface();
	void FillInterfaces(USBInterfaceStorage& interfaceStorage) override;
	void SetUSBDDevice(usbd_device *usbd_dev) override;
	int ControlRequest(usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete) override;
	void EPSetup() override;
	void DataRX();
	void SendData(uint8_t* data, uint8_t len);
	uint8_t IsConnected()
	{
		return m_connected;
	}
	void RegisterHandler(USBSerialHandler* handler)
	{
		m_handler = handler;
	}
};

} /* namespace USB */

#endif /* MIDDLEWARE_USB_INCLUDE_USBCDCINTERFACE_H_ */
