/*
 * USBCDCDevice.cpp
 *
 *  Created on: 12.02.2016
 *      Author: cem
 */

#include <USBCDCDevice.h>


namespace USB
{

#define _ENDPOINT_DIRECT_IN				1
#define _ENDPOINT_DIRECT_OUT			0
#define _ENDPOINT(num, direction)		(uint8_t)(num | direction << 7)
#define ENDPOINT_IN(num)				_ENDPOINT(num, _ENDPOINT_DIRECT_IN)
#define ENDPOINT_OUT(num)				_ENDPOINT(num, _ENDPOINT_DIRECT_OUT)

USBCDCDevice::USBCDCDevice(HAL::USB& usb) :
		m_usb(usb), m_dev(), m_usbd_dev(NULL), m_config(), m_interfaces(), m_FunctionDescriptors(), m_DataEPs(), m_CommEP(), m_handler(
				NULL)
{

	m_dev.bLength = USB_DT_DEVICE_SIZE;
	m_dev.bDescriptorType = USB_DT_DEVICE;
	m_dev.bcdUSB = 0x0200;
	m_dev.bDeviceClass = USB_CLASS_CDC;
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

	m_interfaces[0].num_altsetting = 1;
	m_interfaces[0].altsetting = &m_CommInterface;

	m_interfaces[1].num_altsetting = 1;
	m_interfaces[1].altsetting = &m_DataInterface;

	m_config.bLength = USB_DT_CONFIGURATION_SIZE;
	m_config.bDescriptorType = USB_DT_CONFIGURATION;
	m_config.wTotalLength = 0;
	m_config.bNumInterfaces = 2;
	m_config.bConfigurationValue = 1;
	m_config.iConfiguration = 0;
	m_config.bmAttributes = 0x80;
	m_config.bMaxPower = 0x64;
	m_config.interface = m_interfaces;

}

void USBCDCDevice::Init()
{
	HAL::InterruptRegistry.Enable(m_usb.IRQN_USB_LP, 15, this);

	m_usb.Init();
	m_usbd_dev = usbd_init(m_usb.Driver, &m_dev, &m_config, m_usb_strings, 3, m_usbd_control_buffer,
			sizeof(m_usbd_control_buffer));

	usbd_register_set_config_callback(m_usbd_dev, usb_set_config);
}

void USBCDCDevice::SendData(uint8_t* data, uint8_t len)
{
	usbd_ep_write_packet(m_usbd_dev, m_DataEPs.TX.bEndpointAddress, data, len);
}

void USBCDCDevice::SetConfig(uint16_t wValue)
{
	(void) wValue;

	usbd_ep_setup(m_usbd_dev, m_DataEPs.RX.bEndpointAddress, USB_ENDPOINT_ATTR_BULK, 64, cdcacm_data_rx_cb);
	usbd_ep_setup(m_usbd_dev, m_DataEPs.TX.bEndpointAddress, USB_ENDPOINT_ATTR_BULK, 64, NULL);
	usbd_ep_setup(m_usbd_dev, m_CommEP.bEndpointAddress, USB_ENDPOINT_ATTR_INTERRUPT, 8, NULL);

	usbd_register_control_callback(m_usbd_dev, USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
	USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT, (usbd_control_callback) usb_control_request);
}

int USBCDCDevice::ControlRequest(usb_setup_data *req, uint8_t **buf, uint16_t *len, usbd_control_complete_callback *complete)
{
	(void) complete;
	(void) buf;

	switch (req->bRequest)
	{
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
	{
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

void USBCDCDevice::Poll()
{
	usbd_poll(m_usbd_dev);
}

void USBCDCDevice::ISR()
{
	Poll();
}

void USBCDCDevice::DataRX()
{
	uint8_t buf[64];
	int len = usbd_ep_read_packet(m_usbd_dev, m_DataEPs.RX.bEndpointAddress, buf, 64);

	if (len && m_handler != NULL)
		m_handler->DataRX(buf, len);
}

void USBCDCDevice::usb_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	(void)usbd_dev;
	CDCDevice.SetConfig(wValue);
}

int USBCDCDevice::usb_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
		usbd_control_complete_callback *complete)
{
	(void)usbd_dev;
	return CDCDevice.ControlRequest(req, buf, len, complete);
}

void USBCDCDevice::cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)usbd_dev;
	(void)ep;
	CDCDevice.DataRX();
}

} /* namespace tests */
