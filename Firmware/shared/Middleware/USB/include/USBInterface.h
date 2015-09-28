/*
 * USBInterface.h
 *
 *  Created on: 22.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_USB_INCLUDE_USBINTERFACE_H_
#define MIDDLEWARE_USB_INCLUDE_USBINTERFACE_H_

#include <stdint.h>
#include <libopencm3/usb/usbd.h>

#include "USBInterfaceStorage.h"

namespace USB
{
#define _ENDPOINT_DIRECT_IN				1
#define _ENDPOINT_DIRECT_OUT			0
#define _ENDPOINT(num, direction)		(uint8_t)(num | direction << 7)
#define ENDPOINT_IN(num)				_ENDPOINT(num, _ENDPOINT_DIRECT_IN)
#define ENDPOINT_OUT(num)				_ENDPOINT(num, _ENDPOINT_DIRECT_OUT)

class USBInterface
{
public:
	virtual void FillInterfaces(USBInterfaceStorage& interfaceStorage)
	{
		(void) interfaceStorage;
	}
	virtual void SetUSBDDevice(usbd_device *usbd_dev)
	{
		(void) usbd_dev;
	}

	virtual int ControlRequest(usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete)
	{
		(void) req;
		(void) buf;
		(void) len;
		(void) complete;

		return USBD_REQ_NOTSUPP;
	}

	virtual void EPSetup()
	{
	}

	virtual ~USBInterface()
	{
	}
};

} /* namespace LIBS */

#endif /* MIDDLEWARE_USB_INCLUDE_USBINTERFACE_H_ */
