/*
 * SPIBASE.h
 *
 *  Created on: 08.09.2015
 *      Author: cem
 */

#ifndef HAL_INCLUDE_SPI_H_
#define HAL_INCLUDE_SPI_H_

#include <stdint.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include "dma.h"

namespace HAL
{

class SPIEventListener
{
public:
	virtual void OnTXEmpty() = 0;
	virtual void OnRXNotEmpty() = 0;
	virtual void OnModeFault() = 0;
	virtual void OnOverrun() = 0;
	virtual void OnFormatError() = 0;

	virtual ~SPIEventListener();
};

class SPI
{
private:
	const uint32_t SPIx;
	const rcc_periph_clken SPIClock;
	const Pin& MOSI;
	const Pin& MISO;
	const Pin& SCK;
	const uint8_t alt_func;
	const DMA& m_RX_DMA;
	const DMA& m_TX_DMA;
public:
	SPI(uint32_t spix, rcc_periph_clken spi_clock, Pin& mosi, Pin& miso, Pin& sck, uint8_t alt_func_num, DMA& rx_dma, DMA& tx_dma) :
			SPIx(spix), SPIClock(spi_clock), MOSI(mosi), MISO(miso), SCK(sck), alt_func(alt_func_num), m_RX_DMA(rx_dma), m_TX_DMA(
					tx_dma)
	{
	}

	inline void Init(uint32_t baudrate, uint32_t cpol, uint32_t cpha, uint32_t dff, uint32_t lsbfirst)
	{
		rcc_periph_clock_enable(SPIClock);

		MOSI.PowerUp();
		MOSI.ModeSetup(GPIO_MODE_AF, GPIO_PUPD_NONE);
		MOSI.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ);
		MOSI.Alternate(alt_func);

		MISO.PowerUp();
		MISO.ModeSetup(GPIO_MODE_AF, GPIO_PUPD_PULLDOWN);
		MISO.SetOutputOptions(GPIO_OTYPE_OD, GPIO_OSPEED_100MHZ);
		MISO.Alternate(alt_func);

		SCK.PowerUp();
		SCK.ModeSetup(GPIO_MODE_AF, GPIO_PUPD_NONE);
		SCK.SetOutputOptions(GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ);
		SCK.Alternate(alt_func);

		spi_init_master(SPIx, baudrate, cpol, cpha, dff, lsbfirst);

	}

	inline void Enable()
	{
		spi_enable(SPIx);
	}

	inline void Disable()
	{
		spi_disable(SPIx);
	}

	inline void CleanDisable()
	{
		spi_clean_disable(SPIx);
	}

	inline void Reset()
	{
		spi_reset(SPIx);
	}

	inline uint16_t Xfer(uint16_t data)
	{
		return spi_xfer(SPIx, data);
	}

	inline void Write(uint16_t data)
	{
		spi_write(SPIx, data);
	}

	inline void Send(uint16_t data)
	{
		spi_send(SPIx, data);
	}

	inline void Send8(uint8_t data)
	{
		spi_send8(SPIx, data);
	}

	inline uint16_t Read()
	{
		return spi_read(SPIx);
	}

	inline uint8_t Read8()
	{
		return spi_read8(SPIx);
	}

	inline void SetBiDirectionalMode()
	{
		spi_set_bidirectional_mode(SPIx);
	}

	inline void SetUniDirectionalMode()
	{
		spi_set_unidirectional_mode(SPIx);
	}

	inline void SetBiDirectionalReiceOnlyMode()
	{
		spi_set_bidirectional_receive_only_mode(SPIx);
	}

	inline void SetBiDirectionalTransmitOnlyMode()
	{
		spi_set_bidirectional_transmit_only_mode(SPIx);
	}

	inline void EnableCRC()
	{
		spi_enable_crc(SPIx);
	}

	inline void DisableCRC()
	{
		spi_disable_crc(SPIx);
	}

	inline void SetNextTXBuffer()
	{
		spi_set_next_tx_from_buffer(SPIx);
	}

	inline void SetNextTXCRC()
	{
		spi_set_next_tx_from_crc(SPIx);
	}

	inline void SetDFF8Bit()
	{
		spi_set_dff_8bit(SPIx);
	}

	inline void SetDff16Bit()
	{
		spi_set_dff_8bit(SPIx);
	}

	inline void SetFullDuplexMode()
	{
		spi_set_full_duplex_mode(SPIx);
	}

	inline void SetReceiveOnlyMode()
	{
		spi_set_receive_only_mode(SPIx);
	}

	inline void DisableSoftwareSlaveManagment()
	{
		spi_disable_software_slave_management(SPIx);
	}

	inline void EnableSoftwareSlaveManagement()
	{
		spi_enable_software_slave_management(SPIx);
	}

	inline void SetNSSHigh()
	{
		spi_set_nss_high(SPIx);
	}

	inline void SetNSSLow()
	{
		spi_set_nss_low(SPIx);
	}

	inline void SendLSBFirst()
	{
		spi_send_lsb_first(SPIx);
	}

	inline void SendMSBFirst()
	{
		spi_send_msb_first(SPIx);
	}

	inline void SetBaudratePrescaler(uint8_t baudrate)
	{
		spi_set_baudrate_prescaler(SPIx, baudrate);
	}

	inline void SetMasterMode()
	{
		spi_set_master_mode(SPIx);
	}

	inline void SetSlaveMode()
	{
		spi_set_slave_mode(SPIx);
	}

	inline void SetClockPolarity1()
	{
		spi_set_clock_polarity_1(SPIx);
	}

	inline void SetClockPolarityLow()
	{
		spi_set_clock_polarity_0(SPIx);
	}

	inline void SetClockPhaseSecondEdge()
	{
		spi_set_clock_phase_1(SPIx);
	}

	inline void SetClockPhaseFristEdge()
	{
		spi_set_clock_phase_0(SPIx);
	}

	inline void EnableTXBufferEmptyInterrupt()
	{
		spi_enable_tx_buffer_empty_interrupt(SPIx);
	}

	inline void DisableTXBufferEmptyInterrupt()
	{
		spi_disable_tx_buffer_empty_interrupt(SPIx);
	}

	inline void EnableRXBufferNotEmpty_Interrupt()
	{
		spi_enable_rx_buffer_not_empty_interrupt(SPIx);
	}

	inline void DisableRXBufferNotEmpty_Interrupt()
	{
		spi_disable_rx_buffer_not_empty_interrupt(SPIx);
	}

	inline void EnableErrorInterrupt()
	{
		spi_enable_error_interrupt(SPIx);
	}

	inline void DisableErrorInterrupt()
	{
		spi_disable_error_interrupt(SPIx);
	}

	inline void EnableSSOutput()
	{
		spi_enable_ss_output(SPIx);
	}

	inline void DisableSSOutput()
	{
		spi_disable_ss_output(SPIx);
	}

	inline void EnableTXDma()
	{
		spi_enable_tx_dma(SPIx);
	}

	inline void DisableTXDma()
	{
		spi_disable_tx_dma(SPIx);
	}

	inline void EnableRXDma()
	{
		spi_enable_rx_dma(SPIx);
	}

	inline void DisableRXDma()
	{
		spi_disable_rx_dma(SPIx);
	}

	inline void SetStandardMode(uint8_t mode)
	{
		spi_set_standard_mode(SPIx, mode);
	}

	inline void SetCRCl8Bit()
	{
		spi_set_crcl_8bit(SPIx);
	}

	inline void SetCRCl16Bit()
	{
		spi_set_crcl_16bit(SPIx);
	}

	inline void SetDataSize(uint16_t data_s)
	{
		spi_set_data_size(SPIx, data_s);
	}

	inline void SetFifoReceptionThreshold8Bit()
	{
		spi_fifo_reception_threshold_8bit(SPIx);
	}

	inline void SetFifoReceptionThreshold16Bit()
	{
		spi_fifo_reception_threshold_16bit(SPIx);
	}

	inline uint16_t GetFlag(uint16_t spi_sr)
	{
		return (SPI_SR(SPIx) & spi_sr);
	}

	inline void EnableSPIMode()
	{
		spi_i2s_mode_spi_mode(SPIx);
	}

	inline const DMA& GetRXDMA()
	{
		return m_RX_DMA;
	}

	inline const DMA& GetTXDMA()
	{
		return m_TX_DMA;
	}

	inline void SetupRXDMA()
	{
		m_RX_DMA.EnableClock();
		m_RX_DMA.ChannelReset();
		m_RX_DMA.SetPeriperalSize(DMA_CCR_PSIZE_8BIT);
		m_RX_DMA.SetMemorySize(DMA_CCR_MSIZE_8BIT);
		m_RX_DMA.EnableMemoryIncrementMode();
		m_RX_DMA.DisablePeripheralIncremtnMode();
		m_RX_DMA.SetReadFromPeripheral();
		m_RX_DMA.SetPeripheralAddress((uint32_t) &SPI_DR8(SPIx));
	}

	inline void SetupTXDMA()
	{
		m_TX_DMA.EnableClock();
		m_TX_DMA.ChannelReset();
		m_TX_DMA.SetPeriperalSize(DMA_CCR_PSIZE_8BIT);
		m_TX_DMA.SetMemorySize(DMA_CCR_MSIZE_8BIT);
		m_TX_DMA.EnableMemoryIncrementMode();
		m_TX_DMA.DisablePeripheralIncremtnMode();
		m_TX_DMA.SetReadFromMemory();
		m_TX_DMA.SetPeripheralAddress((uint32_t) &SPI_DR8(SPIx));
	}

	inline void DisableClearAll()
	{
		m_RX_DMA.ClearInterruptFlags(DMA_TCIF);
		m_TX_DMA.ClearInterruptFlags(DMA_TCIF);

		m_RX_DMA.DisableChannel();
		m_TX_DMA.DisableChannel();

		m_RX_DMA.DisableTransferCompleteInterrupt();
		m_TX_DMA.DisableTransferCompleteInterrupt();

		DisableRXDma();
		DisableTXDma();
	}
};

class SPISelect
{
private:
	HAL::SPI& m_SPI;
	HAL::Pin& m_CSN;
public:
	SPISelect(HAL::SPI& spi, HAL::Pin& csn) :
			m_SPI(spi), m_CSN(csn)
	{
		m_CSN.Off();
	}
	~SPISelect()
	{
		m_SPI.DisableClearAll();
		m_CSN.On();
	}
};

}

#endif /* HAL_INCLUDE_SPI_H_ */
