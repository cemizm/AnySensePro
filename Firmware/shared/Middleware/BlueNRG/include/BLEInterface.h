/*
 * BLEInterface.h
 *
 *  Created on: 12.09.2015
 *      Author: cem
 */

#ifndef MIDDLEWARE_BLUENRG_INCLUDE_BLEINTERFACE_H_
#define MIDDLEWARE_BLUENRG_INCLUDE_BLEINTERFACE_H_

#include "Pin.h"
#include "SPI.h"
#include "OSAL.h"
#include "Interrupt.h"
#include <Queue.h>

namespace
{

enum BLETransactionStatus
{
	TransactionDone = 0, DeviceNotReady = 1, Timeout = 2,
};

}

namespace BlueNRG
{

enum SPIState
{
	Sleep = 0, Header_Transmit = 1, Header_Check = 2, Payload_Transmit = 3, Payload_Check = 4,
};

enum SPIDirection
{
	Transmit = 0, Receive = 1,
};

const uint8_t MAX_TRANSACTION_SIZE = UINT8_MAX;

enum HCISPIState
{
	HCI_OK = 0x02, Data_Receive = 0x0B, Data_Transmit = 0x0A
};

struct __attribute__((packed, aligned(1))) SPIHeader
{
	HCISPIState State;
	uint16_t TransmitBuffer;
	uint16_t ReceiveBuffer;
};

struct SPITransaction
{
	SPIHeader MasterHeader = { HCISPIState::Data_Receive, 0, 0 };
	SPIHeader SlaveHeader = { HCISPIState::HCI_OK, 0, 0 };

	uint8_t MasterPayload[MAX_TRANSACTION_SIZE] = { 0x00 };
	uint8_t SlavePayload[MAX_TRANSACTION_SIZE] = { 0x00 };

	uint16_t Size = 0;
};

class BLEDataHandler
{
public:
#pragma GCC diagnostic ignored "-Wunused-parameter"
	virtual void OnDataReceived(uint8_t data[], uint16_t size)
	{
	}
	virtual ~BLEDataHandler()
	{
	}
#pragma GCC diagnostic pop
};

struct Handlers
{
	BLEDataHandler* dataHandler;
};

struct TXQueueItem
{
	uint8_t data[MAX_TRANSACTION_SIZE];
	uint16_t size;
};

class BLEInterface: public HAL::InterruptHandler
{
private:

	static const uint16_t ACTIVEIRQTIMEOUT = delay_us(15);
	static const uint16_t INACTIVEIRQTIMEOUT = delay_ms(10);

	HAL::SPI& m_spi;
	HAL::Pin& m_csn;
	HAL::Pin& m_irq;
	HAL::Pin& m_rstn;
	HAL::OSALEventFlag gotWorkFlag;
	HAL::OSALEventFlag dmaDoneFlag;
	uint8_t m_Timeout;

	SPITransaction transaction;
	BLETransactionStatus XFerData();

	Utils::Queue<TXQueueItem, 5> tx_queue;

	Handlers m_handlers;

public:
	BLEInterface(HAL::SPI& spi, HAL::Pin& csn, HAL::Pin& irq, HAL::Pin& rstn) :
			m_spi(spi), m_csn(csn), m_irq(irq), m_rstn(rstn), gotWorkFlag(HAL::OSALEventFlag()), dmaDoneFlag(
					HAL::OSALEventFlag()), m_Timeout(ACTIVEIRQTIMEOUT), transaction(SPITransaction()), tx_queue(), m_handlers()
	{
	}

	virtual ~BLEInterface()
	{
	}

	/* LOWLevel Hardware Control */
	void Init();
	void Enable();
	void Disable();
	void Reset();

	void RegisterDataHandler(BLEDataHandler* dataHandler)
	{
		m_handlers.dataHandler = dataHandler;
	}

	virtual void ISR();

	void Send(uint8_t data[], uint16_t size);

	void Run();

};

} /* namespace BlueNRG */

#endif /* MIDDLEWARE_BLUENRG_INCLUDE_BLEINTERFACE_H_ */
