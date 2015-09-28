/*
 * USBCDCInterface.cpp
 *
 *  Created on: 22.09.2015
 *      Author: cem
 */

#include <USBCDCInterface.h>

#include <stdlib.h>

#define USBCDCMAXINTERFACES		5

namespace
{

struct USBInterfaceInstanceMap
{
	usbd_device* usbd_dev;
	uint8_t ep;
	USB::USBCDCInterface* interface;
};

USBInterfaceInstanceMap interfaces[USBCDCMAXINTERFACES];

void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	for (uint8_t i = 0; i < USBCDCMAXINTERFACES; i++)
	{
		if (interfaces[i].usbd_dev == usbd_dev && interfaces[i].ep == ep)
			interfaces[i].interface->DataRX();
	}
}

}

namespace USB
{

USBCDCInterface::USBCDCInterface() :
		m_DataInterface(), m_CommInterface(), m_FunctionDescriptors(), m_DataEPs(), m_CommEP(), m_usbd_dev(
		NULL), m_connected(0), m_handler(NULL)
{
	m_CommEP.bLength = USB_DT_ENDPOINT_SIZE;
	m_CommEP.bDescriptorType = USB_DT_ENDPOINT;
	m_CommEP.bEndpointAddress = ENDPOINT_IN(3);
	m_CommEP.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT;
	m_CommEP.wMaxPacketSize = 8;
	m_CommEP.bInterval = 255;

	m_DataEPs.RX.bLength = USB_DT_ENDPOINT_SIZE;
	m_DataEPs.RX.bDescriptorType = USB_DT_ENDPOINT;
	m_DataEPs.RX.bEndpointAddress = ENDPOINT_OUT(1);
	m_DataEPs.RX.bmAttributes = USB_ENDPOINT_ATTR_BULK;
	m_DataEPs.RX.wMaxPacketSize = 64;
	m_DataEPs.RX.bInterval = 1;

	m_DataEPs.TX.bLength = USB_DT_ENDPOINT_SIZE;
	m_DataEPs.TX.bDescriptorType = USB_DT_ENDPOINT;
	m_DataEPs.TX.bEndpointAddress = ENDPOINT_IN(2);
	m_DataEPs.TX.bmAttributes = USB_ENDPOINT_ATTR_BULK;
	m_DataEPs.TX.wMaxPacketSize = 64;
	m_DataEPs.TX.bInterval = 1;

	m_FunctionDescriptors.header.bFunctionLength = sizeof(struct usb_cdc_header_descriptor);
	m_FunctionDescriptors.header.bDescriptorType = CS_INTERFACE;
	m_FunctionDescriptors.header.bDescriptorSubtype = USB_CDC_TYPE_HEADER;
	m_FunctionDescriptors.header.bcdCDC = 0x0110;

	m_FunctionDescriptors.call_mgmt.bFunctionLength = sizeof(struct usb_cdc_call_management_descriptor);
	m_FunctionDescriptors.call_mgmt.bDescriptorType = CS_INTERFACE;
	m_FunctionDescriptors.call_mgmt.bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT;
	m_FunctionDescriptors.call_mgmt.bmCapabilities = 0;
	m_FunctionDescriptors.call_mgmt.bDataInterface = 1;

	m_FunctionDescriptors.acm.bFunctionLength = sizeof(struct usb_cdc_acm_descriptor);
	m_FunctionDescriptors.acm.bDescriptorType = CS_INTERFACE;
	m_FunctionDescriptors.acm.bDescriptorSubtype = USB_CDC_TYPE_ACM;
	m_FunctionDescriptors.acm.bmCapabilities = 0;

	m_FunctionDescriptors.cdc_union.bFunctionLength = sizeof(struct usb_cdc_union_descriptor);
	m_FunctionDescriptors.cdc_union.bDescriptorType = CS_INTERFACE;
	m_FunctionDescriptors.cdc_union.bDescriptorSubtype = USB_CDC_TYPE_UNION;
	m_FunctionDescriptors.cdc_union.bControlInterface = 0;
	m_FunctionDescriptors.cdc_union.bSubordinateInterface0 = 1;

	m_CommInterface.bLength = USB_DT_INTERFACE_SIZE;
	m_CommInterface.bDescriptorType = USB_DT_INTERFACE;
	m_CommInterface.bInterfaceNumber = 0;
	m_CommInterface.bAlternateSetting = 0;
	m_CommInterface.bNumEndpoints = 1;
	m_CommInterface.bInterfaceClass = USB_CLASS_CDC;
	m_CommInterface.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM;
	m_CommInterface.bInterfaceProtocol = USB_CDC_PROTOCOL_AT;
	m_CommInterface.iInterface = 0;
	m_CommInterface.endpoint = &m_CommEP;
	m_CommInterface.extra = &m_FunctionDescriptors;
	m_CommInterface.extralen = sizeof(_cdcacm_functional_descriptors);

	m_DataInterface.bLength = USB_DT_INTERFACE_SIZE;
	m_DataInterface.bDescriptorType = USB_DT_INTERFACE;
	m_DataInterface.bInterfaceNumber = 1;
	m_DataInterface.bAlternateSetting = 0;
	m_DataInterface.bNumEndpoints = 2;
	m_DataInterface.bInterfaceClass = USB_CLASS_DATA;
	m_DataInterface.bInterfaceSubClass = 0;
	m_DataInterface.bInterfaceProtocol = 0;
	m_DataInterface.iInterface = 0;
	m_DataInterface.endpoint = (const usb_endpoint_descriptor*) &m_DataEPs.RX;

}

void USBCDCInterface::FillInterfaces(USBInterfaceStorage& interfaceStorage)
{
	m_DataEPs.RX.bEndpointAddress = ENDPOINT_OUT(interfaceStorage.GetNextEndpointId());
	m_DataEPs.TX.bEndpointAddress = ENDPOINT_IN(interfaceStorage.GetNextEndpointId());
	m_CommEP.bEndpointAddress = ENDPOINT_IN(interfaceStorage.GetNextEndpointId());

	InterfaceReg interfaces[2];
	interfaces[1].numAltSettings = 1;
	interfaces[1].altSetting = &m_DataInterface;
	interfaces[0].numAltSettings = 1;
	interfaces[0].altSetting = &m_CommInterface;

	interfaceStorage.RegisterInterfaces(interfaces, 2);

	m_FunctionDescriptors.cdc_union.bControlInterface = m_CommInterface.bInterfaceNumber;
	m_FunctionDescriptors.cdc_union.bSubordinateInterface0 = m_DataInterface.bInterfaceNumber;
	m_FunctionDescriptors.call_mgmt.bDataInterface = m_DataInterface.bInterfaceNumber;

}

void USBCDCInterface::SetUSBDDevice(usbd_device* device)
{
	m_usbd_dev = device;
	for (uint8_t i = 0; i < USBCDCMAXINTERFACES; i++)
	{
		if (interfaces[i].usbd_dev == NULL)
		{
			interfaces[i].usbd_dev = m_usbd_dev;
			interfaces[i].ep = m_DataEPs.RX.bEndpointAddress;
			interfaces[i].interface = this;
		}
	}
}

int USBCDCInterface::ControlRequest(usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete)
{
	(void) complete;
	(void) buf;

	switch (req->bRequest)
	{
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
	{
		setConnected(req->wValue == (1 << 0));

		/*
		 * This Linux cdc_acm driver requires this to be implemented
		 * even though it's optional in the CDC spec, and we don't
		 * advertise it in the ACM functional descriptor.
		 */
		char local_buf[10];
		struct usb_cdc_notification *notif = (usb_cdc_notification *) local_buf;

		/* We echo signals back to host as notification. */
		notif->bmRequestType = 0xA1;
		notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
		notif->wValue = 0;
		notif->wIndex = 0;
		notif->wLength = 2;
		local_buf[8] = req->wValue & 3;
		local_buf[9] = 0;
		// usbd_ep_write_packet(m_CommEndpoints.bEndpointAddress, buf, 10);
		return USBD_REQ_HANDLED;
	}
	case USB_CDC_REQ_SET_LINE_CODING:
		if (*len < sizeof(struct usb_cdc_line_coding))
			return USBD_REQ_NOTSUPP;
		return USBD_REQ_HANDLED;
	}

	return USBD_REQ_NOTSUPP;
}

void USBCDCInterface::EPSetup()
{
	usbd_ep_setup(m_usbd_dev, m_DataEPs.RX.bEndpointAddress, USB_ENDPOINT_ATTR_BULK, 64, cdcacm_data_rx_cb);
	usbd_ep_setup(m_usbd_dev, m_DataEPs.TX.bEndpointAddress, USB_ENDPOINT_ATTR_BULK, 64, NULL);
	usbd_ep_setup(m_usbd_dev, m_CommEP.bEndpointAddress, USB_ENDPOINT_ATTR_INTERRUPT, 8, NULL);
}

void USBCDCInterface::DataRX()
{
	uint8_t buf[64];
	int len = usbd_ep_read_packet(m_usbd_dev, m_DataEPs.RX.bEndpointAddress, buf, 64);

	if (len && m_handler != NULL)
	{
		m_handler->DataRX(buf, len);
	}
}

void USBCDCInterface::SendData(uint8_t* data, uint8_t len)
{

	usbd_ep_write_packet(m_usbd_dev, m_DataEPs.RX.bEndpointAddress, data, len);
}

void USBCDCInterface::setConnected(uint8_t connected)
{
	if (m_connected == connected)
		return;

	m_connected = connected;

	if (m_handler == NULL)
		return;

	if (m_connected)
		m_handler->Connected();
	else
		m_handler->Disconnected();
}

} /* namespace USB */
