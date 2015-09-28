/*
 * USBInterfaceStorage.h
 *
 *  Created on: 23.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_USB_INCLUDE_USBINTERFACESTORAGE_H_
#define MIDDLEWARE_USB_INCLUDE_USBINTERFACESTORAGE_H_

#include <libopencm3/usb/usbd.h>

#define MAX_ENDPOINTS 		8
#define MAX_INTERFACES		8
#define MAX_ASSOCIATION		10

namespace USB
{

struct InterfaceReg
{
	uint8_t numAltSettings;
	usb_interface_descriptor* altSetting;
};

class USBInterfaceStorage
{
private:
	usb_config_descriptor::usb_interface m_interfaces[MAX_INTERFACES];

	usb_iface_assoc_descriptor m_AssociationDescriptors[MAX_ASSOCIATION];

	uint8_t m_IsComposite;

	uint8_t m_InterfaceCount;
	uint8_t m_EndpointCount;
	uint8_t m_associationCount;
public:
	USBInterfaceStorage(uint8_t isComposite) :
			m_interfaces(), m_AssociationDescriptors(), m_IsComposite(isComposite), m_InterfaceCount(0), m_EndpointCount(0), m_associationCount(
					0)
	{
	}

	uint8_t GetNextEndpointId()
	{
		if (m_EndpointCount == MAX_ENDPOINTS)
			return MAX_ASSOCIATION - 1;

		return ++m_EndpointCount;
	}

	void RegisterInterfaces(InterfaceReg* interfaces, uint8_t count)
	{
		for (uint8_t i = 0; i < count; i++)
		{
			usb_interface_descriptor* interface = (usb_interface_descriptor*) (uint8_t*) interfaces[i].altSetting;

			m_interfaces[m_InterfaceCount].num_altsetting = interfaces[i].numAltSettings;
			m_interfaces[m_InterfaceCount].altsetting = interface;

			interface->bInterfaceNumber = m_InterfaceCount;

			if (m_IsComposite && i == 0)
			{
				m_AssociationDescriptors[m_associationCount].bLength = USB_DT_INTERFACE_ASSOCIATION_SIZE;
				m_AssociationDescriptors[m_associationCount].bDescriptorType = USB_DT_INTERFACE_ASSOCIATION;
				m_AssociationDescriptors[m_associationCount].bFirstInterface = interface->bInterfaceNumber;
				m_AssociationDescriptors[m_associationCount].bInterfaceCount = count;
				m_AssociationDescriptors[m_associationCount].bFunctionClass = interface->bInterfaceClass;
				m_AssociationDescriptors[m_associationCount].bFunctionSubClass = interface->bInterfaceSubClass;
				m_AssociationDescriptors[m_associationCount].bFunctionProtocol = interface->bInterfaceProtocol;
				m_AssociationDescriptors[m_associationCount].iFunction = 0x02;

				m_interfaces[m_InterfaceCount].iface_assoc = &m_AssociationDescriptors[m_associationCount];

				if (++m_associationCount == MAX_ASSOCIATION)
					m_associationCount = MAX_ASSOCIATION - 1;

			}

			if (++m_InterfaceCount == MAX_INTERFACES)
				m_InterfaceCount = MAX_INTERFACES - 1;
		}
	}

	usb_config_descriptor::usb_interface* GetInterfaces()
	{
		return m_interfaces;
	}

	uint8_t GetInterfaceCount()
	{
		return m_InterfaceCount;
	}

	uint8_t GetDeviceClass()
	{
		if (m_IsComposite)
			return 0;

		return m_interfaces[0].altsetting->bInterfaceClass;
	}

};

} /* namespace USB */

#endif /* MIDDLEWARE_USB_INCLUDE_USBINTERFACESTORAGE_H_ */
