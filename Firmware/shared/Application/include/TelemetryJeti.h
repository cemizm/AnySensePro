/*
 * TelemetryJeti.h
 *
 *  Created on: 17.03.2016
 *      Author: cem
 */

#ifndef APPLICATION_INCLUDE_TELEMETRYJETI_H_
#define APPLICATION_INCLUDE_TELEMETRYJETI_H_

#include "TelemetryAdapter.h"

#include "Timer.h"
#include "USART.h"
#include "Pin.h"
#include "Interrupt.h"

namespace App
{

/*
 *
 */
class TelemetryJeti: public HAL::InterruptHandler, public TelemetryAdapter<TelemetryProtocol::Jeti>
{
private:
	static const uint16_t prescaler = 8;
	static const uint16_t baudrate = 9800;

	struct ProtocolConfig
	{
		uint16_t IsValid;
	};

	HAL::USART& m_usart;
	const HAL::Pin& m_pin;
	HAL::Timer& m_timer;
	ProtocolConfig* m_config;
	uint8_t m_run;
	uint8_t m_counter;
	void Reset();
protected:
	void Init() override;
	void Run(void) override;
	void ISR() override;
	void DeInit() override;
public:
	TelemetryJeti(HAL::USART& usart, HAL::Timer& timer) :
			m_usart(usart), m_pin(usart.GetTXPin()), m_timer(timer), m_config(nullptr), m_run(1), m_counter(0)
	{
	}
};

} /* namespace App */

#endif /* APPLICATION_INCLUDE_TELEMETRYJETI_H_ */
