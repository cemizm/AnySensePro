/*
 * USB.h
 *
 *  Created on: 22.09.2015
 *      Author: cem
 */

#ifndef HAL_INCLUDE_USB_H_
#define HAL_INCLUDE_USB_H_

#include "Pin.h"
#include <libopencm3/usb/usbd.h>
#include <Interrupt.h>

namespace HAL
{

class USBConnectedHandler
{
public:
	virtual void DeviceConnected()
	{

	}
	virtual void DeviceDisconnected()
	{

	}

	virtual ~USBConnectedHandler()
	{
	}
};

class USB: InterruptHandler
{
private:
	rcc_periph_clken m_usb_clock;
	void (*m_setup_prescale)();
	Pin& m_DP;
	Pin& m_DM;
	uint8_t m_usb_alt_num;
	Pin& m_Sense;
	Pin& m_Disconnect;
	USBConnectedHandler* m_connectedHandler;
public:
	const usbd_driver* Driver;
	const uint32_t IRQN_USB_LP;
	const uint32_t IRQN_USB_HP;
	const uint32_t IRQN_USB_WKUP;

	USB(rcc_periph_clken usb_clock, void (*setup_prescale)(), Pin& DP, Pin& DM, uint8_t usb_alt_num, Pin& sense, Pin& disconnect,
			const usbd_driver* usb_driver, uint32_t irqn_usb_lp, uint32_t irqn_usb_hp, uint32_t irqn_usb_wkup) :
			m_usb_clock(usb_clock), m_setup_prescale(setup_prescale), m_DP(DP), m_DM(DM), m_usb_alt_num(usb_alt_num), m_Sense(
					sense), m_Disconnect(disconnect), m_connectedHandler(NULL), Driver(usb_driver), IRQN_USB_LP(irqn_usb_lp), IRQN_USB_HP(
					irqn_usb_hp), IRQN_USB_WKUP(irqn_usb_wkup)
	{

	}

	inline void Init()
	{
		if (m_setup_prescale != NULL)
			m_setup_prescale();

		rcc_periph_clock_enable(m_usb_clock);

		m_DP.PowerUp();
		m_DP.ModeSetup(GPIO_MODE_AF, GPIO_PUPD_NONE);
		m_DP.Alternate(m_usb_alt_num);

		m_DM.PowerUp();
		m_DM.ModeSetup(GPIO_MODE_AF, GPIO_PUPD_NONE);
		m_DM.Alternate(m_usb_alt_num);

		m_Sense.PowerUp();
		m_Sense.ModeSetup(GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN);
		m_Sense.EXTI_SetTrigger(exti_trigger_type::EXTI_TRIGGER_BOTH);
		m_Sense.EXTI_SelectSource();

		m_Disconnect.PowerUp();
		m_Disconnect.ModeSetup(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
		m_Disconnect.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ);

		InterruptRegistry.Enable(m_Sense.EXTI_NVIC_IRQ, 2, this);
	}

	inline void SetConnectedHandler(USBConnectedHandler* handler)
	{
		m_connectedHandler = handler;

		if (m_connectedHandler != NULL)
			m_Sense.EXTI_Enable();
		else
			m_Sense.EXTI_Disable();

	}

	inline uint8_t IsConnected()
	{
		return m_Sense.Get();
	}

	inline void Connect()
	{
		m_Disconnect.On();
	}

	inline void Disconnect()
	{
		m_Disconnect.Off();
	}

	virtual void ISR()
	{
		if (m_Sense.EXTI_IsPendingBit())
		{
			m_Sense.EXTI_ResetPendingBit();
			if (m_connectedHandler == NULL) //should not happen
				return;

			if (IsConnected())
				m_connectedHandler->DeviceConnected();
			else
				m_connectedHandler->DeviceDisconnected();
		}
	}

};

}

#endif /* HAL_INCLUDE_USB_H_ */
