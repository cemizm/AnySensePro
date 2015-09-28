/*
 * USBMSCInterface.cpp
 *
 *  Created on: 23.09.2015
 *      Author: cem
 */

#include <USBMSCInterface.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <libopencm3/cm3/common.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/msc.h>

namespace
{

int ramdisk_read(uint32_t lba, uint8_t *copy_to)
{
	(void) lba;
	(void) copy_to;
	// ignore writes
	return 0;
}

int ramdisk_write(uint32_t lba, const uint8_t *copy_from)
{
	(void) lba;
	(void) copy_from;
	// ignore writes
	return 0;
}

}

namespace USB
{

USBMSCInterface::USBMSCInterface() :
		m_Endpoints(), m_usbd_device(NULL), m_massStorage(NULL)
{
	m_Endpoints[0].bLength = USB_DT_ENDPOINT_SIZE;
	m_Endpoints[0].bDescriptorType = USB_DT_ENDPOINT;
	m_Endpoints[0].bEndpointAddress = ENDPOINT_OUT(1);
	m_Endpoints[0].bmAttributes = USB_ENDPOINT_ATTR_BULK;
	m_Endpoints[0].wMaxPacketSize = 64;
	m_Endpoints[0].bInterval = 0;

	m_Endpoints[1].bLength = USB_DT_ENDPOINT_SIZE;
	m_Endpoints[1].bDescriptorType = USB_DT_ENDPOINT;
	m_Endpoints[1].bEndpointAddress = ENDPOINT_IN(2);
	m_Endpoints[1].bmAttributes = USB_ENDPOINT_ATTR_BULK;
	m_Endpoints[1].wMaxPacketSize = 64;
	m_Endpoints[1].bInterval = 0;

	m_Interface.bLength = USB_DT_INTERFACE_SIZE;
	m_Interface.bDescriptorType = USB_DT_INTERFACE;
	m_Interface.bInterfaceNumber = 0;
	m_Interface.bAlternateSetting = 0;
	m_Interface.bNumEndpoints = 2;
	m_Interface.bInterfaceClass = USB_CLASS_MSC;
	m_Interface.bInterfaceSubClass = USB_MSC_SUBCLASS_SCSI;
	m_Interface.bInterfaceProtocol = USB_MSC_PROTOCOL_BBB;
	m_Interface.iInterface = 0;
	m_Interface.endpoint = m_Endpoints;
	m_Interface.extra = NULL;
	m_Interface.extralen = 0;
}

void USBMSCInterface::FillInterfaces(USBInterfaceStorage& interfaceStorage)
{
	m_Endpoints[0].bEndpointAddress = ENDPOINT_OUT(interfaceStorage.GetNextEndpointId());
	m_Endpoints[1].bEndpointAddress = ENDPOINT_IN(interfaceStorage.GetNextEndpointId());

	InterfaceReg interfaces[1];
	interfaces[0].numAltSettings = 1;
	interfaces[0].altSetting = &m_Interface;

	interfaceStorage.RegisterInterfaces(interfaces, 1);
}
void USBMSCInterface::SetUSBDDevice(usbd_device *usbd_dev)
{
	m_usbd_device = usbd_dev;
	/*
	 for (uint8_t i = 0; i < USBCDCMAXINTERFACES; i++)
	 {
	 if (interfaces[i].usbd_dev == NULL)
	 {
	 interfaces[i].usbd_dev = m_usbd_dev;
	 interfaces[i].interface = this;
	 }
	 }
	 */
	/*
	 m_massStorage = usb_msc_init(NULL, m_Endpoints[1].bEndpointAddress, 64, m_Endpoints[0].bEndpointAddress, 64, "", "",
	 "", 1024, NULL, NULL);
	 */
	ramdisk_write(0, NULL);

}
int USBMSCInterface::ControlRequest(usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete)
{
	(void) req;
	(void) buf;
	(void) len;
	(void) complete;
	return USBD_REQ_NOTSUPP;
}

void USBMSCInterface::EPSetup()
{

}

} /* namespace USB */
