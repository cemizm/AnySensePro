/*
 * CAN.h
 *
 *  Created on: 17.09.2015
 *      Author: cem
 */

#ifndef HAL_INCLUDE_CAN_H_
#define HAL_INCLUDE_CAN_H_

#include <stdint.h>
#include <libopencm3/stm32/can.h>
#include <libopencm3/stm32/rcc.h>

#include "Pin.h"

namespace HAL
{

struct CANTxMessage
{
	uint32_t Id;
	uint8_t IsExtendedId;
	uint8_t RequestTransmit;
	uint8_t DataLength;
	uint8_t data[8];
};

struct CANRxMessage
{
	uint32_t Id;bool IsExtendedId;bool RequestTransmit;
	uint32_t MatchedFilterId;
	uint8_t DataLength;
	uint8_t data[8];
};

class CAN
{
private:
	const uint32_t m_canport;
	const rcc_periph_clken m_can_clock;
	const Pin m_tx_pin;
	const Pin m_rx_pin;
	const uint8_t m_pin_alt_func;

public:
	const uint8_t NVIC_TX_IRQn;
	const uint8_t NVIC_RX0_IRQn;
	const uint8_t NVIC_RX1_IRQn;

	CAN(uint32_t canport, rcc_periph_clken can_clock, Pin tx_pin, Pin rx_pin, uint8_t pin_alt_func, uint8_t tx_irqn,
			uint8_t rx0_irqn, uint8_t rx1_irqn) :
			m_canport(canport), m_can_clock(can_clock), m_tx_pin(tx_pin), m_rx_pin(rx_pin), m_pin_alt_func(pin_alt_func), NVIC_TX_IRQn(
					tx_irqn), NVIC_RX0_IRQn(rx0_irqn), NVIC_RX1_IRQn(rx1_irqn)
	{

	}

	inline int8_t Init(uint8_t ttcm, uint8_t abom, uint8_t awum, uint8_t nart, uint8_t rflm, uint8_t txfp, uint32_t sjw,
			uint32_t ts1, uint32_t ts2, uint32_t brp, uint8_t loopback, uint8_t silent) const
	{
		rcc_periph_clock_enable(m_can_clock);

		m_tx_pin.PowerUp();
		m_tx_pin.ModeSetup(GPIO_MODE_AF, GPIO_PUPD_NONE);
		m_tx_pin.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ);
		m_tx_pin.Alternate(m_pin_alt_func);

		m_rx_pin.PowerUp();
		m_rx_pin.ModeSetup(GPIO_MODE_AF, GPIO_PUPD_PULLDOWN);
		m_rx_pin.SetOutputOptions(GPIO_OTYPE_OD, GPIO_OSPEED_100MHZ);
		m_rx_pin.Alternate(m_pin_alt_func);

		return can_init(m_canport, ttcm, abom, awum, nart, rflm, txfp, sjw, ts1, ts2, brp, loopback, silent);
	}

	inline void Reset() const
	{

		can_reset(m_canport);
	}

	inline void FilterInit(uint32_t nr, uint8_t scale_32bit, uint8_t id_list_mode, uint32_t fr1, uint32_t fr2, uint32_t fifo,
			uint8_t enable) const
	{
		can_filter_init(m_canport, nr, scale_32bit, id_list_mode, fr1, fr2, fifo, enable);
	}

	inline void FilterIdMask16BitInit(uint32_t nr, uint16_t id1, uint16_t mask1, uint16_t id2, uint16_t mask2, uint32_t fifo,
			uint8_t enable) const
	{
		can_filter_id_mask_16bit_init(m_canport, nr, id1, mask1, id2, mask2, fifo, enable);
	}

	inline void FilterIdMask32BitInit(uint32_t nr, uint32_t id, uint32_t mask, uint32_t fifo, uint8_t enable) const
	{
		can_filter_id_mask_32bit_init(m_canport, nr, id, mask, fifo, enable);
	}

	inline void FilterIdList16BitInit(uint32_t nr, uint16_t id1, uint16_t id2, uint16_t id3, uint16_t id4, uint32_t fifo,
			uint8_t enable) const
	{
		can_filter_id_list_16bit_init(m_canport, nr, id1, id2, id3, id4, fifo, enable);
	}

	inline void FilterIdList32BitInit(uint32_t nr, uint32_t id1, uint32_t id2, uint32_t fifo, uint8_t enable) const
	{
		can_filter_id_list_32bit_init(m_canport, nr, id1, id2, fifo, enable);
	}

	inline void EnableIRQ(uint32_t irq) const
	{
		can_enable_irq(m_canport, irq);
	}

	inline void DisableIRQ(uint32_t irq) const
	{
		can_disable_irq(m_canport, irq);
	}

	inline int8_t Transmit(CANTxMessage* msg) const
	{
		return can_transmit(m_canport, msg->Id, msg->IsExtendedId, msg->RequestTransmit, msg->DataLength, msg->data);
	}

	inline void Receive(uint8_t fifo, uint8_t release, CANRxMessage* msg) const
	{
		can_receive(m_canport, fifo, release, &msg->Id, &msg->IsExtendedId, &msg->RequestTransmit, &msg->MatchedFilterId,
				&msg->DataLength, msg->data);
	}

	inline void FifoRelease(uint8_t fifo) const
	{
		can_fifo_release(m_canport, fifo);
	}

	inline uint8_t IsMailboxAvailable() const
	{
		return can_available_mailbox(m_canport);
	}
};

}

#endif /* HAL_INCLUDE_CAN_H_ */
