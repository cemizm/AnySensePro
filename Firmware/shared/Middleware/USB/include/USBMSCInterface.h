/*
 * USBMSCInterface.h
 *
 *  Created on: 23.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_USB_INCLUDE_USBMSCINTERFACE_H_
#define MIDDLEWARE_USB_INCLUDE_USBMSCINTERFACE_H_

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/msc.h>

#include "USBInterface.h"

namespace USB
{

class USBMSCInterface: public USBInterface
{
private:
	usb_endpoint_descriptor m_Endpoints[2];
	usb_interface_descriptor m_Interface;
	usbd_device* m_usbd_device;
	usbd_mass_storage* m_massStorage;
public:
	USBMSCInterface();
	void FillInterfaces(USBInterfaceStorage& interfaceStorage) override;
	void SetUSBDDevice(usbd_device *usbd_dev) override;
	int ControlRequest(usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete) override;
	void EPSetup() override;
};

} /* namespace USB */

#endif /* MIDDLEWARE_USB_INCLUDE_USBMSCINTERFACE_H_ */
